#include "pga.h"

#include <math/se3.h>
#include <math/lie/pga.h>
#include <math/tuple.h>

#include <math/vec.h>
#include <math/func/vector.h>
#include <math/func/SE3.h>
#include <math/func/lie.h>
#include <math/func/partial.h>
#include <math/func/tangent.h>

#include <tool/mocap.h>

#include <tool/ragdoll.h>
#include <math/func/select.h>

#include <phys/rigid/kinetic.h>
#include <phys/state.h>

#include <core/stl.h>

#include <script/meuh.h>

namespace tool {

  static pga::data_type make_data(const tool::mocap& mocap,
				  const pga::exclude_type& exclude) {
    
    auto ragdoll = tool::ragdoll( mocap.skeleton() );
    
    auto index = ragdoll.index();
    
    using namespace math::func;
    auto select = math::func::select< math::SE3 >(index, mocap.skeleton().lie() );
    
    auto fun = vec<get_rotation<> >(mocap.skeleton().size() - 1) << select << mocap.relative();
    
    auto samples = mocap.clip.sampling.sample(fun);
    
    core::each(exclude, [&](const std::string& name) {
	
	math::natural index = mocap.skeleton().find( name );
	if( index > mocap.skeleton().root ) --index;
	
	samples.each( [&](math::natural k) {
	    samples(k)(index) = samples(0)(index);
	  });

      });





    math::iter iter;
    iter.bound = 10;
    
    // mass-weighted pga lol
    auto kinematics = ragdoll << part<1>( ragdoll.lie().id() );
    auto kinetic = phys::rigid::kinetic( mocap.skeleton().topology ) << tangent( kinematics );
    
    typedef tool::ragdoll::pose_type pose_type;
    typedef phys::state< pose_type > state_type;

    math::lie::group<pose_type> lie_pose{ mocap.skeleton().size() - 1 };
    
    auto metric = [kinetic,lie_pose](const pose_type& pose ) -> math::mat {
      auto mass = state_type::mass(kinetic, std::make_tuple(pose, lie_pose.alg().zero()));
      return state_type::matrix(mass, lie_pose);
    };
    
    return {samples, iter, math::lie::of(samples(0)), 0.95, metric};
  }

  

  pga::pga(const tool::mocap& mocap,
	   const pga::exclude_type& exclude) 
    : data( make_data(mocap, exclude) )
  {
    script::require("plot");
    
    std::string eigen = "eigen";
    std::string cumulative = "cumulative";
    
    script::exec("%% = plot.new(); %%.title = {'eigen values'} ", eigen, eigen);
    script::exec("%% = plot.new(); %%.title = {'cumulative variance'} ", cumulative, cumulative);
    
    math::real sum = 0;
    
    data.dev.each([&](math::natural i) {
	script::exec("plot.push(%%, %%, %%)", eigen, i, data.dev(i));
	sum += data.dev(i);
      });
    
    math::real total = sum;

    sum = 0;
    data.dev.each([&](math::natural i) {
	sum += data.dev(i);
	script::exec("plot.push(%%, %%, %%)", cumulative, i, sum / total );
      });


  };

    
  // pga::pga(const math::lie::pga<inner>& data, math::natural root, math::natural nn) 
  //   : data(data),
  //     n(nn ? nn : data.geodesics().rows()),
  //     root(root),
  //     kind( first  )
  // { 
  //   assert( n && int(n) <= data.geodesics().rows());
  // }
    
  // using namespace math;

  // const SE3& pga::origin(const domain& g) {
  //   return std::get<0>(g);
  // }

         
  // const vec& pga::coords(const domain& g) {
  //   return std::get<1>(g);
  // }
    

  // SE3& pga::origin(domain& g) {
  //   return std::get<0>(g);
  // }

         
  // vec& pga::coords(domain& g) {
  //   return std::get<1>(g);
  // }
    


