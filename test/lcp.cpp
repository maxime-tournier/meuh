#include <math/matrix.h>
#include <math/vec.h>

#include <math/algo/stop.h>
#include <math/algo/iter.h>
#include <math/algo/pgs.h>
#include <math/algo/cg.h>
#include <math/lcp.h>

#include <math/mosek.h>
#include <core/timer.h>

#include <ctime>

#include <Eigen/Cholesky>
#include <Eigen/LU>

#include <plugin/mosek.h>

#include <memory>
#include <script/console.h>

using namespace math;

real spec(const mat& M, natural ) {
  const natural n = M.rows();
  vec x = vec::Random(n).normalized();
  vec Mx = M*x;

  real ro = 0;
  for(int i = 0; i < 15; ++i) {
    x = Mx.normalized();
    Mx = M*x;
    
    ro = x.dot( Mx );
 
  }

  return ro;
}

vec S(const vec& z, const vec& v) {
  vec res; res.resize(v.rows());

  v.each([&](natural i) {
      res(i) = z(i) >= 0 ? v(i) : -v(i);
    });
    
  return res;
}


vec prout(const mat& M, const vec& q, const algo::stop& stop) {
  const natural n = q.rows();

  vec z = vec::Zero(n);
 
  const mat A = M + mat::Identity(n, n);
  
  vec zabs, b, grad;

  algo::iter(stop, [&]() {
      zabs = z.cwiseAbs();
      b = zabs - M * zabs - q;
            
      grad = A*z - b;

      const vec Ag = A*grad;
      const real gamma = grad.squaredNorm() / grad.dot( Ag );
      // const real gamma = grad.dot(Ag) / Ag.squaredNorm(); 
	
      z -= gamma * grad;
            
    });
  

  return z + z.cwiseAbs();
}

vec gs_prout(const mat& M, const vec& q, const algo::stop& stop) {
  const natural n = q.rows();

  vec z = vec::Zero(n);
  real ro = spec(M, 5);

  real omega = 4/ro;
    
  vec zabs = z;
  vec zpzabs = z + zabs;
  
  algo::iter(stop, [&]() {
      z.each([&](natural i) {
	  
	  const real omegaMii = omega * M(i, i);
	  z(i) = (  - omega * M.row(i).dot(zpzabs) + omegaMii * z(i) + std::abs(z(i)) - omega * q(i) )
	    / ( 1 + omegaMii );
	  
	  zabs(i) = std::abs(z(i));
	  zpzabs(i) = z(i) + zabs(i);
	});
    });
  

  return z + z.cwiseAbs();
}




vec conj_prout(const mat& M, const vec& q, const algo::stop& stop) {
  const natural n = q.rows();

  vec z = vec::Zero(n);

  mat A = M + mat::Identity(n, n);
  
  // const vec d = A.diagonal().array().sqrt().inverse();
  // A = d.asDiagonal() * A * d.asDiagonal();
   
  vec zabs  = z.cwiseAbs();
  vec b = zabs - M * zabs - q;
  vec grad = A*z - b;
  
  // descent direction
  vec p = -grad;
  vec Ap = A*p;

  algo::iter(stop, [&]() {
      // line search along p
      const real gamma = -grad.dot(p) / p.dot( Ap );
      // const real gamma = grad.squaredNorm() / p.dot( Ap );
      // const real gamma = - grad.dot( A*grad ) / (A*grad).dot(Ap);
      
      // next iteration
      z += gamma * p;
      
      // update values
      zabs  = z.cwiseAbs();
      b = zabs - M * zabs - q;
      grad = A*z - b;
      
      // A-conjugate grad with previous direction
      const real beta = grad.dot( Ap ) / p.dot(Ap );
      p = -grad + beta * p;

      Ap = A*p;
    });
  
  

  return z + z.cwiseAbs();
}





vec chab(const mat& M, const vec& q, const algo::stop& stop) {
  const natural n = q.rows();

  vec z = vec::Zero(n);
  vec zabs = vec::Zero(n);
  
  real ro = M.trace();
  ro = spec(M, 10);
  
  vec grad, Kgrad;
  
  real omega = 2/ro;
  algo::iter(stop, [&]() {
      
      // zabs = z.cwiseAbs();
      // grad = M * (z + zabs) + q;
      
      // Kgrad = M * (grad + S(z, grad) );
      // real gamma = -grad.dot(Kgrad) / Kgrad.squaredNorm();

      // z += gamma * grad;
      // z += -grad / ro;

      z.each([&](natural i) {
      	  z(i) -= omega * ( M.row(i).dot(z + zabs) + q(i)  );
      	  zabs(i) = std::abs( z(i) );
      	});
      
    });
  
  return (z.cwiseAbs() + z);
}



