#include "friction.h"

#include <phys/solver/bilateral.h>
#include <sparse/linear.h>

#include <iostream>

#include <math/algo/cpg.h>
#include <math/algo/bokhoven.h>
#include <math/algo/mosek.h>

namespace phys {
  namespace solver {

    friction::~friction() { }

    friction::friction(const constraint::friction::pack& pack,
		       const solver::any& bilateral,
		       math::natural iter)
      : bilateral(bilateral),
	pack(pack),
	data(pack.matrix, bilateral),
	iter(iter),
	Q(data.compliance())
    {
      if( Q.empty() )  return;
      
      qp.reset( new math::algo::mosek_qp(Q, math::mat()) );
    }
    

    // main computation
    math::vec coy(const friction& solver, const dof::velocity& resp, 
		     const constraint::friction::bounds& bounds) {
      if(solver.Q.empty()) return math::vec();
      using namespace math;
      
      const natural n = solver.data.keys.dim();

      const vec low = vec::Zero(n);
      const vec up = solver.data.keys.dense(solver.pack.bounds).cwiseProduct(solver.data.keys.dense(bounds));
      
      solver.qp->mosek.bounds(MSK_BK_RA, low, up);
      
      // here is a puzzle: which one is correct ? the Staggered
      // Projections paper suggests the latter, but is this really
      // maximum dissipation then ?

      // this amounts to min 1/2 xQx + c.x, which minimizes the total
      // kinetic energy of the system over admissible friction forces
      // const vec c = solver.data.dense * solver.data.dofs.dense(resp);

      // this amounts to min xQx + c.x, which only minimizes the work
      // of friction forces over the set of admissible friction forces
      const vec c = 0.5 * solver.data.dense * solver.data.dofs.dense(resp);
      
      // std::cout << "solving friction" << std::endl;
      const vec lambda = solver.qp->solve(c, vec() );
      solver.last = lambda;

      assert(! math::nan(lambda ) );
      
      return lambda;
    }



    dof::velocity friction::operator()(const dof::momentum& f, const constraint::friction::bounds& bounds) const {
      const dof::velocity resp = bilateral(f);
      const math::vec lambda = solver::compute(*this, resp, bounds);

      if(lambda.empty() ) return resp;
      
      using sparse::linear::operator+;
      return bilateral(f + data.dofs.sparse(data.dense.transpose() * lambda) );
    };


    constraint::friction::vector friction::lambda(const dof::momentum& f, 
						  const constraint::friction::bounds& bounds) const {
      const dof::velocity resp = bilateral(f);
      math::vec lambda = solver::compute(*this, resp, bounds);
      
      if(lambda.empty() ) return constraint::friction::vector();

      return data.keys.sparse(lambda);
    }
    
    
    dof::momentum friction::force(const dof::momentum& f, 
			     const constraint::friction::bounds& bounds) const {
      const dof::velocity resp = bilateral(f);
      const math::vec lambda = solver::compute(*this, resp, bounds);

      if(lambda.empty() ) return dof::momentum();
      
      return data.dofs.sparse(data.dense.transpose() * lambda);
    }

  }
}
