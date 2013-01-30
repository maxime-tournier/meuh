#ifndef PHYS_SOLVER_VSI_H
#define PHYS_SOLVER_VSI_H

#include <phys/system.h>
#include <phys/solver/compliance.h>
#include <math/algo/stop.h>

namespace phys {
  namespace solver {

    struct vsi {

      const solver::compliance& K;
      const math::algo::stop& stop;
      
      const math::vec mu;
      const math::vector<math::natural> normal_map;
      
      vsi(const solver::compliance& compliance, 
	 const math::algo::stop& stop);

      math::real project(const math::vec& x, math::natural i) const;

      dof::velocity operator()(const dof::momentum& f) const;
      
    };


  }
}


#endif
