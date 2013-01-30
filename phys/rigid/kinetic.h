#ifndef PHYS_RIGID_KINETIC_H
#define PHYS_RIGID_KINETIC_H

#include <phys/rigid/topology.h>
#include <phys/rigid/config.h>

namespace phys {
  namespace rigid {


    struct kinetic {
      
      typedef math::lie::group<config>::algebra domain;
      typedef math::real range;

      typedef topology::directed topology_type;
      const topology_type& topology;
      
      kinetic(const topology_type& topology);
      
      range operator()(const domain& v) const;
      

      struct push {
	domain grad;

	static domain make_grad(const topology_type& topology, 
				const domain& at);
	
	push(const kinetic& of, const domain& at);

	range operator()(const domain& dv) const;

      };

      
      struct pull {
	typedef math::lie::group<domain>::coalgebra grad_type;
	
	grad_type grad;
	math::euclid::space<grad_type> coalg;
	
	static grad_type make_grad(const topology_type& topology, 
				   const domain& at);

	pull(const kinetic& of, const domain& at);

	grad_type operator()(const range& f) const;
	
      };


    };


    

  }
}


#endif
