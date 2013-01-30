#ifndef PHYS_SOLVER_UNILATERAL_H
#define PHYS_SOLVER_UNILATERAL_H

#include <phys/constraint/unilateral.h>

#include <math/matrix.h>

#include <phys/solver/affine.h>
#include <phys/solver/any.h>

namespace math {
  namespace algo {
    struct pgs;
    struct mosek_qp;
  }
}

namespace phys {
  namespace solver {
    
    // TODO iterations ?
    struct unilateral {
            
      const constraint::unilateral::pack pack;
      const affine< constraint::unilateral > data;

      // const solver::bilateral& bilateral;
      const solver::any bilateral;
      
      const std::unique_ptr< math::algo::mosek_qp > pgs;
      // const std::unique_ptr< math::algo::pgs > pgs;
      const math::natural iter;

      mutable math::vec last;

      unilateral(const constraint::unilateral::pack& pack,
		 const solver::any& bilateral,
		 math::natural iter = 100); 
      ~unilateral();
      
      dof::velocity operator()(const dof::momentum& f) const;
      
      constraint::unilateral::vector lambda(const dof::momentum& f) const;
      
      dof::momentum force(const dof::momentum& f) const;
      dof::momentum force(const constraint::unilateral::vector& lambda) const;
            
    };


  }
}





#endif
