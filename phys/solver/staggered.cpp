#include "staggered.h"


#include <math/vec.h>
#include <math/matrix.h>

#include <phys/constraint/unilateral.h>

#include <phys/solver/friction.h>
#include <phys/solver/unilateral.h>

#include <core/stl.h>
#include <core/find.h>

#include <sparse/linear.h>


namespace phys {
  namespace solver {


    staggered::staggered(const solver::any& bilateral,
			 const solver::unilateral& unilateral ,
			 const solver::friction& friction,
			 const constraint::cone::set& cones,
			 math::natural iter)
      : bilateral(bilateral),
	unilateral(unilateral),
	friction(friction),
	cones(cones),
	iter(iter)
    {
      // assert(&unilateral.bilateral == &friction.bilateral );
    }


    constraint::friction::vector bounds(const constraint::cone::set& cones,
					const constraint::friction::bounds& mu,
					const constraint::unilateral::vector& lambda) {
      assert(!lambda.empty());
      constraint::friction::vector res;

      core::each(cones, [&](const constraint::cone* cone) {
	  const core::error argh("pas trouve");
	  
	  math::vec1 normal = core::find(lambda, cone->normal, argh)->second;
	  res[ cone->tangent ] = core::find(mu, cone->tangent, argh)->second * normal;

	});
      
      return res;


    }
    void compute(dof::momentum& normal, dof::momentum& tangent, const staggered& solver, const dof::impulses& f) {
      constraint::unilateral::vector lambda = 
	solver.unilateral.data.keys.sparse( math::vec::Zero( solver.unilateral.data.keys.dim() ) );
      
      using sparse::linear::operator+;
      for(math::natural i = 0; i < solver.iter; ++i) {

	
	tangent = solver.friction.force(f + normal, bounds(solver.cones, 
							   solver.friction.pack.bounds, lambda));


	// unilateral last, so that non-penetration is enforced
	lambda = solver.unilateral.lambda( f + tangent );
	normal = solver.unilateral.force(lambda);


      }
      
    }
    
    dof::velocity staggered::operator()(const dof::momentum& f) const {
      dof::momentum normal, tangent;
      
      compute(normal, tangent, *this, f);
      
      using sparse::linear::operator+;
      return bilateral( f + normal + tangent );
    };
     
    
    dof::momentum staggered::force(const dof::momentum& f) const {
      dof::momentum normal, tangent;
      
      compute(normal, tangent, *this, f);
      
      using sparse::linear::operator+;
      return normal + tangent;
    }
    
  }
}
