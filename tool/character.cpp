#include "character.h"

#include <core/log.h>

#include <math/matrix.h>
#include <math/svd.h>
#include <math/func/vector.h>
#include <math/func/SE3.h>

// #include <phys/skel/momentum.h>
// #include <phys/skel/absolute.h>
// #include <phys/skel/articulated.h>

#include <phys/constant.h>
#include <core/unique.h>
#include <math/func/ref.h>

namespace tool { 
  namespace character {

    math::natural damping::kind = damping::joint;

    struct adaptor {
      typedef std::tuple<math::SO3, math::vec3, math::vec> domain;
      typedef std::tuple<math::SE3, math::vec> range;

      range operator()(const domain& g) const {
	return std::make_tuple( math::func::rigid<>()( std::make_tuple( std::get<0>(g), std::get<1>(g) ) ),
				std::get<2>(g) );
      }

      math::T<range> diff(const math::T<domain>& dg) const {
	auto at = std::make_tuple( std::get<0>(dg.at()), std::get<1>(dg.at()) );
	auto body = std::make_tuple( std::get<0>(dg.body()), std::get<1>(dg.body()) );

	auto push = math::func::rigid<>().diff( math::body(at, body));
	
	return math::body( std::make_tuple(push.at(), std::get<2>(dg.at())),
			   std::make_tuple(push.body(), std::get<2>(dg.body())));
      }
      
      math::coT<domain> diffT(const math::pullT<adaptor>& fg) const {

	auto at = std::make_tuple( std::get<0>(fg.at()), std::get<1>(fg.at()) );
	// auto body = std::make_tuple( std::get<0>(dg.body()), std::get<1>(dg.body()) );

	auto pull = math::pull( math::func::rigid<>(), at, std::get<0>(fg.body()));

	return math::cobody( fg.at(), 
			     std::make_tuple(std::get<0>(pull.body()), 
					     std::get<1>(pull.body()), 
					     std::get<1>(fg.body())));

      }
      

    };




    math::func::any<reduced::G, reduced::H> reduced::joint() const {
      using math::func::operator <<;
      return math::func::ref(pga) << adaptor();
    }


    // TODO redo
    void reduced::reset(const conf_type& at) {
      base::reset();
      
      conf = at;
      act[&actuator].setZero();
      
      AM.setZero();
    }

    void ragdoll::reset(const conf_type& at) {
      base::reset();
      
      conf = at;
    }


    
    reduced::reduced(const phys::rigid::skeleton& skel, 
		     const math::lie::pga<P>& pga,
		     math::natural n) 	
      : base( math::Lie<conf_type>(math::Lie<math::SO3>(),
				     math::Lie<math::vec3>(),
				     // math::Lie<math::SE3>(), 
				     math::Lie<math::vec>(n))),
	skeleton(skel),
	pga(pga, skel.root, n),
	actuator(n),
	damping(2), stiffness(10),
	rest( math::vec::Zero(n) ),
	init( lie.id() ),
	log("/tmp/reduced.dat")
    { 
      act[&actuator] = math::vec::Zero(n);
      assert(n);

    };

    static const int PGA = 2;

   
    
    void reduced::save_rest() {
      rest = std::get<PGA>( conf );
    }

  
    std::unique_ptr<reduced> reduced::copy(math::natural n) const {
      assert(n);
      
      reduced* obj = new reduced(skeleton, pga.data, n);
      math::natural old_n = pga.n;

       using std::get;

      get<0>(obj->conf) = get<0>(conf);
      get<0>(obj->velocity) = get<0>(velocity);
      get<0>(obj->momentum) = get<0>(momentum);
      
      get<0>(obj->init) = get<0>(init);

      get<1>(obj->conf) = get<1>(conf);
      get<1>(obj->velocity) = get<1>(velocity);
      get<1>(obj->momentum) = get<1>(momentum);
      
      get<1>(obj->init) = get<1>(init);

     

      if( n <= old_n ) {
      	std::get<PGA>(obj->conf) = std::get<PGA>(conf).head(n);
      	std::get<PGA>(obj->velocity) = std::get<PGA>(velocity).head(n);
      	std::get<PGA>(obj->momentum) = std::get<PGA>(momentum).head(n);
	get<PGA>(obj->init) = get<PGA>(init).head(n);
      } else {
	
      	std::get<PGA>(obj->conf) = math::vec::Zero(n);
      	std::get<PGA>(obj->velocity) = math::vec::Zero(n);
      	std::get<PGA>(obj->momentum) = math::form::Zero(n);

	get<PGA>(obj->init) = math::form::Zero(n);
	
      	std::get<PGA>(obj->conf).head(old_n) = std::get<PGA>(conf);
      	std::get<PGA>(obj->velocity).head(old_n) = std::get<PGA>(velocity);
      	std::get<PGA>(obj->momentum).head(old_n) = std::get<PGA>(momentum);

	std::get<PGA>(obj->init).head(old_n) = std::get<PGA>(init);
	
      }
      
      obj->damping = damping;
      obj->stiffness = stiffness;

      return core::unique( obj );
    }

    


    math::real reduced::potential(const G& at) const {
      return       
	// phys::constant::g * absolute(*this, phys::rigid::com(skeleton) )(at).y()
	+ 0.5 * stiffness * ( std::get<PGA>(at) - rest ).squaredNorm();
    }


    ragdoll::ragdoll(const phys::rigid::skeleton& skel) 
      : base( math::Lie<conf_type>( math::Lie<math::SE3>(), 
				      math::Lie<math::vector<math::SO3> >(skel.size() - 1)) ),
	skeleton(skel),
	damping(0), stiffness(0)
    {
      
    }

    
    // TODO FIXME !
    // math::func::any<ragdoll::conf_type, 
    // 		    phys::skel::pose> ragdoll::joint() const {
    //   return phys::skel::articulated(skeleton.topology);
    // }
    

  }
}
