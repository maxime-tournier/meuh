#ifndef PHYS_SOLVER_QP_H
#define PHYS_SOLVER_QP_H

#include <phys/solver/numbering.h>
#include <math/matrix.h>
#include <plugin/mosek.h>

namespace phys {

 

  namespace solver {

    namespace qp {

        // TODO make these members ?
      template<class Key>
      void A(plugin::mosek& solver,
	     const typename Key::matrix& matrix, 
	     const math::mosek::constraint& rows,
	     const math::mosek::variable& cols,
	     const sparse::linear::numbering< phys::dof>& dofs ) {
	using namespace math;
      
	natural roff = rows.start;

	core::each(matrix, [&](const Key* key, const typename Key::row& row) {
	    core::each(row, [&](const phys::dof* dof, const mat& block) {
		natural cstart = cols.start + dofs.off(dof);

		mosek::constraint c(roff, roff + sparse::linear::dim(key));
		mosek::variable v(cstart, cstart + sparse::linear::dim(dof) );
		
		solver.A(c, v, block);
	      });
	    roff += sparse::linear::dim(key);
	  });

	assert( roff == rows.end );
      }
      

      template<class Key>
      void AT(plugin::mosek& solver, const typename Key::matrix& matrix, 
	      const math::mosek::constraint& rows,
	      const math::mosek::variable& cols,
	      const sparse::linear::numbering< phys::dof>& dofs, 
	      math::real lambda = 1.0) {
	using namespace math;
      
	natural coff = cols.start;

	core::each(matrix, [&](const Key* key, const typename Key::row& row) {
	    core::each(row, [&](const phys::dof* dof, const mat& block) {
	      
		natural rstart = rows.start + dofs.off(dof);

		mosek::constraint c(rstart, rstart + sparse::linear::dim(dof) );
		mosek::variable v(coff, coff + sparse::linear::dim(key) );
    
		solver.A(c, v, lambda * block.transpose());

	      });
	    coff += sparse::linear::dim(key);
	  });

	assert( coff == cols.end );
      }



      void A(plugin::mosek& solver, const phys::dof::mass& matrix,
	     const math::mosek::constraint& rows,
	     const math::mosek::variable& cols);

      void Q(plugin::mosek& solver, const phys::dof::matrix& matrix, 
	     const math::mosek::variable& vars,
	     const sparse::linear::numbering<phys::dof>& num);

      void c(plugin::mosek& solver, const phys::dof::vector& vector,
	     const math::mosek::variable& vars,
	     const sparse::linear::numbering<phys::dof>& num);
     
      void add_Q(plugin::mosek& solver, const phys::dof::matrix& matrix, 
		 const math::mosek::variable& vars,
		 const sparse::linear::numbering<phys::dof>& num);

      void add_Q(plugin::mosek& solver, const std::map<const phys::dof*, phys::dof::matrix>& matrix, 
		 const math::mosek::variable& vars,
		 const sparse::linear::numbering<phys::dof>& num,
		 math::real lambda = 1.0);
      
     
      constraint::friction::matrix cones(const phys::system& system );

      math::mosek::variable variable(phys::dof::key, math::mosek::variable dofs, 
					 const sparse::linear::numbering<phys::dof>& num);
      
    }
    
  }
}



#endif
