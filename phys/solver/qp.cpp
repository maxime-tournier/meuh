#include "qp.h"

#include <phys/system.h>

#include <sparse/zero.h>
#include <sparse/zip.h>
#include <sparse/linear.h>

#include <phys/error.h>

#include <core/log.h>

namespace phys {
  namespace solver {
    

    namespace qp {

      struct cones_impl {
	const math::real mu;	// coulomb coeff
      
	math::mat operator()(const math::mat& normal, const math::mat& tangent) const {
	  assert(normal.rows() == 1);
	  math::mat res = tangent;
	
	  res.rowwise() += normal.row(0) / mu;

	  return res;
	}

	math::mat operator()(const math::mat& , sparse::zero ) const { 
	  throw phys::error("normal/tangent dof mismatch");
	}

	math::mat operator()(sparse::zero, const math::mat& ) const { 
	  throw phys::error("normal/tangent dof mismatch");
	}

      
      };



      constraint::friction::matrix cones(const phys::system& system ) {
	constraint::friction::matrix res;
      
	core::each(system.constraint.cones,
		   [&](const phys::constraint::cone* c) {
		   
		     sparse::zip( res[ c->tangent ],
				  system.constraint.unilateral.matrix.find(c->normal)->second,
				  system.constraint.friction.matrix.find(c->tangent)->second,
				  cones_impl{c->mu} );
		   }); 
      
	return res;
      }
    

    


      void A(plugin::mosek& solver, const phys::dof::mass& matrix,
	     const math::mosek::constraint& rows,
	     const math::mosek::variable& cols) {
	using namespace math;
   
	natural roff = rows.start;
	natural coff = cols.start;

	core::each(matrix, [&](const phys::dof* dof, const math::mat& block) {
	    using sparse::linear::dim;
	    
	    mosek::constraint c(roff, roff + dim(dof));
	    mosek::variable v(coff, coff + dim(dof));

	    solver.A(c, v, block);
	  
	    roff += dim(dof);
	    coff += dim(dof);
	  });
      
      }
    

    
    

      // this assumes a block diagonal Q !
      void Q(plugin::mosek& solver, const phys::dof::matrix& matrix, 
	     const math::mosek::variable& vars,
	     const sparse::linear::numbering<phys::dof>& num) {
	using namespace math;
	
	mosek::sparse_qobj qobj;
	
	core::each(matrix, [&](const phys::dof* dof, const mat& block) {

	    
	    mosek::variable v = variable(dof, vars, num);
	    
	    qobj[ v ] [ v ] = block;
	  });
	
	solver.Q( qobj );
	
      }

      // this assumes a block diagonal Q !
      void add_Q(plugin::mosek& solver, const phys::dof::matrix& matrix, 
		 const math::mosek::variable& vars,
		 const sparse::linear::numbering<phys::dof>& num) {
	using namespace math;
	core::each(matrix, [&](const phys::dof* dof, const mat& block) {

	    natural voff = vars.start + num.off(dof);
	    mosek::variable v(voff, voff + dof->dim());

	    solver.add_Q(v, block);
	  });
      
      }

      void add_Q(plugin::mosek& solver, const std::map<const phys::dof*, phys::dof::matrix>& matrix, 
		 const math::mosek::variable& vars,
		 const sparse::linear::numbering<phys::dof>& num, 
		 math::real lambda) {
	using namespace math;
	core::each(matrix, [&](const phys::dof* i, const phys::dof::matrix& row) {
	    natural ioff = vars.start + num.off(i);
	    mosek::variable vi(ioff, ioff + i->dim());

	    core::each(row, [&](const phys::dof* j, const mat& block) {
		
		natural joff = vars.start + num.off(j);
		
		if( joff <= ioff ) {
		  mosek::variable vj(joff, joff + j->dim());
		  solver.add_Q(vi, vj, lambda * block);
		}

	      });
	  });
	
      }




      void c(plugin::mosek& solver, const phys::dof::vector& vector,
	     const math::mosek::variable& vars,
	     const sparse::linear::numbering<phys::dof>& num) {
	using namespace math;
	core::each(vector, [&](const phys::dof* dof, const vec& block) {

	    natural voff = vars.start + num.off(dof);
	    mosek::variable v(voff, voff + dof->dim());
	  
	    solver.c(v, block);
	  
	  });
      
      }


      

   

 

      // void qp::add_c(math::mosek& solver, const phys::dof::vector& vector,
      // 		      const math::mosek::variable& vars,
      // 		      const sparse::linear::numbering<phys::dof>& num) {
      //   using namespace math;
      //   core::each(vector, [&](const phys::dof* dof, const vec& block) {

      // 	  natural voff = vars.start + num.off(dof);
      // 	  mosek::variable v(voff, voff + dof->dim);
	  
      // 	  solver.add_c(v, block);
	  
      // 	});
      
      // }
    

      
      math::mosek::variable variable(phys::dof::key d, math::mosek::variable dofs, 
				     const sparse::linear::numbering<phys::dof>& num) {
	math::natural start = dofs.start + num.off(d);

	return math::mosek::variable(start, start + d->dim() );
      }
     

    }




  }
}
