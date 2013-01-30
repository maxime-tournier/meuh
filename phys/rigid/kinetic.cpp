#include "kinetic.h"

#include <core/stl.h>

namespace phys {
  namespace rigid {

    kinetic::kinetic(const topology_type& topology) 
      : topology(topology) {

    }

    kinetic::range kinetic::operator()(const domain& v) const {

      range res = 0;

      core::each(boost::vertices(topology), [&](math::natural i) {
	  res += topology[i].kinetic( v(i) );
	});
	
      return res;
    }


    kinetic::domain kinetic::push::make_grad(const topology_type& topology, 
					     const domain& at) {
      domain res = at;

      res.each([&](math::natural i) {
	  res(i).noalias() = topology[i].tensor() * at(i);
	});

      return res;
    };

    kinetic::push::push(const kinetic& of, const domain& at)  
      : grad(make_grad(of.topology, at)) {

    }


    kinetic::range kinetic::push::operator()(const domain& dv) const {
      return math::euclid::dot(grad, dv);
    }

    kinetic::pull::grad_type kinetic::pull::make_grad(const topology_type& topology, 
						      const domain& at) {
      grad_type res;
      res.resize(at.size());

      res.each([&](math::natural i) {
	  res(i).noalias() = at(i).transpose() * topology[i].tensor();
	});

      return res;
    };

    kinetic::pull::pull(const kinetic& of, const domain& at) 
      : grad(make_grad(of.topology, at)),
	coalg(math::lie::of(at).coalg() )
    {				

    }

    kinetic::pull::grad_type kinetic::pull::operator()(const range& f) const {
      // TODO optimize f == 1, f == 0 ?
      return coalg.scal(f, grad);
    }
    

  }
}