vec michboule(const mat& M, const vec& q, const algo::stop& stop) {
  const natural n = q.rows();

  vec z = vec::Zero(n);
 
  vec s = vec::Ones(n);



  real ro = spec(M, 10);
  
  vec tmp;
  real omega = 2/ro;
  
  algo::iter(stop, [&]() {
      // zabs = z.cwiseAbs();
      // Az = M * (z + zabs) + (z - zabs);
      
      for(natural i = 0; i < n; ++i) {
	tmp = z.array() + s.array() * z.array();
	z(i) -= omega * (q(i) +  M.row(i).dot(tmp) + (1 - s(i))*z(i) );
	s(i) = (z(i) > 0 ? 1 : -1 );
      }
      
      // z += omega * ( b - A(z) );
      
      // grad = A(z) - b;
      
      // const real gamma = grad.squaredNorm() / grad.dot( A(grad) );
      // z -= gamma * grad;
      
    });
  
  return (z.cwiseAbs() + z);
}






// marche: prendre f(z) (= 0) comme direction de descente et minimiser
// ||f(z)||^2 pour le line search
vec doubidou(const mat& M, const vec& q, const algo::stop& stop) {
  const natural n = q.rows();

  vec z = vec::Zero(n);
  
  auto IpS = [&](const vec& v) ->vec { return v + S(z, v); };
  
  auto A = [&](const vec& v) -> vec {
    const vec Sv = S(z, v);
    // return M * ( v + Sv ) + v - Sv;
    return IpS( M * IpS(v) ); 
  };
  

  real ro = spec(M, 10);
  
  vec zabs;
  vec Az;
  vec b = -q;

  vec grad;

  algo::iter(stop, [&]() {
      
      grad = A(z) - IpS(b);

      vec Ag = A(grad);
      const real Ag2 = Ag.squaredNorm();

      real gamma = 1/(4 * ro);
      
      // // gamma = grad.dot(Ag) / Ag2;
      // gamma = grad.squaredNorm() / grad.dot(Ag);
      
      z -= gamma * grad;
      
      // std::cout << "step: " << (gamma * grad).norm() << std::endl;
    });
  
  return (z.cwiseAbs() + z);
}







vec bizarre(const mat& M, const vec& q, const math::iter& iter) {

  const natural n = q.rows();
  
  vec z = vec::Zero(n);

  auto psd = [&] ( const vec& x) {
    return x + M * x;
  };


  auto rhs = [&](const vec& x) {
    return -q + x.cwiseAbs() - M * x.cwiseAbs();
  };
  
  vec r = rhs(z) - psd(z);
  vec p = r;

  vec s = p;
  vec As = psd(s);
  real sAs = s.dot(As);
  
  // vec v = s;
  // vec Av = As;
  // real vAv = v.dot(Av);
  

  script::exec("p = plot.new()");
  script::exec("p.data = {}");
  script::exec("p.plot['1:2'] = { with = 'lines' }");
  
  natural i = 0;
  iter.go( [&] {
      
      vec b = rhs(z);

      for( natural j = 0; j < 1; ++j) {
	vec Ap = psd(p);
	real pAp = p.dot(Ap);
      
	real alpha = r.dot( p ) / pAp;
      
	z += alpha * p;

	r = b - psd(z);
      
	real eps = r.norm();
	vec next = r;
      
	next -= (next.dot(Ap) / pAp) * p;
	next -= (next.dot(As) / sAs) * s;
	// next -= (next.dot(Av) / vAv) * v;
      

	// v = s;
	// Av = As;
	// vAv = sAs;

	s = p;
	As = Ap;
	sAs = pAp;

	p = next;
      }

      script::exec("p.data[ %% ] = {%%, %%}", i+1, i, (b - psd(z)).norm());      
      ++i;
    });

  return z + z.cwiseAbs();
}



vec pcg(const mat& M, const vec& q, const math::iter& iter) {

  const natural n = q.rows();
  
  vec x = vec::Zero(n);
  vec y = x;
  
  auto psd = [&](const vec& x) { return M * x; };

  vec b = -q;
  vec r = b - psd(x);
  vec p = r;
  
  vec s = p;
  vec As = psd(s);
  real sAs = s.dot(As);
  
  script::exec("pcg = plot.new()");
  script::exec("pcg.data = {}");
  script::exec("pcg.plot['1:2'] = { with = 'lines' }");
  
  natural i = 0;
  iter.go( [&] {
      
      vec Ap = psd(p);
      real pAp = p.dot(Ap);
      
      real alpha = r.dot( p ) / pAp;
      
      y += alpha * p;
      
      vec proj = y;
      proj.each([&](natural j) { if( proj(j) < 0 ) proj(j) = 0; } );
      
      vec g = proj - x;
      real beta = g.dot( psd(y - x) ) / g.dot(psd(g));
      
      // beta = std::min(1.0, beta);
      // beta = std::max(0.0, beta);
      beta = 1;

      x += beta * g;
      // x = target;

      r = b - psd(y);
      
      real eps = r.norm();
      vec next = r;
      
      // next -= (next.dot(Ap) / pAp) * p;
      // next -= (next.dot(As) / sAs) * s;
      
      // s = p;
      // As = Ap;
      // sAs = pAp;
      
      p = next;
      
      script::exec("pcg.data[ %% ] = {%%, %%}", i+1, i, x.dot(M * x) + q.dot(x) );      
      ++i;
    });
  
  return x;
}



