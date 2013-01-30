#include <math/mat.h>
#include <math/vec.h>

#include <plugin/mosek.h>
#include <plugin/load.h>

#include <core/timer.h>

#include <math/qp.h>
#include <math/iter.h>

#include <meta/name.h>

#include <math/opt/bokhoven.h>
#include <math/pgs.h>

using namespace math;

mat random_psd(natural n, natural ker = 0) {
  mat M = mat::Random(n - ker, n);
  
  mat res =  M.transpose() * M;
  
  return res;
  
}



mat random_vec(natural n) {
  return vec::Random(n);
}

real error(const mat& M, const vec& q,
	   const vec& x) {
  
  return (M * x + q).dot(x);
  
}


void results(const mat& M, const vec& q, 
	     const vec& x,
	     vec reference = vec()) {
  // std::cout << "solution: " << x.transpose() << std::endl; 
  using namespace std;
  
  const math::natural n = q.rows();
  
  core::log("primal:", (M * x + q).cwiseMin( vec::Zero(n) ).squaredNorm() );
  core::log("dual:", x.cwiseMin( vec::Zero(n) ).squaredNorm() );
  
  if(!reference.empty()) {
    core::log("error:", (x - reference).norm() );
  }
  
  core::log("");
}


template<class Solver>
vec bench(const mat& M, const vec& q,
	  const iter& it ) {
  core::log(meta::name<Solver>() );
  
  
  Solver solver{M};
  
  core::timer t;  
  vec res = solver.solve(q, it);
  core::log("time:", t.start());
  
  return res; 
}


vec minres(const mat& M, const vec& q, iter it) {

  auto id = [&](const vec& x) { return x; };
  
  auto qp = math::qp(M, id, id);

  core::timer t;  
  vec res = qp.solve(q, vec::Zero(q.rows()), it);
  core::log("time:", t.start());
 
  return res;
}


struct bokhoven {
  mat M;

  vec solve(const vec& q, iter it) const {

    vec res = vec::Zero(q.size());

    it.go([&] {

	real eps = 0;
	
	res.each([&](natural i) {
	
	    real old = res(i);
	    res(i) = ( -q(i) - M.col(i).dot(res + res.cwiseAbs()) + M(i, i) * res(i) + std::abs(res(i)) ) 
	      / (1 + M(i, i));
	    
	    // projection
	    // res(i) = std::max(0.0, res(i));
	    
	    real delta = res(i) - old;

	    eps += delta * delta;
	  });

	
	return std::sqrt( eps );
      });
    
    return res + res.cwiseAbs();
  }
    
};

 
int main(int argc, char** argv ) {		
  std::srand( std::time(0) );

  natural iter = 10;
  if(argc >= 2) { iter = std::stoi(argv[1]); }

  // dofs
  natural n = 10;
  if(argc >= 3) { n = std::stoi(argv[2]); }

  mat M = random_psd(n);
  vec q = random_vec(n);
  
  core::log("iter:", iter, "dim:", n);
  
  math::iter it;
  it.bound = iter;
  it.epsilon = math::epsilon;
  
  script::exec("plot = require 'plot'");
  script::exec("res = plot.new()");
  
  script::exec("res.title = { 'minres', 'pgs', 'pbokhoven' }");
  script::exec("res.set.logscale = 'y'");
  
  it.cb = [&](int k, real eps) {
    script::exec("plot.push(res, %%, %%)", k, eps);
  };
  
  vec reference = vec::Zero( n );
  
  
  math::iter ref = it;
  ref.bound = std::max(n, it.bound);
  
  reference = ::minres(M, q, ref);
  
  results(M, q, reference );
  results(M, q, bench<math::pgs>(M, q, it), reference );
  results(M, q, bench< ::bokhoven>(M, q, it), reference );
 
  script::exec("plot.show( res )");
}
 
 
