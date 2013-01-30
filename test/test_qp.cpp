#include <math/mat.h>
#include <math/vec.h>

#include <plugin/mosek.h>
#include <plugin/load.h>

#include <core/timer.h>

#include <math/qp.h>
#include <math/iter.h>

#include <meta/name.h>

#include <math/opt/bokhoven.h>

#include <script/console.h>

using namespace math;

mat random_mat(natural n ) {
  natural ker = 0;
  mat M = mat::Random(n - ker, n);
  
  mat res =  M.transpose() * M;
  
  return res;
  
}



mat random_vec(natural n) {
  return vec::Random(n);
}


void results(const mat& Q, const vec& c,
	     const mat& A, const vec& b, 
	     const vec& x,
	     vec reference = vec()) {
  // std::cout << "solution: " << x.transpose() << std::endl; 

  using namespace std;
  
  const math::natural m = c.rows();
  const math::natural n = b.rows();

  cout << "violation: " << (A * x - b).cwiseMin( vec::Zero(n) ).squaredNorm() / m << endl;
  cout << "obj: " << x.dot( 0.5 * (Q * x) + c ) << endl;

  if(!reference.empty()) {
    core::log("error:", (x - reference).norm() );
  }
  
  cout << endl;

}






vec mosek(const mat& Q, const vec& c,
	  const mat& A, const vec& b,
	  const iter& ) {
  
  core::timer t_full;
  
  auto solver = plugin::load<plugin::mosek>();
  
  const natural m = Q.rows();
  const natural n = A.rows();
  
  solver->init(m, n);
  // solver.debug( true );

  core::log("mosek:");
  auto vars = solver->variables();
  auto csts = solver->constraints();
  
  solver->Q(vars, Q );
  solver->c(vars, c );
  
  solver->A(csts, vars, A);
  solver->b(csts, b, mosek::bound::lower);
  
  core::timer t_solve;
  vec res = solver->solve(vars);
  core::log("solve:", t_solve.start(), "full:", t_full.start());
  return res;
} 




struct test {
  
  const mat Q;
  const mat A;
  
  vec solve(const vec& c, const vec& b, const iter& it) {
    natural m = c.size();
    natural n = b.size();
    
    vec at = vec::Zero( m + n );
    
    vec delta = vec::Zero( m + n );
    vec eps = vec::Constant(n, it.epsilon);
    vec log_eps = eps.array().log();

    vec exp;
    
    auto kkt = [&](const math::vec& dx) {
      
      assert(!nan(dx));
      vec res = vec::Zero(m + n);
      
      res.head(m) = Q * dx.head(m) - A.transpose() * exp.cwiseProduct( dx.tail(n) );
      res.tail(n) = -exp.cwiseProduct( A * dx.head(m) ) - eps.cwiseProduct(dx.tail(n));
      
      // core::log("res:", res.transpose());
      
      assert(!nan(res));
      
      return res;
    };
    
    vec rhs = vec::Zero( m + n );
    
    math::iter sub;
    sub.bound = 1 + std::log(m + n);
    sub.bound = it.bound;
    
    math::natural k = 0;
    it.go([&] {
	exp = at.tail(n).array().exp();
	assert(!nan(exp));
	
	rhs << -c - Q * at.head(m) + A.transpose() * exp,
	  -exp.cwiseProduct( b - A * at.head(m) ) - eps;
	
	assert(!nan(rhs));
	
	delta = math::minres(kkt).solve(rhs, sub);

	at += delta;

	++k;
	return rhs.norm();      
      });
    
    return at.head(m);
  }

};



struct bokhoven {
  
  const mat Q;
  const mat A;
  
  vec solve(const vec& c, const vec& b, const iter& it) const {
    
    auto AT = [&](const vec& x) { return A.transpose() * x; };

    auto solver =  math::opt::qp::make_bokhoven(Q, A, AT);
    
    // solver.eps = math::epsilon;

    return solver.solve(c, b, it);
  }
    
};


  
struct bokhoven_taiste {

  const mat Q;
  const mat A;

