#ifndef MATH_ROTATION_H
#define MATH_ROTATION_H

#include <math/lie.h>
#include <math/quaternion.h>

#include <math/func.h>
#include <math/func/pull.h>
#include <math/func/push.h>

namespace math {

  
  template<class U >
  class rotation  {  

    typedef vector<U, 3> vec_type;
    typedef math::quaternion<U> quat_type;
    typedef math::matrix<U, 3, 3> mat_type;
    
    quat_type q_;
    
        
  public:
    
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW;

    typedef vec_type domain;
    typedef vec_type range;

    rotation(const quat_type& q) :q_(q) { }
    rotation() { }
    
    const quat_type& quaternion() const { return q_; }
    quat_type& quaternion() { return q_; }
      
    mat_type matrix() const { return q_.toRotationMatrix(); }
    
    rotation operator*(const rotation& o) const { return lie::prod(q_, o.q_); }
    rotation inverse() const { return lie::inv(q_); }

    range operator()(const domain& x) const { return q_ * x; }
    
    // T<range> diff(const T<domain>& dx) const { return body((*this)(dx.at()), (*this)(dx.body()) ); }
    struct push : func::base< rotation > {
      push(const rotation& of, const domain& ) : rotation(of) { }
    };

    
    static rotation identity() { return quat_type::identity(); }
    
    // returns euler angles corresponding to zyx composition
    vec_type zyx() const {
      mat_type m = matrix();
      if( std::abs(m(2,0)) != 1) {
	U theta1 = -std::asin(m(2, 0));
	U theta2 = pi - theta1;
	
	U psi1 = std::atan2(m(2,1)/std::cos(theta1),
			    m(2,2)/std::cos(theta1));
	U psi2 = std::atan2(m(2,1)/std::cos(theta2),
			    m(2,2)/std::cos(theta2));

	U phi1 = std::atan2(m(1,0)/std::cos(theta1),
			    m(0,0)/std::cos(theta1));
	U phi2 = std::atan2(m(1,0)/std::cos(theta2),
			    m(0,0)/std::cos(theta2));
	return vec_type(psi1, theta1, phi1);
      } else {
	U psi = 0;
	if( m(2, 0) == -1 ) {
	  U theta = pi / 2;
	  U phi = psi + std::atan2(m(0,1), m(0, 2));
	  return vec_type(psi, theta, phi);
	} else {
	  U theta = - pi/2;
	  U phi = -psi + std::atan2(-m(0,1), -m(0, 2));
	  return vec_type(psi, theta, phi);
	}
	
      }
    }

    static rotation euler(const vec_type& angles, const char* order = "zyx") {
      rotation res = identity();
      
      const auto exp = lie::group<rotation>().exp();
      
      for(natural i = 0; i < 3; ++i) {
	rotation delta;
	switch(order[i]) {
	case 'x' : delta = exp( vec_type::UnitX() * angles.x() );
	  break;
	case 'y' : delta = exp( vec_type::UnitY() * angles.y() );
	  break;
	case 'z' : delta = exp( vec_type::UnitZ() * angles.z() );
	  break;
	default: throw math::error("derp");
	}
	res = res * delta;
      }
      return res;
    }
    
  };

  

  
  namespace lie {
    
    template<class U>
    struct traits< rotation<U> > {
      
      const group< quaternion<U> > lie;
      
      typedef rotation<U> rot;
      
      typedef vector<U, 3> algebra;
      typedef covector<U, 3> coalgebra;
    
      rot id() const  { return rot::identity(); }
      rot inv(const rot& r)  const { return r.inverse(); }
      rot prod(const rot& a, const rot& b) const { return a * b; }
    
      euclid::space< algebra > alg() const { return {}; }
      
      traits(const rot& ) : lie() { }
      traits() : lie() { }
      
      struct adjoint : func::base<typename lie::group< quaternion<U > >::adjoint> {
	
	adjoint(const rot& at) : adjoint::base(at.quaternion()) { }
	
	
      };


      struct coadjoint : func::base<typename lie::group< quaternion<U > >::coadjoint> {
	coadjoint(const rot& at) : coadjoint::base(at.quaternion()) { }
	
      };
    
      class exponential {
	typedef typename group<quaternion<U> >::exponential impl_type;
	impl_type impl;
      public:
	exponential(const group<rot>& = group<rot>()) : impl() { }
	
	typedef algebra domain;
	typedef rot range;
      
	range operator()(const domain& v) const {
	  return impl( v / 2.0 );
	}

	struct push {
	  typename func::traits<impl_type >::push dimpl;
	  
	  push(const exponential& exp, const domain& at) 
	    : dimpl( func::d(exp.impl)(at / 2)) { }
	
	  algebra operator()(const algebra& v) const {
	    return dimpl(v);
	  }
	
	};


	struct pull {
	  typename func::traits<impl_type >::pull dTimpl;

	  pull(const exponential& exp, const domain& at) 
	    : dTimpl( func::dT(exp.impl)(at / 2)) { }
	
	  coalgebra operator()(const coalgebra& f) const {
	    return dTimpl(f);
	  }
	};
	
      };
    
      
      class logarithm {
	typedef typename group<quaternion<U> >::logarithm impl_type;
	impl_type impl;
      public:
	logarithm(const group<rot>& = group<rot>() ) : impl() { }
	
	typedef rot domain;
	typedef algebra range;
      
	range operator()(const domain& g) const {
	  return 2 * impl( g.quaternion() );
	}
      
	struct push {
	  
	  typename func::traits<impl_type>::push dimpl;
	  push(const logarithm& log, const domain& at)
	    : dimpl( func::d(log.impl)(at.quaternion()) ) { }

	  algebra operator()(const algebra& dg) const {
	    return dimpl(dg);
	  }
	
	};

	struct pull {
	  typename func::traits<impl_type>::pull dTimpl;

	  pull(const logarithm& log, const domain& at)
	    : dTimpl( func::dT(log.impl)(at.quaternion()) ) { }

	  coalgebra operator()(const coalgebra& f) const {
	    return dTimpl(f);
	  }
	  
	};
      };

        
    };
  }
  
  
  // cross-product skew-symmetric matrix
  template<class U>
  matrix<U, 3, 3> hat(const vector<U, 3>& v) {
    matrix<U, 3, 3> res;
  
    res << 
         0,   -v.z(),   v.y(),
       v.z(),     0,   -v.x(),
      -v.y(),  v.x(),     0;
    
    return res;
  }
 
  
  template<class U>
  vector<U, 3> hat_inv(const matrix<U,3,3>& m) {
    return vector<U, 3>(m(2, 1), m(0, 2), m(0, 1) );
  }
  

  template<class U>
  bool nan( const rotation<U>& g) {
    return nan( g.quaternion() );
  }

  
}
  

#endif
