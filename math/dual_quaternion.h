#ifndef MATH_DUAL_QUATERNION_H
#define MATH_DUAL_QUATERNION_H

#include <math/quaternion.h>
#include <math/vector.h>
#include <math/kinematics.h>
#include <math/pi.h>

#include <cmath>

#include <math/func/polar.h>

namespace math {

  // dual quaternion are supposed to be unitary
  template<class U >
  class dual_quaternion {
    typedef quaternion<U> quat_type;
    typedef vector<U, 3> vec_type;
    typedef vector<U, 6> tangent_type;
    
    quat_type real_, dual_;

    
  public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW;
    
    // when you know what you're doing
    dual_quaternion(const quat_type& r, const quat_type& d) 
      : real_(r), dual_(d) { }

    const quat& real() const{ return real_; }
    const quat& dual() const{ return dual_; }

    quat& real() { return real_; }
    quat& dual() { return dual_; }
    
    dual_quaternion() { }
    
    dual_quaternion( const vec_type& trans, const quat_type& r) 
      : real_(r) { 
      quat_type t;
      t.w() = 0;
      t.vec() = 0.5 * trans;
	
      dual_ = t * real();
    }
      
      
    dual_quaternion operator*( const dual_quaternion& o) const {
      dual_quaternion res;
      res.real() = real() * o.real();
      res.dual().coeffs() = ( dual() * o.real() ).coeffs() + ( real() * o.dual() ).coeffs();
	
      return res;
    }
      
    // TODO implement true inverse in the general case !
    dual_quaternion inverse() const {
      const quat_type rinv = real().inverse();
      
      return dual_quaternion( rinv, -(rinv * dual() * rinv).coeffs() );
    }

    // quaternion conjugate
    dual_quaternion conjugate() const {
      return dual_quaternion( quat_type(real().conjugate()),
			      quat_type(dual().conjugate()));
    }
    
    static dual_quaternion identity() { return { quat_type::identity(), vector<U, 4>::Zero()} ; }
      
    static dual_quaternion translation(const vec_type& t) { 
      dual_quaternion res;
      res.real().setIdentity();
      res.dual().w() = 0;
      res.dual().vec() = t * 0.5;
      return res;
    }
      
    static dual_quaternion rotation( const quat_type& q) {
      dual_quaternion res;
      res.real() = q;
      res.dual().coeffs().setZero();
      return res;
    }

    dual_quaternion operator-() const { 
      dual_quaternion res;
      res.real() = -real().coeffs();
      res.dual() = -dual().coeffs();
      return res;
    }

    vec_type translation() const {
      return 2.0 * (dual() * real().conjugate()).vec();
    }
    
    quat_type rotation() const {
      return real();
    }

    // make this a unit dual_quaternion
    dual_quaternion& unit() {
      const U real_norm = real().norm();
      const U real_inv = 1.0 / real_norm;
      
      const U dual_norm = real().coeffs().dot(dual().coeffs()) * real_inv;
      const U dual_inv = -dual_norm * real_inv * real_inv;
      
      real().coeffs() *= real_inv;
      dual().coeffs() = (real().coeffs() * dual_inv) + (dual().coeffs() * real_inv);
      
      return *this;
    }
    
    dual_quaternion& flip() {
      if( flip_needed() ) *this = -*this;
      return *this;
    }

    
    bool flip_needed() const {
      return real().w() < 0 ;
    }

  };

    
  
  namespace lie { 
    
    template<class U>
    struct traits< dual_quaternion<U> >{
      typedef dual_quaternion<U> dual_quat;
      
      typedef vector<U, 6> algebra;
      typedef covector<U, 6> coalgebra;
      
      dual_quat id() const { return dual_quat::identity(); }
      dual_quat inv(const dual_quat& q) const { return q.conjugate(); }
      dual_quat prod(const dual_quat& q1, const dual_quat& q2) const { 
	return q1.operator*(q2);
      }
      
      struct adjoint {
	typedef adjoint base;
	EIGEN_MAKE_ALIGNED_OPERATOR_NEW;
	
	typedef algebra domain;
	typedef algebra range;

	typename lie::group< quaternion<U> >::adjoint ad;
	vector<U, 3> t;

	adjoint( const dual_quat& g ) 
	: ad(g.rotation()),
	  t(g.translation()){ 
	
	}
      
	
	range operator()(const domain& v) const {
	  range res;
	  
	  angular(res) = ad( angular(v) );
	  linear(res) = ad( linear(v) ) + t.cross( angular(res) );
	  
	  return res;
	}


	mat66 matrix() const {
	  mat66 res;
	  const mat33& R = ad.mat;
	  
	  res << R, mat33::Zero(),
	    hat( t ) * R, R;
	  
	  return res;
	};
      
	
      };


      struct coadjoint {
	typedef coadjoint base;
	
	EIGEN_MAKE_ALIGNED_OPERATOR_NEW;
	
	typedef coalgebra domain;
	typedef coalgebra range;
	
	dual_quat at;
	
	typename lie::group<quaternion<U> >::coadjoint adT;
	vector<U, 3> t;

	coadjoint( const dual_quat& g ) 
	: adT(g.rotation()),
	  t(g.translation()) { 
	}
      
	
	range operator()(const domain& f) const {
	  range res;
	  
	  angular(res) = adT( angular(f) - t.cross(linear(f).transpose()).transpose());
	  linear(res) = adT( linear(f) );
	  
	  return res;
	}

      };


      
    
      struct number {
	U real;
	U dual;

	number operator*(const number& other) const {
	  return {real * other.real,
	      dual * other.real + real * other.dual };
	}

	number cos() const {
	  return {std::cos(real), -dual * std::sin(real) };
	}

	number sin() const {
	  return {std::sin(real), dual * std::cos(real) };
	}
      
