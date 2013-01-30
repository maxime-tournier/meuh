#include "reduced.h"

#include <math/func/euclid.h>
#include <math/func/SE3.h>

#include <phys/rigid/absolute.h>
#include <phys/rigid/relative.h>
#include <phys/rigid/com.h>

namespace tool {


  reduced::lagrangian_type reduced::potential() const {

    using namespace math::func;
    tuple_get<1, config_type> pose;
  
    auto diff = std::get<1>(lie.log().args) << lie::L( math::lie::of(rest).inv(rest) ) << pose << state_type::conf();
  
    math::natural pose_dim = skeleton.size() - 1;
  
    math::euclid::space< math::lie::group<pose_type>::algebra > euclid(pose_dim);

    return (0.5 * stiffness) * norm2( diff, euclid );
  }



  reduced::reduced(const phys::rigid::skeleton& skeleton,
		   const math::lie::group<pose_type>& lie)
    : character< config_type >(skeleton, 
				     std::make_tuple( math::lie::group<root_type>(),
						      lie))
  {
    using namespace math::func;
  
    tuple_get<0, config_type> root;
    tuple_get<1, config_type> pose;
  
    constant<config_type, root_type> const_root{ math::lie::group<root_type>().id() };
    constant<config_type, pose_type> const_pose{ lie.id() };
  
    const core::const_vector< pure_rotation<> > data(skeleton.size() - 1);
    auto rigid_pose = vec( data ) << pose;  
  
    auto canonical_pose = phys::rigid::absolute( this->skeleton ) 
      << adaptor<math::SE3>(this->skeleton.root)  
      << join(rigid<>() << root, rigid_pose);
  
    // auto canonical_com = phys::rigid::com(this->skeleton) << canonical_pose;
    // auto offset = pure_translation<>() << (- canonical_com);
  
    // auto rigid_root = lie::prod(rigid<>() << root, offset);

    auto rigid_root = rigid<>() << root;
    //  auto rigid_root = pure_translation<>() << tuple_get<1, root_type>() << root;
  
    kinematics = 
      phys::rigid::absolute( this->skeleton ) 
      << adaptor<math::SE3>(this->skeleton.root) 
      << join(rigid_root, rigid_pose);
  
    rest = lie.id();
    stiffness = 10.0;
  
    project = [&](const phys::rigid::config& g) {
      using namespace math::func;
      auto relative = inv_adaptor<math::SE3>(this->skeleton.root) << phys::rigid::relative( this->skeleton );
    
      auto r = relative(g);
    
      config_type res;
      std::get<1>(res) = vec<get_rotation<> >( +g.size() - 1) ( std::get<1>(r) );
      std::get<0>(res) = join(get_rotation<>(), get_translation<>())( std::get<0>(r) ); 

      return res;
    };

    
  }






}
