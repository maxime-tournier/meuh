#ifndef MATH_KINEMATICS_H
#define MATH_KINEMATICS_H

#include <math/real.h>

#include <math/vec.h>
#include <math/form.h>

#include <math/tuple.h>

namespace math {

  template<class U = real>
  struct kinematics {

    typedef vector<U, 6> twist;
    typedef covector<U, 6> wrench;
    
    typedef std::tuple<U, U, vec3, vec3> screw;
    
    struct screw2twist {

      typedef screw domain;
      typedef twist range;
      
      range operator()(const domain& s) const {
	using namespace std;
	const real& t0 = get<0>(s);
	const real& tE = get<1>(s);
	const vec3& s0 = get<2>(s);
	const vec3& sE = get<3>(s);

	twist res;

	angular(res) = t0 * s0;
	linear(res) = tE * s0 + t0 * sE;
	return res;
      }
      
      // TODO pushforward
      // T<range> operator()(const T<domain>& ds) const {
	
      // 	using namespace std;  
      // 	const real& t0 = get<0>( ds.at() );
      // 	const real& tE = get<1>( ds.at() );
      // 	const vec3& s0 = get<2>( ds.at() );
      // 	const vec3& sE = get<3>( ds.at() );
      
      // 	const real& dt0 = get<0>( ds.body() );
      // 	const real& dtE = get<1>( ds.body() );
      // 	const vec3& ds0 = get<2>( ds.body() );
      // 	const vec3& dsE = get<3>( ds.body() );

      // 	typename Lie< twist >::algebra body;
      // 	angular(body) = dt0 * s0 + t0 * ds0;
      // 	linear(body) = (dtE * s0 + tE * ds0) + (dt0 * sE + t0 * dsE );
      
      // 	return math::body( (*this)(ds.at()), body);
      // }
    };




    struct twist2screw {
      
      typedef twist domain;
      typedef screw range;
      
      range operator()(const domain& t) const {
	const vec3 ang = angular(t), lin = linear(t);
      
	const real t0 = ang.norm();
      
	if( t0 > math::epsilon ) {
	  using namespace std;
	  const vec3 s0 = ang / t0;
	
	  const real tE = s0.dot(lin);
	  const vec3 sE = (lin - s0 * tE)/ t0;
	
	  return std::make_tuple(t0, tE, s0, sE);
	} else {
	  struct degenerated : std::exception { };
	  throw degenerated();
	
	}
      }
	
      // T<range> operator()(const T<domain>& dt) const {
      // 	// TODO !

      // 	struct not_implemented : std::exception { };
      // 	throw not_implemented();

      // }
      
    };

   };
  
  
  // accessor/mutator for angular/linear parts of the twist
  template<class U>
  typename vector<U, 6>::template ConstFixedSegmentReturnType< 3 >::Type
  angular(const vector<U, 6>& t) { return t.template head<3>(); }

  template<class U>
  typename vector<U, 6>::template FixedSegmentReturnType< 3 >::Type
  angular(vector<U, 6>& t)  { return t.template head<3>(); }

  template<class U>
  typename vector<U, 6>::template ConstFixedSegmentReturnType< 3 >::Type
  linear(const vector<U, 6>& t){ return t.template tail<3>(); }

  template<class U>
  typename vector<U, 6>::template FixedSegmentReturnType< 3 >::Type
  linear(vector<U, 6>& t) { return t.template tail<3>(); }
  
  
  template<class U>
  typename covector<U, 6>::template ConstFixedSegmentReturnType< 3 >::Type 
  angular(const covector<U, 6>& t) { return t.template head<3>(); }

  template<class U>
  typename covector<U, 6>::template FixedSegmentReturnType< 3 >::Type 
  angular(covector<U, 6>& t) { return t.template head<3>(); }
  
  
  template<class U>
  typename covector<U, 6>::template ConstFixedSegmentReturnType< 3 >::Type 
  linear(const covector<U, 6>& t){ return t.template tail<3>(); }

  template<class U>
  typename covector<U, 6>::template FixedSegmentReturnType< 3 >::Type 
  linear(covector<U, 6>& t) { return t.template tail<3>(); }
  
  typedef kinematics<>::screw screw;
  
}

#endif
