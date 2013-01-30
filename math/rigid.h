#ifndef MATH_RIGID_H
#define MATH_RIGID_H

#include <math/dual_quaternion.h>
#include <math/rotation.h>

#include <math/exception.h>

namespace math {

  template<class U >
  class rigid {
    typedef math::dual_quaternion<U> quat_type;

    quat_type quat_;

    typedef vector<U, 3> vec_type;
    typedef math::rotation<U> rot_type;
    typedef math::matrix<U, 4, 4> mat_type;
      
  public:
      
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW;

    rigid(const vec_type& t, const rot_type& q) 
      : quat_( t, q.quaternion() ) { }

    rigid(const quat_type& q) : quat_(q) { }
    rigid() { }

    typedef vec_type domain;
    typedef vec_type range;
      
    range operator()(const domain& x) const {
      return translation() + rotation()(x);
    }

    // TODO push/pull
    
    static rigid identity() { return quat_type::identity(); }
    rigid inverse() const { return quat_.conjugate().unit(); }
    rigid operator*(const rigid& other) const { return (quat_ * other.quat_).unit(); }
      
    rot_type rotation() const { return quat_.rotation(); }
    vec_type translation() const { return quat_.translation(); }
   
    static rigid rotation( const rot_type& g ) {
      return quat_type::rotation( g.quaternion() );
    }

    static rigid translation( const vec_type& v ) {
      return  quat_type::translation( v );
    }
       
    const quat_type& dual_quaternion() const { return quat_; }
    quat_type& dual_quaternion() { return quat_; }
        
    mat_type matrix() const {
      mat_type res;
      res << rotation().matrix(), translation(),
	vec_type::Zero().transpose(), 1.0;
      return res;
    }

  };

  
  template<class U>
  bool nan(const rigid<U>& g) {
    return nan(g.dual_quaternion() );
  }
  
  template<class U>
  std::ostream& operator<<(std::ostream& o, const rigid<U>& ) {
    // TODO 
    return o << "SE3";
  }
  

  namespace lie {

    template<class U>
    struct traits< rigid<U> > {

      typedef twist algebra;
      typedef wrench coalgebra;
    
      typedef rigid<U> G;
    
      const group<dual_quaternion<U> > lie;
      
      G id() const { return G::identity(); }
      G inv( const G& g ) const { return g.inverse(); }
      G prod(const G&a, const G& b) const { return a * b; }
    
      euclid::space<algebra> alg() const { return {}; }
      
      traits(const G& ) : lie() { }
      traits( ) : lie() { }
    
      struct  adjoint :  func::base< typename group< dual_quaternion<U> >::adjoint > {
	adjoint(const G& g) : adjoint::base( g.dual_quaternion()) { } 
      };
    
      struct coadjoint : func::base< typename group< dual_quaternion<U> >::coadjoint > {
	coadjoint(const G& g) : coadjoint::base( g.dual_quaternion()) { } 
      };
    
      class exponential {
	typedef typename group<dual_quaternion<U> >::exponential impl_type;
	const impl_type impl;
	
      public:
	exponential(const group<G>& = group<G >() ) : impl() { }
	
	typedef algebra domain;
	typedef G range;

	range operator()( const domain& v ) const { return impl( v / 2 ); }
      
	struct push {
	  const typename func::traits<impl_type>::push dimpl;
	  
	  push( const exponential& exp, const domain& at) 
	    : dimpl(d(exp.impl)(at / 2)) { }
	  
	  algebra operator()(const algebra& dv) const {
	    return dimpl(dv);
	  }
	  
	};

	// TODO pullback !
	
	// coT<domain> diffT( const pullT<exponential>& fv ) const { 
	//   auto fq = impl( math::body(impl, fv.at() / 2,  fv.body() ) );
	
	//   return math::cobody(fv.at(), fq.body() );
	// }

      };


      class logarithm {
	typedef typename group<dual_quaternion<U> >::logarithm impl_type;
	const impl_type impl;
      public:
	logarithm(const group<G>& = group<G>() ) : impl() { }
	
	typedef G domain;
	typedef algebra range;
	
	range operator()(const domain& g) const {
	  return 2 * impl( g.dual_quaternion() );
	}
      
	struct push {
	  const typename func::traits<impl_type>::push dimpl;
	  
	  push(const logarithm& log, const domain& at)
	    : dimpl(d(log.impl)(at)) { }
	  
	  algebra operator() (const algebra& dq) const {
	    return dimpl(dq);
	  }
	  
	};

	// TODO pullback
	// coT<domain> diffT(const pullT<exponential>& fv) const {
	//   auto fq = impl.diffT( math::body(impl, fv.at().dual_quaternion(), fv.body() ) );
	
	//   return math::cobody(fv.at(), fq.body() );
	// }
      
      };
    
    
    };

  }  
  

}
  


#endif
