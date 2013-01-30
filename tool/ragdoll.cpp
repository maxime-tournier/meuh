#include "ragdoll.h"

#include <math/func/euclid.h>
#include <math/func/SE3.h>
#include <math/func/coord.h>
#include <math/func/tuple.h>
#include <math/func/ref.h>

#include <phys/rigid/absolute.h>
#include <phys/rigid/relative.h>
#include <phys/rigid/com.h>

namespace tool {

  using namespace math;
  using namespace func;

  ragdoll::ragdoll(const math::natural root,
		   const math::natural size) 
    : root(root),
      size(size) {

    res.resize( size );

  }
     
  math::lie::group< ragdoll::domain > ragdoll::lie() const {
    return {math::lie::group<root_type>{},
	math::lie::group<pose_type>(size - 1) };
  }

  const ragdoll::range& ragdoll::operator()(const domain& x) const {
    auto get_root = tuple_get<0, domain>();
    auto get_pose = tuple_get<1, domain>();

    res(root) = (func::rigid<>() << join( tuple_get<0, root_type>(), 
					  tuple_get<1, root_type>()) << std::move(get_root))(x);
    auto ith = coord<pose_type>(0);
    auto fun = pure_rotation<>() << ref(ith) << std::move(get_pose);
    
    
    for(natural i = 0; i < size - 1; ++i ) {
      math::natural off = (i < root) ? 0 : 1;
      ith.i = i;
      res(i + off) = fun(x);
    }
     
    return res;
  };


  ragdoll::push::push(const ragdoll& of, const domain& at) 
    : root(of.root),
      size(of.size),
      at(at)
  {
    res.resize( size );
  }

      
  const math::lie::group<ragdoll::range>::algebra& ragdoll::push::operator()(const math::lie::group<domain>::algebra& v) const {
     
    auto get_root = tuple_get<0, domain>();
    auto get_pose = tuple_get<1, domain>();
    
    res(root) = d(func::rigid<>() << join( tuple_get<0, root_type>(), 
					   tuple_get<1, root_type>()) << std::move(get_root))(at)(v);
    
    auto ith = coord<pose_type>(0);
    auto fun = pure_rotation<>() << ref(ith) << std::move(get_pose);
    
    for(natural i = 0; i < size - 1; ++i ) {
      math::natural off = (i < root) ? 0 : 1;
      ith.i = i;
      res(i + off) = d( math::func::ref(fun))(at)(v);
    }
    
    return res;

  }


  ragdoll::ragdoll(const phys::rigid::skeleton& skeleton) 
    : root(skeleton.root),
      size(skeleton.size()) {

  }

  ragdoll::pull::pull(const ragdoll& of, const domain& at) 
    : root(of.root),
      size(of.size),
      at(at)
  {
     std::get<1>(res).resize( size - 1);
     
  }

  std::vector<math::natural> ragdoll::index() const {
    std::vector<math::natural> res; 
    res.resize( size - 1 );
    
    for(math::natural i = 0; i < (size - 1); ++i) {
      math::natural off = (i < root) ? 0 : 1;
      res[ i ] = i + off;
    }

    return res;
  }

      
  const math::lie::group<ragdoll::domain>::coalgebra& ragdoll::pull::operator()(const math::lie::group<range>::coalgebra& f) const {
        
    auto get_root = tuple_get<0, domain>();
    auto get_pose = tuple_get<1, domain>();
  

    std::get<0>(res) = dT(func::rigid<>())(get_root(at))(f(root));
    
    for(natural i = 0; i < size - 1; ++i ) {
      math::natural off = (i < root) ? 0 : 1;
      std::get<1>(res)(i) = dT(pure_rotation<>())( get_pose(at)(i))(f (i + off));
    }
    
    return res;

  }

  
  // template<class G>
  // struct adaptor {
  //   // typedef adaptor base;
  
  //   const math::natural root;
  //   adaptor(math::natural root) : root(root) { }
  
  //   typedef std::tuple< G, math::vector<G> > domain;
  //   typedef math::vector<G> range;
  
    // range operator()(const domain& x) const {
    //   range res;
    //   res.resize( std::get<1>(x).rows() + 1);
    
    //   res(root) = std::get<0>(x);
    
    //   std::get<1>(x).each([&](math::natural i) {
	
    // 	  math::natural off = (i < root) ? 0 : 1;

    // 	  res(i + off) = std::get<1>(x)(i);
    // 	});
    
    //   return res;
    // }
  
  //   struct push : math::func::base< adaptor< typename math::lie::group<G>::algebra > > {

