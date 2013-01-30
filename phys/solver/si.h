#ifndef PHYS_SOLVER_SI_H
#define PHYS_SOLVER_SI_H

#include <phys/system.h>
#include <phys/solver/compliance.h>
#include <math/algo/stop.h>

// BROKEN

namespace phys {
  namespace solver {

    struct si {

      const solver::compliance& K;
      const math::algo::stop& stop;
      
      const math::vec mu;
      const math::vector<math::natural> normal_map;
      
      si(const solver::compliance& compliance, 
	 const math::algo::stop& stop);

      dof::velocity operator()(const dof::momentum& f, math::vec lambda = math::vec() ) const;
      
    };


  }
}


#endif