  vec solve(const vec& c, const vec& b, const iter& it) const {
    natural m = c.size();
    natural n = b.size();
  
    // inactive constraints
    vec mask = vec::Zero(n);

    vec res = vec::Zero(m + 2 * n);
    
    auto update = [&]() -> natural {
      natural changed = 0;

      mask.each([&](natural i)  {
	  real old = mask(i);
	  
	  mask(i) = res.tail(n)(i) > 0 ? 1 : 0;
	  	  
	  if( old != mask(i) ) ++changed;
	});
      return changed;
    };
    
    vec storage = vec::Zero( m + 2 * n );
    
    auto S = [&](const vec& x) {
      return 2 * mask.cwiseProduct(x) - x;
    };
    
    auto kkt = [&](const vec& x) -> const vec& {
      
     
      storage <<
      Q(x.head(m) ) - A.transpose() * x.segment(m, n),
      -A(x.head(m)) - x.segment(m, n) + S( x.tail( n ) ),
      S( x.segment(m, n) ) - mask.cwiseProduct(x.tail(n))
      ;
      
      return storage;
    };

    mask = ((A(res.head(m)) - b).array() < 0).cast< real > ();
    
    vec rhs(m + 2 * n);
    rhs << -c, -b, vec::Zero(n);
    
    math::impl::minres minres;
    minres.init(rhs - kkt(res));

    math::iter outer = it;
    math::iter inner;
    inner.bound = 1 + std::log(m + 2 * n);

    core::log("inner:", inner.bound);
    // inner.bound = outer.bound;
    
    natural k = 0;
	  
    // std::swap(inner, outer);
    // std::swap(inner.cb, outer.cb);
    
    outer.go([&]() -> real  {
	      
	inner.go([&]() -> real  {
	    minres.step(res, kkt);
	    return minres.phi;
	  });
	      
	if( update() ) {

	  vec primal = S( A(res.head(m)) - b + res.segment(m, n));
	  
	  for(natural j = 0; j < 3; ++j) {
	    mask.each([&](natural i) {
		
	    	if( mask(i) ) {
	    	  res.tail(n)(i) = res.segment(m, n)(i);
	    	} else {
	    	  res.tail(n)(i) = primal(i);
	    	  // res.segment(m, n)(i) = 0;
		  
	    	  // what if > 0 ?
	    	}
	      });
	    // res.tail(n) = -S( primal + res.segment(m, n) );
	    update();	    
	  }

	  
	  // core::log("k:", k, mask.transpose() );

	  storage = rhs - kkt(res);
	  minres.init( storage );
	}
	      
	++k;
	return minres.phi;
      });
    
    return res.head(m);
  }
  
};


struct active_new {
  
  const mat Q;
  const mat A;
  
  vec solve(const vec& c, const vec& b, const iter& it) {
    natural m = c.size();
    natural n = b.size();
    
     
    vec res = vec::Zero( m + 2 * n);
    
    active_set active( n );

    // auto update = [&] {
      
    //   natural changed = 0;
      
    //   mask.each([&](natural i) {
	  
    // 	  real old = mask(i);
	  
    // 	  mask(i) = res.segment(m, n)(i) < 0;
	  
    // 	  if( mask(i) != old ) ++changed;
    // 	});

    //   return changed;
    // };
    
    
    auto U = [&](const vec& x) {
      return (vec::Ones(n) - active.mask).cwiseProduct(x);
    };
    
    vec storage = vec::Zero(m + 2 * n);
     
    auto kkt = [&](const vec& x) -> const vec&  {
      
      storage << 
      
      Q(x.head(m)) - A.transpose() * x.segment(m, n),
      -A(x.head(m)) + U(x.tail(n)),
      U(x.segment(m, n));
      
      return storage;
    };
    
    iter outer = it;

    iter inner;
    inner.bound = 1 + std::log( m + 2 * n );
    
    vec rhs = vec::Zero( m + 2 * n );
    rhs << -c, -b, vec::Zero(n);
    
    impl::minres minres;
    minres.init( rhs - kkt(res) );
    
    outer.go([&] {
	
	inner.go([&] {
	    minres.step(res, kkt);
	    return minres.phi;
	  });
	
	vec primal = minres.r.segment(m, n) + U(res.tail(n)); // A(res.head(m)) - b;
	vec dual = res.segment(m, n);
	
	if( active.update(primal, dual) ) {
	  // res.tail(n) = U(primal.tail(n));
	  
	  // res.segment(m, n) = active.filter(res.segment(m, n));
	  minres.init(rhs - kkt(res));
	}
	
	return minres.phi;
      });
    
    return res.head(m);
  }
  
};



 
vec taiste(const mat& Q, const vec& c,
	   const mat& A, const vec& b,
	   const iter& it ) {
   core::log("taiste");
   
   test solver{Q, A};

   vec res = solver.solve(c, b, it);
   
   return res;
}