  //     push(const adaptor& of, const domain& ) : push::base(of.root) {  }

  //   };

  //   struct pull  {
  //     const math::natural root;
    
  //     pull(const adaptor& of, const domain& ) : root( of.root ) {  }
    
  //     mutable typename math::lie::group<domain>::coalgebra res;
  //     const typename math::lie::group<domain>::coalgebra& operator()(const typename math::lie::group<range>::coalgebra& f) const {
      
  // 	std::get<1>(res).resize( f.cols() - 1 );
      
  // 	std::get<0>(res) = f(root);

  // 	std::get<1>(res).each([&](math::natural i) {
  // 	    math::natural off = (i < root) ? 0 : 1;
	  
  // 	    std::get<1>(res)(i) = f(i + off);
  // 	  });
      
  // 	return res;
  //     }


  //   };
  // };



  // template<class G>
  // struct inv_adaptor {
  
  //   const math::natural root;
  //   inv_adaptor(math::natural root) : root(root) { }
  
  //   typedef math::vector<G> domain;
  //   typedef std::tuple< G, math::vector<G> > range;
  
  //   range operator()(const domain& x) const {
  //     range res;

  //     std::get<1>(res).resize( x.size() - 1);
  //     std::get<0>(res) = x(root);
    
  //     std::get<1>(res).each([&](math::natural i) {
	
  // 	  math::natural off = (i < root) ? 0 : 1;

  // 	  std::get<1>(res)(i) = x(i + off);
  // 	});
    
  //     return res;
  //   }
  
  //   struct push : math::func::base< inv_adaptor< typename math::lie::group<G>::algebra > > {

  //     push(const inv_adaptor& of, const domain& ) : push::base(of.root) {  }

  //   };

  
  // };



  // ragdoll::lagrangian_type ragdoll::potential() const {

  //   using namespace math::func;
  //   tuple_get<1, config_type> pose;
  
  //   auto diff = std::get<1>(lie.log().args) << lie::L( math::lie::of(rest).inv(rest) ) << pose << state_type::conf();
  
  //   math::natural pose_dim = skeleton.size() - 1;
  
  //   math::euclid::space< math::lie::group<pose_type>::algebra > euclid(pose_dim);

  //   return (0.5 * stiffness) * norm2( diff, euclid );
  // }



  // ragdoll::ragdoll(const phys::rigid::skeleton& skeleton,
  // 		   const math::lie::group<pose_type>& lie)
  //   : character< config_type >(skeleton, 
  // 				     std::make_tuple( math::lie::group<root_type>(),
  // 						      lie))
  // {
  //   using namespace math::func;
  
  //   tuple_get<0, config_type> root;
  //   tuple_get<1, config_type> pose;
  
  //   constant<config_type, root_type> const_root{ math::lie::group<root_type>().id() };
  //   constant<config_type, pose_type> const_pose{ lie.id() };
  
  //   const core::const_vector< pure_rotation<> > data(skeleton.size() - 1);
  //   auto rigid_pose = vec( data ) << pose;  
  
  //   auto canonical_pose = phys::rigid::absolute( this->skeleton ) 
  //     << adaptor<math::SE3>(this->skeleton.root)  
  //     << join(rigid<>() << root, rigid_pose);
  
  //   // auto canonical_com = phys::rigid::com(this->skeleton) << canonical_pose;
  //   // auto offset = pure_translation<>() << (- canonical_com);
  
  //   // auto rigid_root = lie::prod(rigid<>() << root, offset);

  //   auto rigid_root = rigid<>() << root;
  //   //  auto rigid_root = pure_translation<>() << tuple_get<1, root_type>() << root;
  
  //   kinematics = 
  //     phys::rigid::absolute( this->skeleton ) 
  //     << adaptor<math::SE3>(this->skeleton.root) 
  //     << join(rigid_root, rigid_pose);
  
  //   rest = lie.id();
  //   stiffness = 10.0;
  
  //   project = [&](const phys::rigid::config& g) {
  //     using namespace math::func;
  //     auto relative = inv_adaptor<math::SE3>(this->skeleton.root) << phys::rigid::relative( this->skeleton );
    
  //     auto r = relative(g);
    
  //     config_type res;
  //     std::get<1>(res) = vec<get_rotation<> >( +g.size() - 1) ( std::get<1>(r) );
  //     std::get<0>(res) = join(get_rotation<>(), get_translation<>())( std::get<0>(r) ); 

  //     return res;
  //   };

    
  // }






}
