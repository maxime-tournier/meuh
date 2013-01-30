#include <math/matrix.h>
#include <math/vec.h>

#include <math/algo/stop.h>
#include <math/algo/iter.h>
#include <math/lcp.h>

#include <math/mosek.h>
#include <math/svd.h>

#include <ctime>

#include <Eigen/QR>
#include <Eigen/LU>

#include <math/algo/stop.h>

#include <core/timer.h>

using namespace math;

// rayon spectral ?
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

vec IpS(const vec&z, const vec& v) {
  return v + S(z, v);
}

vec ImS(const vec&z, const vec& v) {
  return v - S(z, v);
}


// converge
vec prout(const mat& Q, const vec& c,
	  const mat& J, const vec& q,
	  
	  const algo::stop& stop) {

  const natural m = Q.rows();
  const natural n = J.rows();

  vec x = vec::Zero(m);
  vec z = vec::Zero(n);
  

  vec gradx, gradz;
  vec Agradx, Agradz;
  
  algo::iter(stop, [&]() {
      
      gradx = Q * x - J.transpose() * IpS(z, z) + c;
      gradz = J * x - q + ImS(z, z);
      
      Agradx = Q * gradx;
      Agradz = gradz;
      
      real gamma = 
      	(gradx.squaredNorm() + gradz.squaredNorm() )
      	/ ( gradx.dot(Agradx) + gradz.dot(Agradz) );
      
      // const real gamma = (gradx.dot(Agradx) + gradz.dot(Agradz)) 
      // 	/  (Agradx.squaredNorm() + Agradz.squaredNorm() );
  
      x -= gamma * gradx;
      z -= gamma * gradz;
      
      // std::cout << "step: " << (gamma * gradx).norm() << std::endl;

    });
  
  return x;
  
  // return clamp(z + z.cwiseAbs());
}




vec gs_bourrin(const mat& Q, const vec& c,
	    const mat& J, const vec& q,
	    
	    const algo::stop& stop) {
  const natural m = Q.rows();
  const natural n = J.rows();
  
  mat M; M.resize(m+n, m+n);

  M <<  
    Q - J.transpose() * J, -J.transpose(),
    - J, -mat::Identity(n, n);
  
  vec b; b.resize(m+n);
  b <<  
    - (J.transpose() * q + c),
    - q;

  vec x = vec::Zero(m+n);
  
  algo::iter(stop, [&]() {
      
      x.each([&](natural i) {
	  x(i) += (b(i) - M.col(i).dot(x) )/ M(i, i);
	  
	  if(i >= m) {
	    b(i) = - (std::max(0.0, x(i)) + q(i - m));
	  }
	});
      
      
    });
  
  return x.head(m);
}

vec bourrin(const mat& Q, const vec& c,
	    const mat& J, const vec& q,
	    
	    const algo::stop& stop) {
  const natural m = Q.rows();
  const natural n = J.rows();
  
  mat M; M.resize(m+n, m+n);

  M <<  
    Q - J.transpose() * J, -J.transpose(),
    - J, -mat::Identity(n, n);

  vec b; b.resize(m+n);
  b <<  
    - (J.transpose() * q + c),
    - q;

  vec x = vec::Zero(m+n);

  mat Mi = math::svd(M).inverse();
  
  algo::iter(stop, [&]() {

      // x = Mi * b;
      // b.tail(n) = - (x.tail(n).cwiseMax( vec::Zero(n) ) + q );

      x.each([&](natural i) {

      	  x(i) = Mi.col(i).dot(b);

      	  if(i >= m ) {
      	    b(i) = - ( std::max(0.0, x(i)) + q(i-m) );
      	  }
      	});
            
    });
  
  return x.head(m);
}
 

