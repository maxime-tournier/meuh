#ifndef PHYS_SOLVER_PGS_H
#define PHYS_SOLVER_PGS_H

#include <phys/solver/task.h>
#include <core/shuffle.h>
#include <math/iter.h>

#include <sparse/prod.h>
#include <sparse/diff.h>
#include <sparse/scalar.h>
#include <sparse/dot.h>
#include <sparse/dense.h>


namespace phys {

  namespace solver {
    
    
    template<class Solver>
    struct pgs {
      
      const Solver solver;

      typedef constraint::unilateral::matrix matrix_type;
      const matrix_type matrix;

      typedef typename constraint::unilateral::key key_type;
      const typename matrix_type::index_type index;
      
      mutable sparse::vector<key_type> d;
      
      mutable std::vector<key_type> ordering;
      math::iter iter;
      
      math::real damping;
      math::real omega;

      dof::momentum ith(key_type c, math::natural i) const {
	math::natural size = index.find(c)->second.size();
	dof::momentum res(size, 6*size ); // HARDCODE
	
	core::each(index.find(c)->second, [&](dof::key d) {
	    res(d) = matrix(c, d).row(i).transpose();
	  });

	return res;
      }
      
      pgs(const Solver& solver,
	  const matrix_type& matrix)
	: solver(solver),
	  matrix(matrix),
	  index(matrix.index()),
	  ordering(index.size()),
	  damping( 0 ),
	  omega( 1 )
	  
      {
	math::natural i = 0;
	matrix.rows().each([&](key_type k, core::unused ) {
	    ordering[i++] = k;
	  });

	// std::reverse(ordering.begin(), ordering.end());
	// core::shuffle( ordering );
	// script::require("log");
      }
      

      void solve(const solver::task& task) const {
	solver.solve(task);
	if(matrix.empty()) return;
	
	using namespace sparse;

	vector<key_type> lambda(index.size());

	if( task.mu ) lambda = *task.mu;

	vector<key_type> error = (task.unilateral - matrix * task.velocity);
	
	typename sparse::storage< key_type, std::vector< dof::velocity >  >::type responses;
	
	responses.rehash( matrix.rows().dim() );

	// constraint response TODO alloc
	dof::velocity resp, ith_resp;
	dof::momentum impulse;
	
	// constraint task
	solver::task tmp( ith_resp );	
	
	math::real eps = 0;
	math::natural k = 0;

	const bool jacobi = false;
	
	
	// core::timer timer; 
	iter.go([&] {
	    
	    math::real eps2 = 0;
	    dof::velocity storage;
	    
	    auto main = [&](key_type c) {
	    
		if(responses[c].empty()) responses[c].resize( dim(c) );
		
		// iterate on constraint lines
		for( math::natural i = 0; i < dim(c); ++i) {

		  // get ith constraint
		  auto Ji = this->ith(c, i);

		  if( responses[c][i].empty() ) {

		    tmp.momentum = Ji;
		    
		    // compute ith_resp
		    solver.solve( tmp );
		    
		    responses[c][i] = tmp.velocity;
		  }
		  
		  // compute diagonal term
		  const math::real d = dot(Ji, responses[c][i]) + damping;
		  
		  // if( d < math::epsilon ) break;

		  // form the rhs term
		  math::real delta = omega * ( error(c)(i) - dot(Ji, resp ) ) / d;

		  // update
		  lambda(c)(i) += delta;
		  
		  if(lambda(c)(i) < 0 ) {
		    delta -= lambda(c)(i);
		    lambda(c)(i) = 0;
		  }
		  
		  
		  dof::velocity& delta_resp = jacobi ? storage : resp;

		  // TODO optimize sum using dense ?
		  delta_resp += delta * responses[c][i];
		  
		  impulse += delta * Ji;
	      
		  eps2 += delta * delta;
		};
		
	      };
	    
	    // core::shuffle( ordering );
	    core::each( ordering, main );
	    
	    if( jacobi ) resp += storage;

	    // eps = sparse::norm();

	    eps = std::sqrt( eps2 );
	    ++k;
	    return eps;
	  });

		
	// script::exec("log.now().solve = { iter = %%, error = %%, target = %%, time = %%}", 
	// 	     k, eps, iter.epsilon, timer.start() );
	
	if(task.mu) task.update(*task.mu, lambda);

	solver::task final = task;
	final.momentum += impulse;

	solver.solve(final);

	const bool log = false;
	if( log ) {
	  auto w = matrix * task.velocity - task.unilateral;
	  math::vec taiste;
	  sparse::dense(taiste, matrix.rows(), w);
	
	  core::log("iter:", k, "error:", taiste.cwiseMin(math::vec::Zero(taiste.rows())).norm(), "compl:", sparse::dot(w, lambda) );
	}

	core::log("iterations:", k);
	
	// and that should do it
	// task.velocity += resp;
      };

    };


    template<class Solver>
    pgs<Solver> make_pgs(const Solver& solver,
		       const constraint::unilateral::matrix& mat) {
      return {solver, mat};
    }


  }

}



#endif
