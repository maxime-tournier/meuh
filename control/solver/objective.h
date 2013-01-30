#ifndef CONTROL_SOLVER_OBJECTIVE_H
#define CONTROL_SOLVER_OBJECTIVE_H


#include <phys/actuator.h>
#include <phys/constraint/bilateral.h>
#include <phys/constraint/unilateral.h>

#include <phys/solver/affine.h>
#include <phys/solver/idealized.h>

#include <math/svd.h>
#include <sparse/linear.h>
#include <sparse/linear/dense.h>
#include <sparse/linear/numbering.h>

#include <math/kkt.h>

namespace math {
  namespace algo {
    struct bokhoven_qp;
    struct lol;
    
    struct modulus_qp;
    struct mosek_qp;
    struct mosek_sep_qp;
  }
}

namespace phys {
  namespace solver {
    struct bilateral;
    struct staggered;
  }
}

namespace control {

  namespace solver {


    struct objective {
      const phys::solver::idealized idealized;

      const phys::solver::affine<phys::actuator> act;
      const phys::solver::affine<phys::constraint::unilateral> uni;
      const phys::solver::chunk<phys::constraint::bilateral> obj;
      const phys::constraint::bilateral::values& values;
      
      const phys::actuator::bounds& bounds;
      
      // const phys::solver::bilateral& bilateral;
      const phys::solver::staggered& staggered;
      const math::natural iter;
      

      math::mat Q;
      math::mat QtQ;
      
      math::real laziness;

      std::unique_ptr<math::algo::mosek_qp> qp;

      objective( const phys::actuator::pack& act,
		 const phys::constraint::bilateral::pack& obj,
		 const phys::solver::staggered& solver,
		 math::natural iter = 100,
		 math::real laziness = 1e-3);
      ~objective();
    
      phys::dof::velocity operator()(const phys::dof::momentum& f) const; 

      // actuation
      phys::dof::momentum force(const phys::dof::momentum& f) const; 

      // planning
      phys::dof::momentum planning(const phys::dof::momentum& f) const;
 

    };



  }
}


#endif
