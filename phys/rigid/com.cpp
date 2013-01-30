#include "com.h"

#include <math/func/SE3.h>
#include <math/func.h>

namespace phys {
  namespace rigid {

    com::com(const phys::rigid::skeleton& skeleton)
      : skeleton(skeleton) { }
    
    using namespace math;
    
    com::range com::operator()(const domain& g) const {
      range res = range::Zero();
      
      skeleton.each( [&](math::natural i) {
	  res += skeleton.topology[i].mass * g(i).translation();
	});
      
      return res / skeleton.mass;
    }


    com::push::push(const com& of, const domain& at) 
      : skeleton(of.skeleton),
	dimpl( d( func::vec< func::get_translation<> >(of.skeleton.size()) )(at) )
    {
      
    }

    
    com::range com::push::operator()(const math::lie::group<domain>::algebra& v) const{
      assert(v.size() == skeleton.size());

      range res = range::Zero();
      
      skeleton.each( [&](math::natural i) {
	  
	  const real mi = skeleton.topology[i].mass;
	  res += mi * dimpl.data(i)( v(i) );
	});
      
      return res / skeleton.mass;
    }


    com::pull::pull(const com& of, const domain& at) 
      : skeleton(of.skeleton),
	dTimpl( dT( func::vec< func::get_translation<> >(of.skeleton.size()) )(at) )
    {
      
    }


    math::lie::group<com::domain>::coalgebra com::pull::operator()(const math::lie::group<range>::coalgebra& f) const {
      math::lie::group<domain>::coalgebra res;
      res.resize( skeleton.size() );

      res.each([&](math::natural i) {

	  const real mi = skeleton.topology[i].mass;

	  res(i) = (mi / skeleton.mass) * dTimpl.data(i)(f);

	});
      
      return res;
    }


    // coT<com::domain> com::diffT(const pullT<com>& fg) const {
    //   Lie<domain>::coalgebra body;
    //   body.resize( skeleton.size() );
      
    //   const real m = skeleton.mass;
      
    //   skeleton.each( [&](natural i ) {
    // 	  const real mi = skeleton.topology[i].mass;
	  
    // 	  body(i) = math::pull(func::translation<>(), 
    // 			       fg.at()(i), 
    // 			       (mi / m) * fg.body() ).body();
	  
    // 	});

    //   return math::cobody(fg.at(), body);
    // }

  }
}
