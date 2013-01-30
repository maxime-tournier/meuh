#ifndef MATH_FUNC_SE3_H
#define MATH_FUNC_SE3_H

#include <math/rigid.h>
#include <math/kinematics.h>
#include <math/func/tuple.h>

namespace math {
  namespace func {

    template<class U = real>
    struct get_translation {
      
      typedef rigid<U> domain;
      typedef math::vector<U, 3> range;

      range operator()(const domain& g) const {
	return g.translation();
      }

      struct push {

	const rotation<U> q;
	
	push(const get_translation&, const domain& at) : q(at.rotation()) { }
	
	typename math::lie::group<range>::algebra operator()
	(const typename math::lie::group<domain>::algebra& v) const {
	  return q( linear(v) );
	}
	
      };

      struct pull {

	const rotation<U> qT;
	
	pull(const get_translation&, const domain& at) 
	  : qT(at.rotation().inverse()) { }
	
	typename math::lie::group<domain>::coalgebra operator()
	(const typename math::lie::group<range>::coalgebra& f) const {
	  typename math::lie::group<domain>::coalgebra res;

	  angular(res).setZero();
	  linear(res) = qT(f.transpose()).transpose();
	  
	  return res;
	}
	
      };
      
      

    };


    template<class U = real>
    struct get_rotation {

      typedef rigid<U> domain;
      typedef math::rotation<U> range;

      range operator()(const domain& g) const {
	return g.rotation();
      }
      
    };


    // TODO rename
    // SO3 -> SE3 
    template<class U = real>
    struct pure_rotation {

      typedef rigid<U> range;
      typedef math::rotation<U> domain;

      range operator()(const domain& g) const {
	return range::rotation(g);
      }


      struct push {
	
	push( const pure_rotation& , const domain& ) { }
	typename math::lie::group<range>::algebra operator()(const typename math::lie::group<domain>::algebra& v) const {
	  typename math::lie::group<range>::algebra res;
	  res.setZero();
	  angular(res) = v;
	  return res;
	}
	
      };

      struct pull {
	
	pull( const pure_rotation& , const domain& ) { }
	
	typename math::lie::group<domain>::coalgebra operator()(const typename math::lie::group<range>::coalgebra& f) const {
	  return angular(f);
	}
	
      };

     
    };



    template<class U = real>
    struct pure_translation {

      typedef rigid<U> range;
      typedef math::vector<U, 3> domain;
      
      range operator()(const domain& g) const {
	return range::translation(g);
      }


      struct push {
	
	push( const pure_translation& , const domain& ) { }
	typename math::lie::group<range>::algebra operator()(const typename math::lie::group<domain>::algebra& v) const {
	  typename math::lie::group<range>::algebra res;
	  res.setZero();
	  linear(res) = v;
	  return res;
	}
	
      };

      struct pull {
	
	pull( const pure_translation& , const domain& ) { }
	
	typename math::lie::group<domain>::coalgebra operator()(const typename math::lie::group<range>::coalgebra& f) const {
	  return linear(f);
	}
	
      };

     
    };



    

    template<class U = real>
    struct rigid {
      
      // in this order so that inertia are the same as with SE3
      typedef std::tuple< math::rotation<U >, math::vector<U, 3>  > domain;
      typedef math::rigid<U> range;
      
      range operator() (const domain& p ) const {
	return range(std::get<1>(p), std::get<0>(p));	
      }
      
      struct push {
	const rotation<U> qT;
	
	push( const rigid& , const domain& at)
	  : qT( std::get<0>(at).inverse() ) {  }
	
	typename math::lie::group<range>::algebra operator()
	(const typename math::lie::group<domain>::algebra& v) const {
	  
	  typename math::lie::group<range>::algebra res;
	  
	  angular(res) = std::get<0>(v);
	  linear(res) = qT( std::get<1>(v));
	  
	  return res;
	}
	
      };


      struct pull {
	const rotation<U> q;
	
	pull( const rigid& , const domain& at)
	  : q( std::get<0>(at) ) {  }
	
