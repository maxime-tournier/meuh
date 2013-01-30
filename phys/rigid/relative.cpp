#include "relative.h"

#include <math/func/lie.h>
#include <math/func/lie/translation.h>
#include <math/func/coord.h>


namespace phys {
  namespace rigid {

    relative::relative(const rigid::skeleton& skeleton) 
      : skeleton(skeleton) { }

    relative::range relative::operator()(const domain& g) const {
      assert(g.rows() == skeleton.size());

      range res; 
      res.resize( skeleton.size() );
      
      skeleton.prefix([&](math::natural i) {
	  
	  if(i == skeleton.root) {
	    res(i) = g(i);
	  } else {
	    
	    auto e = skeleton.in_edge(i);
	    auto p = skeleton.parent(i);
	    
	    // current joint
	    const joint& j = skeleton.topology[e];
	    
	    typedef math::func::coordinate< math::vector< math::SE3 > > coord;
	    
	    using namespace math::func;
	    
	    auto f = prod( lie::inverse<math::SE3>() << lie::R(j.frame(p)) << coord(p),
			   lie::R(j.frame(i)) << coord(i) );
	    
	    res(i) = f(g);
	  }
	});
      
      return res;
    }


    // TODO optimize
    relative::push::push( const relative& of,
			  const domain& at) 
      : at(at),
	skeleton(of.skeleton)
    {
      

    }


    math::lie::group< relative::range >::algebra relative::push::operator()(const math::lie::group<domain>::algebra& body) const {
      auto res = skeleton.lie().alg().zero();

      skeleton.prefix([&](math::natural i) {

	   if(i == skeleton.root) {
	    res(i) = body(i);
	  } else {
	    
	    auto e = skeleton.in_edge(i);
	    auto p = boost::source(e, skeleton.topology );
	    
	    // current joint
	    const joint& j = skeleton.topology[e];

	    typedef math::func::coordinate< math::vector< math::SE3 > > coord;
	    
	    using namespace math::func::lie;
	    using math::func::operator<<;
	    
	    auto f = prod( inverse<math::SE3>() << R(j.frame(p)) << coord(p),
			    R(j.frame(i))  << coord(i) );
	    
	    res(i) = d(f)(at)(body);
	  }
	});
      
      return res;
    }
    
   
     
    
  }
}