template<class Solver>
vec bench(const mat& Q, const vec& c,
	  const mat& A, const vec& b,
	  const iter& it ) {
  std::string name = meta::name<Solver>();
  
  core::log( name );
  script::exec("table.insert(res.title, '%%')", name);
  
  natural m = c.size();
  natural n = b.size();
  
  natural s = m + n;
  
  Solver solver{Q, A};
  
  core::timer t;  
  vec res = solver.solve(c, b, it);
  core::log("time:", t.start());
  
  return res; 
}



// vec simple(const mat& Q, const vec& c,
// 	   const mat& A, const vec& b,
// 	   const iter& it ) {
//    core::log("simple");
   
//    auto q = [&](const vec& x)  {
//      return Q * x;
//    };
   
//    auto a = [&](const vec& x)  { return A * x; };
//    auto aT = [&](const vec& x) { return A.transpose() * x; };
   
//    vec res = math::lol::simple(q, a, aT).solve(c, b, it, vec(), 0);

//    return res;
// }


struct minres_old {
  const mat Q;
  const mat A;

  vec solve(const vec& c, const vec& b, const iter& it ) const {
    
    auto AT = [&](const vec& x) { return A.transpose() * x; };
  
    auto res =  math::qp(Q, A, AT).solve_old(c, b, it);
    
    return res;
  }


};


struct minres {
  const mat Q;
  const mat A;
  
  vec solve(const vec& c, const vec& b, const iter& it ) const  {
    
   
    auto AT = [&](const vec& x) { return A.transpose() * x; };
  
    core::timer t; 
    auto res =  math::qp(Q, A, AT).solve(c, b, it);

    return res;
  }
};


int main(int argc, char** argv ) {		
  std::srand( std::time(0) );

  natural iter = 10;
  if(argc >= 2) { iter = std::stoi(argv[1]); }

  // dofs
  natural n = 10;
  if(argc >= 3) { n = std::stoi(argv[2]); }

  // constraints
  natural m = 5;
  if(argc >= 4) { m = std::stoi(argv[3]); }

  mat Q = 10 * random_mat(n);
  vec c = 5 * random_vec(n);

  mat A = 10 * mat::Random(m, n);
  vec b = 2 * vec::Random(m);
  
  core::log("iter:", iter, "dim:", n, "constraints:", m);

  math::iter it;
  it.bound = iter;
  it.epsilon = math::epsilon;
  
  script::exec("plot = require 'plot'");
  script::exec("res = plot.new()");
  
  script::exec("res.title = {}");
  script::exec("res.set.logscale = 'y'");
  
 
  math::iter ref = it;
  ref.bound = m + n;
  
  vec reference = ::minres{Q, A}.solve(c, b, ref);

  // now for the plots
  it.cb = [&](int k, real eps) {
    script::exec("plot.push(res, %%, %%)", k, eps);
  };

  results(Q, c, A, b, bench< ::minres >(Q, c, A, b, it), reference );
  // results(Q, c, A, b, bench< bokhoven >(Q, c, A, b, it), reference );
  results(Q, c, A, b, bench< bokhoven_taiste >(Q, c, A, b, it), reference );
  // results(Q, c, A, b, bench< active_new >(Q, c, A, b, it), reference );
  
  // results(Q, c, A, b, ::simple(Q, c, A, b, it), reference );
  
  script::exec("print('preparing plot...'); plot.show( res )");

  script::console console;
  // console();
}
 
 