	number inv() const {
	  assert( real > epsilon );
	  return { 1.0 / real, - dual / (real * real) };
	}

	bool nan() const { return math::nan(real) || math::nan(dual); }

      };


      class exponential {
	typedef typename group<quaternion<U> >::exponential impl_type;
	impl_type impl;
      
	typedef vector<U, 3> vec3;
      
      public:
      
	exponential(const group<dual_quat>& = group<dual_quat>() ) : impl() { }
	
	typedef algebra domain;
	typedef dual_quat range;
	
	range operator()(const domain& v) const {
	  if( v.squaredNorm() == 0 ) return group<dual_quat>().id();
	  
	  auto w = angular(v);
	  auto at = impl( w );
	  auto body = func::d(impl)(w)( linear(v) );
	  
	  return {at, at * quaternion<U>::im(body)};
	}
	
	struct push {
	
	  typename func::traits<impl_type>::push dimpl;
	  
	  push(const exponential& exp, const domain& at) 
	    : dimpl( d(exp.impl)(at) ) { }
	  
	  
	  // // TODO optimize 
	  // algebra operator()(const algebra& dv) const {
	  
	  //   const algebra& v = dimpl.at;
	
	  //   vec3 real = dimpl(angular(dv));
	  //   vec3 dual = dimpl(linear(dv));
	  
	  //   // decomposes into norm + dir
	  //   const func::polar<U, 3> polar;
	    
	  //   // TODO check suspicious line: angular(dv.body() ? ) => seems legit
	  //   auto pol_dx = polar( T<vec3>(angular(v), angular(dv)));
	  //   auto pol_y =  polar( T<vec3>(angular(v), linear(v)));
	    
	  //   using namespace std;

	  //   const U& theta = get<0>(pol_dx.at());
	  //   const vec3& n = get<1>(pol_dx.at());
      
	  //   const U& dtheta1 = get<0>(pol_dx.body());
	  //   const vec3& dn1 = get<1>(pol_dx.body());

	  //   const U& dtheta2 = get<0>(pol_y.body());
	  //   const vec3& dn2 = get<1>(pol_y.body());
      
	  //   const U s = std::sin(theta);
	  //   const vec3 dtheta2dn1 = dtheta2 * dn1;

	  //   dual += dn2 * dtheta1 - s * s * dn2.cross(dn1) + std::cos(2 * theta) * dtheta2dn1 
	  //     - std::sin(2*theta) * n.cross( dtheta2dn1);

	  //   algebra body;
	
	  //   angular(body) = real;
	  //   linear(body) = dual;
      
	  //   return body;
	  // }

	};



	// // TODO mucho optimize !
	// T<range> diff(const T<domain>& dv) const {
	
	// 	const algebra& v = dv.at();
	
	// 	vec3 real = exp(T<vec3>(angular(v), angular(dv.body())));
	// 	vec3 dual = exp(T<vec3>(angular(v),  linear(dv.body())));
	
	// 	const func::polar<U, 3> pol;
	// 	auto pol_dx = pol(T<vec3>(angular(v), angular(dv)));
	// 	auto pol_y = pol(T<vec3>(angular(v), linear(v)));
	
	// 	using namespace std;

	// 	const U& theta = get<0>(pol_dx.at());
	// 	const vec3& n = get<1>(pol_dx.at());
      
	// 	const U& dtheta1 = get<0>(pol_dx.body());
	// 	const vec3& dn1 = get<1>(pol_dx.body());

	// 	const U& dtheta2 = get<0>(pol_y.body());
	// 	const vec3& dn2 = get<1>(pol_y.body());
      
	// 	const U s = std::sin(theta);
	// 	const vec3 dtheta2dn1 = dtheta2 * dn1;

	// 	dual += dn2 * dtheta1 - s * s * dn2.cross(dn1) + std::cos(2 * theta) * dtheta2dn1 
	// 	  - std::sin(2*theta) * n.cross( dtheta2dn1);

	// 	algebra body;
	
	// 	angular(body) = real;
	// 	linear(body) = dual;
      
	// 	return math::body( (*this)(v), body);
	// };
      
	

      };
    

      class logarithm {
	typename group<quaternion<U> >::logarithm log;
      public:
      
	logarithm(const group<dual_quat>& = group<dual_quat>() ) { }

	typedef dual_quat domain;
	typedef algebra range;
      
	range operator()(const domain& g) const {
	  range res;
	  
	  // decompose dual as q.t (TODO assert w = 0 on result)
	  const vector<U, 3> t = (g.real().conjugate() * g.dual()).vec(); 
	
	  angular( res )  = log( g.real() );
	  linear( res )  = func::d(log)(g.real() )( t );
	  
	  return res;
	}

	// T<range> diff(const T<domain>& ) const {
	//   throw math::error("not implemented lol !");
	// };
      
	// coT<domain> diffT(const pullT<exponential>& ) const {
	//   throw math::error("not implemented lol !");
	// }
      
      };
    
    
      euclid::space<algebra> alg() const { return {}; }

      traits(const dual_quat& ) { }
      traits() { }

    };
  }    
  
  template<class U>
  bool nan(const dual_quaternion<U>& q) {
    return nan(q.real()) || nan(q.dual());
  }

  

}


// Eigen fixes
namespace Eigen {

  template<typename U> struct NumTraits< math::dual_quaternion<U> >
  {
    typedef math::dual_quaternion<U>  Real;
    typedef math::dual_quaternion<U> NonInteger;
    typedef math::dual_quaternion<U> Nested;
    
    enum {
    IsComplex = 0,
    IsInteger = 0,
    IsSigned,
    ReadCost = 1,
    AddCost = 1,
    MulCost = 1
    };
    
  };

}
  
#endif
