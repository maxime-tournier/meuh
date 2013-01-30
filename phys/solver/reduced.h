#ifndef PHYS_SOLVER_REDUCED_H
#define PHYS_SOLVER_REDUCED_H

#include <phys/constraint/bilateral.h>
#include <phys/dof.h>


#include <math/matrix.h>
#include <phys/solver/affine.h>

namespace phys {
  namespace solver {

    
    struct reduced {

      const dof::mass& masses;

      // row vectors describing allowed velocity subspace
      const constraint::bilateral::matrix& space;
      
      const chunk<constraint::bilateral> data;

      typedef Eigen::LDLT< Eigen::MatrixXd > inv_type;
      std::unique_ptr< inv_type > Mi;
      
      reduced(const dof::mass&, 
	      const constraint::bilateral::matrix& );

      dof::velocity operator()(const dof::momentum& f) const;

    };



  }
}



#endif
