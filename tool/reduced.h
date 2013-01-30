#ifndef TOOL_REDUCED_H
#define TOOL_REDUCED_H

#include <tuple>

#include <tool/character.h>
#include <tool/ragdoll.h>

#include <math/func/any.h>
#include <math/func/euclid.h>
#include <core/shared.h>

namespace tool
 {

  template<class DOFS>
  struct reduced : tool::character< std::tuple< ragdoll::root_type, 
						DOFS > > 
  {
    typedef DOFS dofs_type;

    void pose( const math::func::any<dofs_type, ragdoll::pose_type>& p) {
      using namespace math::func;

      auto root = tuple_get<0, typename reduced::config_type>();
      auto dofs = tuple_get<1, typename reduced::config_type>();
      
      this->relative = ragdoll(this->skeleton.root,
    			       this->skeleton.size()) << join( root, p << dofs);

      *this->proxy.lock() = this->absolute()( this->g() );
    }

    core::shared< typename reduced::absolute_type > proxy;
    core::shared< typename math::lie::group< typename reduced::absolute_type >::algebra > dproxy;
    
    math::real stiffness;
    dofs_type rest;
    
    math::vec act;

    typename reduced::lagrangian_type potential() const {
      
      using namespace math::func;
      tuple_get<1, typename reduced::config_type> pose;
  
      auto lie = math::lie::of(rest);
      
      auto diff = lie.log() << math::func::lie::L( math::lie::of(rest).inv(rest) ) 
			    << pose 
			    << reduced::state_type::conf();
      return math::func::euclid::scalar_ref<math::real>(stiffness) << (0.5 * norm2( diff, lie.alg() ) );
    }
    
    reduced(const phys::rigid::skeleton& skeleton,
	    const math::lie::group< dofs_type >& lie ) 
      : reduced::character(skeleton, std::make_tuple( math::lie::group< ragdoll::root_type >(),
						      lie)),
	stiffness(1.0),
	rest(lie.id())
    {
      act = math::vec::Zero(lie.alg().dim());
    }

    math::natural inner_dofs() const {
      return math::lie::of( rest ).alg().dim();
    }
    

  };

}


#endif
  