vec mitch(const mat& Q, const vec& c,
	  const mat& J, const vec& q,
	    
	  const algo::stop& stop) {
  const natural m = Q.rows();
  const natural n = J.rows();
  
  mat M; M.resize(m+n, m+n);

  M <<  
    Q - J.transpose() * J, -J.transpose(),
    - J, -mat::Identity(n, n);

  vec b; b.resize(m+n);
  b <<  
    - (J.transpose() * q + c),
    - q;

  vec x = vec::Zero(m+n);

  real omega = 1e-8;
  algo::iter(stop, [&]() {

      // x = Mi * b;
      // b.tail(n) = - (x.tail(n).cwiseMax( vec::Zero(n) ) + q );

      x +=  omega * (M*x - b);
      b.tail(n) = - (x.tail(n).cwiseMax( vec::Zero(n) ) + q );
      
    });
  
  return x.head(m);
}
 








vec mosek(const mat& Q, const vec& c,
	  const mat& A, const vec& b,
	  const algo::stop& ) {

  core::timer t;
  math::mosek solver;

  const natural m = Q.rows();
  const natural n = A.rows();
  
  solver.init(m, n);
  // solver.debug( true );

  auto vars = solver.variables();
  auto csts = solver.constraints();
  
  solver.Q(vars, Q );
  solver.c(vars, c );
  
  solver.A(csts, vars, A);
  solver.b(csts, b, mosek::bound::lower);
  
  vec res = solver.solve(vars);
  core::log("solve took", t.start());
  return res;
}





// ca marche en psd mais bourrin
vec fixe(const mat& Q, const vec& c,
	 mat A, vec b,
	 const algo::stop& stop) {

  const natural m = Q.rows();
  const natural n = A.rows();
  
  mat M; M.resize(m+n, m+n);

  real w = 0.1 * real(m) / real(n);

  A = w * A;
  b = w * b;
  
  core::timer t;
  
  M <<  
    Q , A.transpose(),
    A, -mat::Identity(n, n);
  
  auto Mi = M.ldlt();
  
  vec x = vec::Zero(m+n);
  
  core::log("init took", t.start());

  vec q = vec::Zero(m + n);
  algo::iter(stop, [&]() {
      
      q << 
	A.transpose() * x.tail(n).cwiseAbs() - c,
	x.tail(n).cwiseAbs() + b;
      
      x = Mi.solve(q);
      
    });
  
  core::log("iterations took", t.start());
  return x.head(m);
}

vec fast_fixe(const mat& Q, const vec& c,
	 mat A, vec b,
	 const algo::stop& stop) {

  const natural m = Q.rows();
  const natural n = A.rows();
  
  real w = 0.1 * real(m) / real(n);

  A = w * A;
  b = w * b;
  
  core::timer t;

  mat Qi = Q.inverse();
  mat AQi = A*Qi;

  mat K = A * AQi.transpose() + mat::Identity(n, n);
  auto Ki = K.ldlt();

  vec x = vec::Zero(m + n);
  vec q = vec::Zero(m + n);
  
  core::log("init took:", t.start());
  algo::iter(stop, [&]() {
            
      q << 
	A.transpose() * x.tail(n).cwiseAbs() - c,
	x.tail(n).cwiseAbs() + b;
      
      x.tail(n) = Ki.solve( -q.tail(n) + AQi*q.head(m));
      x.head(m) = Qi * (-A.transpose() * x.tail(n) + q.head(m) );
	
    });
  
  core::log("iterations took:", t.start());
  
  return x.head(m);
}



vec furious_fixe(const mat& Q, const vec& c,
	 mat A, vec b,
	 const algo::stop& stop) {

  const natural m = Q.rows();
  const natural n = A.rows();
  
  real w = real(m) / real(n);

  A = w * A;
  b = w * b;
  
  core::timer t;

  mat Qi = Q.inverse();
  mat AQi = A*Qi;

  mat K = A * AQi.transpose() + mat::Identity(n, n);
  auto Ki = K.ldlt();

  vec z = vec::Zero(n);
  vec q = vec::Zero(m + n);
  
  core::log("init took:", t.start());
  algo::iter(stop, [&]() {
            
      q << 
	A.transpose() * z.cwiseAbs() - c,
	z.cwiseAbs() + b;
      
      z = Ki.solve( -q.tail(n) + AQi*q.head(m));
    });
  
  
  core::log("iterations took:", t.start());
  return Qi * (-A.transpose() * z + q.head(m) );

}