vec bokhoven(const mat& M, const vec& q, const math::iter& iter) {
  const natural n = q.rows();
  vec z = vec::Zero( n );

  const mat Ki = (mat::Identity(n, n) + M).inverse();

  script::exec("q = plot.new()");
  script::exec("q.data = {}"); 
  script::exec("q.plot['1:2'] = { with = 'lines' }");
  
  int i = 0;
  iter.go([&] {
      vec rhs = (z.cwiseAbs() - M * z.cwiseAbs() - q);
      script::exec("q.data[ %% ] = {%%, %%}", i+1, i, ( z + M * z - rhs).norm() );

      vec dir = -z + Ki * (z.cwiseAbs() - M * z.cwiseAbs() - q);

      real alpha = 1;
      vec tmp = -z.cwiseProduct( dir.cwiseInverse() );

      tmp.each([&](natural j) {
	  if( tmp(j) <= 0 ) return;

	  if (tmp(j) < alpha ) {
	    alpha = tmp(j);
	  }
	});

      // alpha = 1;
      z += alpha * dir;

      ++i;
    });

  return z + z.cwiseAbs();

}

vec newton(const mat& M, const vec& q, const math::iter& iter) {
  const natural n = q.rows();
  vec z = vec::Zero( n );
  
  script::exec("r = plot.new()");
  script::exec("r.data = {}");
  script::exec("r.plot['1:2'] = { with = 'lines' }");
  
  auto I = mat::Identity(n , n);

  int i = 0;
  iter.go([&] {

      vec rhs = z + M * z - z.cwiseAbs() + M.cwiseAbs() + q;

      vec p = vec::Ones(n);
      z.each([&](natural j) { if( z(j) < 0 ) p(j) = -1; } );

      auto ones = vec::Ones(n);

      mat P = p.asDiagonal();
      mat Ki = ( I + M  -  (I - M) * P ).inverse();
      
      z -= 0.01 * Ki * rhs;

      script::exec("r.data[ %% ] = {%%, %%}", i+1, i, rhs.norm() );
      
      ++i;
    });

  return z + z.cwiseAbs();

}


vec mosek(const mat& M, const vec& q, const algo::stop& stop) {
  
  auto solver = core::plugin::load<plugin::mosek>();
  
  const natural n = q.rows();
  
  solver->init(n, n);
  solver->debug( true );

  auto vars = solver->variables();
  auto csts = solver->constraints();
  
  // min xMx + qx
  solver->Q(vars,  M );
  solver->c(vars, q );
  
  // Mx + q > 0
  solver->A(csts, vars, M);
  solver->b(csts, -q, math::mosek::bound::lower);
  
  // x > 0
  solver->bounds(vars, vec::Zero(n), math::mosek::bound::lower );
  
  return solver->solve(vars);
}






mat random_mat(natural n ) {
  mat M = mat::Random(2 * n, n);
  
  // maybe scale columns according to desired eigenvalue ?
  mat res =  M.transpose() * M;

  return res;
  
}

mat random_vec(natural n) {
  return 10 * vec::Random(n);
}


void results(const mat& M, const vec& q, const vec& x) {
  std::cout << "solution: " << x.transpose() << std::endl; 
  const math::natural n = x.rows();

  std::cout << "neg solution: " <<  x.cwiseMin( vec::Zero(x.rows()) ).squaredNorm()/n << std::endl;
  std::cout << "neg slack: " <<  (M * x + q ).cwiseMin( vec::Zero(x.rows()) ).squaredNorm()/n << std::endl;
  
  std::cout << "obj: " << x.dot( M * x + q ) << std::endl;
 
  
}


int main(int argc, char** argv ) {			
  std::srand( std::time(0) );

  natural iter = 10;
  if(argc >= 2) { iter = std::stoi(argv[1]); }

  natural n = 10;
  if(argc >= 3) { n = std::stoi(argv[2]); }

    

  std::cout << "iter: " << iter << ", n: " << n << std::endl;


  script::require("plot");
  script::require("pretty");
  
  script::api api("app");
  
  api.fun("go", [&] {

      const mat M = 10 * random_mat(n);
      const vec q = random_vec(n);

      std::cout << "mosek: " << std::endl;
      results(M, q, ::mosek(M, q, algo::stop().it(iter) ) );
      std::cout << std::endl;
      
      core::log("bizarre");
      results(M, q, ::bizarre(M, q, math::iter(iter, 0) ) );
      core::log();

      core::log("bokhoven");
      results(M, q, ::bokhoven(M, q, math::iter(iter, 0) ) );
      core::log();

      core::log("pcg");
      results(M, q, ::pcg(M, q, math::iter(iter, 0) ) );
      core::log();
      
      //  core::log("newton");
      // results(M, q, ::newton(M, q, math::iter(iter, 0) ) );
      // core::log();
  
      script::exec( "plot.show(pcg)");
    })
    .fun("iter", [&](natural n) { iter = n; } )
    ;
  
  script::exec("go = api.app().go");
  script::exec("go()");
  
  script::console console;
  console();


}
  