	typename math::lie::group<domain>::coalgebra operator()
	(const typename math::lie::group<range>::coalgebra& f) const {
	  
	  typename math::lie::group<domain>::coalgebra res;
	  
	  std::get<0>(res) = angular(f);
	  std::get<1>(res) = q( linear(f).transpose() ).transpose();

	  return res;
	}
	
      };



      
    };

    
    // absolute coordinates of a point given in relative coordinates
    template<class  U = real>
    struct rigid_map {
      
      EIGEN_MAKE_ALIGNED_OPERATOR_NEW;

      typedef math::rigid<U> domain;
      typedef math::vector<U, 3> range;
      
      range local;
      
      rigid_map(const range& local) : local( local ) { }
      rigid_map() : local( range::Zero() ) { }

      range operator()(const domain& g) const {
	return g( local );
      }

      
      struct push {
	const math::rotation<U> q;
	const range local;
	
	push(const rigid_map& of, const domain& at) 
	  : q(at.rotation()),
	    local(of.local) {

	}

	typename math::lie::group<range>::algebra operator()
	(const typename math::lie::group<domain>::algebra& v) const {
	  return q(  angular( v ).cross( local ) + linear(v) );
	}

      };

       struct pull {
	const math::rotation<U> qT;
	const range local;
	
	 pull(const rigid_map& of, const domain& at) 
	   : qT(at.rotation().inverse()),
	    local(of.local) {
	   
	 }
	 
	 typename math::lie::group<domain>::coalgebra operator()
	 (const typename math::lie::group<range>::coalgebra& f) const {
	   
	   typename math::lie::group<domain>::coalgebra res;
	   linear(res) = qT(f.transpose()).transpose();
	   angular(res) = local.cross( linear(res).transpose() ).transpose();

	   return res;
	}

      };


      // T<range> diff(const T<domain>& dg) const {
      // 	const range at = (*this)(dg.at());
	
      // 	const range body =
      // 	  dg.at().rotation() ( angular( dg.body()).cross( local ) + linear( dg.body()));
	
      // 	return math::body(at, body);
      // }


      // coT<domain> diffT(const pullT<absolute>& fg) const {
	
      // 	typename Lie<domain>::coalgebra body;
	
      // 	const range tmp = fg.at().rotation().inverse()( fg.body().transpose() );
	
      // 	linear(body) = tmp.transpose();
      // 	angular(body) = local.cross(tmp).transpose();
	
      // 	return math::cobody(fg.at(), body);
      // }




    };


    
    template<class U = real>
    struct apply_rotation {

      typedef std::tuple<math::rotation<U>, math::vector<U, 3> > domain;
      typedef math::vector<U, 3> range;

      range operator() ( const domain& x) const {
	return std::get<0>(x)( std::get<1>(x) );
      }

      // T<range> diff(const T<domain>& dx) const {
	
      // 	const math::rotation<U>& g = std::get<0>(dx.at());
      // 	const math::vector<U, 3>& w = std::get<0>(dx.body());

      // 	const math::vector<U, 3>& p = std::get<1>(dx.at());
      // 	const math::vector<U, 3>& v = std::get<1>(dx.body());
	
      // 	return math::body( g(p), g(v + w.cross(p)));
	
      // }
    };


    template<class U = real>
    struct apply_rigid {

      typedef std::tuple<math::rigid<U>, math::vector<U, 3> > domain;
      typedef math::vector<U, 3> range;

      range operator() ( const domain& x) const {
	return std::get<0>(x)( std::get<1>(x) );
      }

      // T<range> diff ( const T<domain>& dx) const {
	
      // 	const math::rigid<U>& g = std::get<0>(dx.at());
      // 	const math::vector<U, 6>& w = std::get<0>(dx.body());

      // 	const math::vector<U, 3>& p = std::get<1>(dx.at());
      // 	const math::vector<U, 3>& v = std::get<1>(dx.body());
	
      // 	return math::body( g(p), g.rotation()(v + angular(w).cross(p) + linear(w)) );
      // }
    };



    

  }
}


// namespace core {
//   template<class U>
//   struct type< math::func::rigid<U> > {
//     static std::string name() { return std::string("math::func::rigid") + core::args_name<U>(); }
//   };
// }


#endif
