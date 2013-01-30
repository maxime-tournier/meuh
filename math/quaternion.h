#ifndef MATH_QUATERNION_H
#define MATH_QUATERNION_H

#include <math/lie.h>
#include <math/eigen.h>

#include <math/vector.h>
#include <math/covector.h>
#include <math/matrix.h>
#include <math/func/trigo.h>
#include <math/lie.h>
#include <math/euclid.h>
#include <math/nan.h>

#include <utility>

#include <Eigen/Geometry>

#include <math/error.h>
#include <math/pi.h>

namespace math {

  template<class U>
  struct quaternion : Eigen::Quaternion<U> { 
    typedef Eigen::Quaternion<U> base;

    quaternion( const quaternion& ) = default;
    quaternion( quaternion&& other) : base( other ) { }
    quaternion( const quaternion&& other) : base( other ) { }
    
    template<class ... Args>
    quaternion( Args&& ... args) 
      : base(std::forward<Args>(args)...) { 
    }
    
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW;

    static quaternion identity() { return base::Identity(); }
    
    static quaternion re_im(const U& w, const vector<U, 3>& v) { 
      return quaternion(w, v.x(), v.y(), v.z()); 
    }
    
    static quaternion im(const vector<U, 3>& v) { 
      return quaternion(0.0, v.x(), v.y(), v.z()); 
    }
    
    // between 0 and pi
    U angle() const { return 2.0 * half_angle(); }

    // between 0 and half_pi
    U half_angle() const { 
      U w = flip_needed() ? - base::w() : base::w();
      if (w > 1) w = 1;		// TODO warning ?
      const U res = std::acos( w );
      return res; 
    }
      
    // TODO make this robust
    vector<U, 3> axis() const { 
      const U n = base::vec().norm();
      if( n < precision<U>::epsilon ) { return vector<U, 3>::Zero(); }
      else { return base::vec() / n; }
    }
    
    // recover a positive angle
    quaternion& flip() { 
      if( flip_needed() ){ 
	*this = - *this;
      }
      return *this; 
    }

    quaternion flipped() const {
      quaternion q = *this;
      return q.flip();
    }
    
    bool flip_needed() const { return this->w() < 0; }

    quaternion operator-()const {
      quaternion res;
      res.coeffs() = - this->coeffs(); 
      return res;
    }

    quaternion& operator=(const quaternion& ) = default;
    
  };


  template<class U>
  bool nan(const quaternion<U>& q) { return nan(q.coeffs().eval()); } // TODO better

  
  // unit quaternion lie group
  namespace lie {
    
    template<class U>
    struct traits< quaternion<U> > {

      typedef vector<U, 3> algebra;
      typedef covector<U, 3> coalgebra;
    
      typedef quaternion<U> quat;

      // TODO overhead ?
      quat id() const { return quat::Identity(); }
      quat inv(const quat& q) const { return q.normalized().conjugate(); }
      quat prod(const quat& a, const quat& b) const { return (a * b).normalized(); }
      
      traits(const quat& ) { }
      traits() { }
      
      class adjoint {
	matrix<U, 3, 3> mat;
      public:
	EIGEN_MAKE_ALIGNED_OPERATOR_NEW;
	
	adjoint(const quat& q) : mat( q.matrix() ) { }  

	typedef algebra domain;
	typedef algebra range;

	range operator()(const domain& x) const {
	  return mat * x;
	}
  
      };


      class coadjoint {
	matrix<U, 3, 3> mat;
      public:
	EIGEN_MAKE_ALIGNED_OPERATOR_NEW;
	
	coadjoint(const quat& q) 
	: mat(q.matrix()) { 
	
	}  

	typedef coalgebra domain;
	typedef coalgebra range;

	range operator()(const domain& x) const {
	  return x * mat;
	}
	
      };

    
      // TODO duplicate with rotation.h
      static mat33 hat(const algebra& v) {
	mat33 res;
	res.diagonal().setZero();
	res(0, 1) = -v.z();
	res(1, 0) = v.z();

	res(0, 2) = v.y();
	res(2, 0) = -v.y();

	res(1, 2) = -v.x();
	res(2, 1) = v.x();

	return res;
      }

      static algebra hat_inv(const mat33& m) {
	return algebra(m(2, 1), m(0, 2), m(0, 1) );
      }
    
      struct exponential {
	exponential(const group<quat>& = group<quat>()) { }
	
	typedef algebra domain;
	typedef quat range;
	
	range operator()(const domain& v) const {
	  const U theta = v.norm();
      
	  if(theta < epsilon) return group<quat>().id();
	  
	  return quat::re_im( std::cos( theta ), func::sinc(theta) * v );
	}
      
	struct push {
	
	  const algebra at;
	  const U theta2;
	  const U theta;
	  const U c;
	  const U sinc;
	  const quat q;
	  
	  push(const exponential&, const algebra& at) 
	    : at(at),
	      theta2(at.squaredNorm()),
	      theta(std::sqrt(theta2) ),
	      c( std::cos(theta) ),
	      sinc( func::sinc(theta) ),
	      q( quat::re_im( c, sinc * at ) )
	  { }
	