vec bokhoven(const mat& Q, const vec& c,
	     mat A, vec b,
	     const algo::stop& stop) {

  const natural m = Q.rows();
  const natural n = A.rows();
  
  real w = 0.1 * real(m) / real(n);

  A = w * A;
  b = w * b;
  
  core::timer t;

  mat Qi = math::svd(Q, 1e-5).inverse();
  mat QiAt = Qi * A.transpose();
  
  mat M = A * QiAt;
  
  auto Li = (M + mat::Identity(n, n)).ldlt();
  
  vec z = vec::Zero(n);
  vec q = b + QiAt.transpose() * c;
  
  vec last;

  core::log("init took:", t.start());
  algo::iter(stop, [&]() {
      last = z;
      z = Li.solve(q + z.cwiseAbs() - M * z.cwiseAbs() );
    });
  
  core::log("iterations took:", t.start());
  
  return Qi * (-c + A.transpose() * (last.cwiseAbs() + z));
}


vec gauss_seidel(const mat& Q, const vec& c,
		 mat A, vec b,
		 const algo::stop& stop) {

  const natural m = Q.rows();
  const natural n = A.rows();
  
  real w = 0.1 * m / n;

  A = w * A;
  b = w * b;
  
  core::timer t;

  mat Qi = Q.inverse();
  mat AQi = A*Qi;

  mat M = A * AQi.transpose();

  vec z = vec::Zero(n);
  vec q = b + AQi * c;
  
  core::log("init took:", t.start());
  algo::iter(stop, [&]() {
      z.each([&](natural i) {
	  z(i) += (q(i) -M.col(i).dot(z))/M(i,i);
	  z(i) = std::max(0.0, z(i));
	});
      
    });
  
  core::log("iterations took:", t.start());
  
  return Qi * (-c + A.transpose() * z);
}

 





mat random_mat(natural n ) {
  natural ker = 2;
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
	     vec solution = vec()) {
  // std::cout << "solution: " << x.transpose() << std::endl; 

  const math::natural m = c.rows();
  const math::natural n = b.rows();

  std::cout << "violation: " << (A * x - b).cwiseMin( vec::Zero(n) ).squaredNorm() / m << std::endl;
  std::cout << "obj: " << x.dot( 0.5 * (Q * x) + c ) << std::endl;

  if(!solution.empty()) {
    core::log("error:", (solution - x).norm() );
  }
  
  std::cout << std::endl;

}


int main(int argc, char** argv ) {			
  std::srand( std::time(0) );

  natural iter = 10;
  if(argc >= 2) { iter = std::stoi(argv[1]); }

  natural n = 10;
  if(argc >= 3) { n = std::stoi(argv[2]); }

  natural m = 5;
  if(argc >= 4) { m = std::stoi(argv[3]); }
  
  const mat Q = 10 * random_mat(n);
  const vec c = 5 * random_vec(n);

  const mat A = 10 * mat::Random(m, n);
  const vec b = 2 * vec::Random(m);
  
  std::cout << "iter: " << iter << ", vars: " << n <<  " cons: " << m << std::endl;
  
  vec solution;
  algo::stop stop;
  stop.it(iter);
  
  core::log("mosek:");
  solution = ::mosek(Q, c, A, b, algo::stop().it(iter) );
  results(Q, c, A, b, solution );
  
  core::log("fixe:");
  results(Q, c, A, b, ::fixe(Q, c, A, b, stop ), solution );
  
  core::log("fast_fixe:");
  results(Q, c, A, b, ::fast_fixe(Q, c, A, b, stop ), solution );

  core::log("furious_fixe:");
  results(Q, c, A, b, ::furious_fixe(Q, c, A, b, stop ), solution );

  core::log("bokhoven:");
  results(Q, c, A, b, ::bokhoven(Q, c, A, b, stop ), solution );

  // core::log("gauss_seidel:");
  // results(Q, c, A, b, ::gauss_seidel(Q, c, A, b, stop ), solution );
  
  
  
}
  