  // const T<SE3> pga::origin(const T<domain>& dg) {
  //   return math::body( std::get<0>( dg.at()),
  // 		       std::get<0>( dg.body()) );
  // }


  // const T<vec> pga::coords(const T<domain>& dg) {
  //   return math::body( std::get<1>( dg.at()),
  // 		       std::get<1>( dg.body()) );
  // }


   
  // Lie<pga::domain> pga::dom() const {
  //   return Lie<domain>(Lie<SE3>(), Lie<vec>(n));
  // }
    
  // Lie<pga::range> pga::rng() const {
  //   return Lie<pga::range>(data.geodesics()(0).rows());
  // }
    

  // struct scale {
  //   typedef vec domain;
  //   typedef vec range;

  //   const vec& scaling;
  //   scale(const vec& scaling) : scaling(scaling) { }
      
  //   range operator()(const domain& x) const {
  //     return scaling.cwiseProduct(x);
  //   }


  //   T<range> operator()(const T<domain>& dx) const {
  //     return math::body((*this)(dx.at()),
  // 			(*this)(dx.body()));
  //   }

  // };




  // pga::range pga::operator()(const domain& g) const {
    

  //   using func::operator<<;
      
  //   func::vector< func::pure_rotation<> > rot;
    
  //   // relative conf from pga
  //   pga::range res = kind == first ? 
  //     (rot << data.ccfk(n)) ( coords(g) ) :
  //     (rot << data.ccsk(n)) ( coords(g) ) ;
      
  //   // set root conf
  //   res( root ) = origin(g);

  //   return res;
  // }


  // T<pga::range> pga::diff(const T<domain>& dg) const {
  //   using func::operator<<;
      
  //   func::vector< func::pure_rotation<> > rot;

  //   // relative velocities from pga
  //   const T<pga::range> res = kind == first ? 
  //     d(rot << data.ccfk(n))( coords(dg) ) :
  //     d(rot << data.ccsk(n))( coords(dg) ) ;
      
  //   range at = std::move(res.at());
  //   Lie<range>::algebra body = std::move(res.body());
      
  //   // fill result
  //   const T<SE3> dorg = origin(dg);
      
  //   at( root ) = dorg.at();
  //   body( root ) = dorg.body();
            
  //   return math::body(std::move(at), std::move(body));
  // }

    
  // coT<pga::domain> pga::diffT(const pullT<pga>& fg) const {
      
  //   func::vector< func::pure_rotation<> > rot;
    
  //   // pull local forces on pga coords
  //   const coT<vec> fpga =  kind == first ? 
  //     math::pull(rot << data.ccfk(n), coords(fg.at()), fg.body()) : 
  //     math::pull(rot << data.ccsk(n), coords(fg.at()), fg.body()) ;
      
  //   // total forces
  //   const Lie<domain>::coalgebra body( fg.body()( root), fpga.body());
      
  //   return math::cobody(fg.at(), body); 
  // }



}


namespace math {
  namespace func {

    // tangent<tool::pga>::tangent( const of_type& of, const at_type& at) 
    //   : of(of), at(at),
    // 	impl( make_impl() )
    // {
      
    // }


    // tangent<tool::pga>::impl_type tangent<tool::pga>::make_impl() const {
    //   switch( of.kind ) {
    //   case tool::pga::first: 
    // 	return d( func::vector< pure_rotation<> >() << of.data.ccfk(of.n) )( std::get<1>(at)  );
    //   case tool::pga::second:
    // 	return d( func::vector< pure_rotation<> >() << of.data.ccsk(of.n) )( std::get<1>(at)  );
    //   default:
    // 	throw math::error("derp");
    //   };
    // }

    // tangent<tool::pga>::range tangent<tool::pga>::operator()(const domain& body) const {

    //   range res = impl( std::get<1>(body) );
    //   res(of.root) = std::get<0>(body);
      
    //   return res;
    // }

  }
}