	  algebra operator()(const algebra& h) const {
	  
	    if( theta < epsilon) return h;
      
	    const algebra proj = at * ( at.dot(h) / theta2 );
	    const algebra orth = h - proj;
	    
	    const algebra body = 
	      h + (sinc * c - 1) * orth  // symmetric part
	      - sinc * q.vec().cross(orth); // antisymmetric part
	    
	    return body;
	  }
	
	};
      

	struct pull {
	  
	  const algebra at;
	  
	  const U theta2;
	  const U theta;
	  const U c;
	  const U sinc;

	  pull(const exponential&, const algebra& at) 
	    : at(at),
	      theta2( at.squaredNorm() ),
	      theta( std::sqrt( theta2 ) ),
	      c( std::cos(theta) ),
	      sinc( func::sinc(theta) )
	  { }

	  coalgebra operator()(const coalgebra& f) const {
	    if( theta < epsilon) return f;
	    
	    const algebra proj = at * (at.dot(f.transpose()) / theta2);
	    const algebra orth = f.transpose() - proj;
	    
	    const coalgebra body = f + (sinc * c - 1) * orth.transpose()
	      + (sinc * sinc) * at.cross(orth).transpose();

	    return body;
	  }

	};

      };


    
      struct logarithm {
	logarithm(const group<quat>& = group<quat>()) { }
	
	typedef quat domain;
	typedef algebra range;

	range operator()(const domain& qq ) const {
	  // sanity flip
	  const quat q = qq.flipped().normalized();

	  // sanity clamp
	  const U w = std::min(1.0, q.w());
	  assert( w >= 0 );

	  const U theta = std::acos( w );
	
	  if( theta < epsilon ) return q.vec();
	
	  return theta * q.vec().normalized();
	}

	
	struct push {
	
	  const quat at;
	  const U w;
	  const U theta;
	  const algebra n;
	  const U sinc;
	  
	  push(const logarithm&, const quat& at) 
	    : at(at.flipped().normalized()),
	      w(std::min(1.0, this->at.w())),
	      theta( std::acos(w) ),
	      n( this->at.vec().normalized() ),
	      sinc( func::sinc(theta) )
	  { 
	    assert( w >= 0 );
	  }
	  
	  

	  // TODO optimize
	  algebra operator()(const algebra& h) const {
	  
	    if( theta < epsilon ) return h;

	    const algebra proj = n * n.dot(h);
	    const algebra orth = h - proj;
	
	    const range at = theta * n;
	    const algebra body = h  + (w - sinc  ) / sinc * orth // symmetric
	      + at.cross(h); // antisymmetric
	
	    return body;
	  }
	
	};

	
	struct pull {
	  
	  const quat at;
	  const U w;
	  const U theta;
	  const U sinc;
	  const algebra n;
	  
	  pull(const logarithm&, const quat& at) 
	    : at(at.flipped().normalized() ),
	      w( std::min(1.0, this->at.w()) ),
	      theta( std::acos( w ) ),
	      sinc(func::sinc( theta ) ),
	      n( this->at.vec().normalized() )
	  { 
	    assert( w >= 0 );

	  }
	  
	  coalgebra operator()(const coalgebra& f) const {
	    if( theta < epsilon ) return f;
	    
	    const algebra proj = n * n.dot( f.transpose() );
	    const algebra orth = f.transpose() - proj;

	    const coalgebra body = f + (( w - sinc ) / sinc) * orth.transpose() // symmetric
	      - theta * n.cross(f.transpose()).transpose(); // antisymmetric

	    return body;
	  }

	};

      };
      
      euclid::space<algebra> alg() const { return {}; }
      
    };

  }


  // template<class U>
  // matrix<U, 3, 3> hat(const vector<U, 3>& x) {
  //   return lie::group< quaternion<U> >::hat(x);

  // }

}

// Eigen fixes
namespace Eigen {

  template<typename _Real> struct NumTraits<Quaternion<_Real> >
  {
    typedef _Real Real;
    typedef Real FloatingPoint;
    enum {
      IsComplex = 0,
      HasFloatingPoint = NumTraits<Real>::HasFloatingPoint,
      ReadCost = 4,
      AddCost = 4 * NumTraits<Real>::AddCost,
      MulCost = 12 * NumTraits<Real>::MulCost + 4 * NumTraits<Real>::AddCost
    };
  };


  template<class Real>
  std::ostream& operator<<(std::ostream& o, const Quaternion<Real>& q) {
    o << q.coeffs().transpose();
    return o;
  }

  typedef Matrix<Quaternion<double>, Dynamic, Dynamic> MatrixXqd;
  typedef Matrix<Quaternion<double>, Dynamic, 1> VectorXqd;

  typedef Matrix<Quaternion<float>, Dynamic, Dynamic> MatrixXqf;
  typedef Matrix<Quaternion<float>, Dynamic, 1> VectorXqf;

}



  
#endif
