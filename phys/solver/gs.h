#ifndef PHYS_SOLVER_GS_H
#define PHYS_SOLVER_GS_H

#include <phys/solver/task.h>
#include <core/shuffle.h>
#include <math/iter.h>

#include <sparse/prod.h>
#include <sparse/diff.h>
#include <sparse/scalar.h>
#include <sparse/dot.h>


namespace phys {

  namespace solver {
    
    
    template<class Solver>
    struct gs {
      
      const Solver solver;

      typedef constraint::bilateral::matrix matrix_type;
      const matrix_type matrix;

      typedef typename constraint::bilateral::key key_type;
      const typename matrix_type::index_type index;
      
      mutable sparse::vector<key_type> d;
      
      mutable std::vector<key_type> ordering;
      math::iter iter;
      
      math::real damping;
      math::real omega;

      dof::momentum ith(key_type c, math::natural i) const {
	dof::momentum res(index.find(c)->second.size());
	
	core::each(index.find(c)->second, [&](dof::key d) {
	    res(d) = matrix(c, d).row(i).transpose();
	  });

	return res;
      }
      
      gs(const Solver& solver,
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

	// script::require("log");
      }
      

      void solve(const solver::task& task) const {
	solver.solve(task);
	if(matrix.empty()) return;
	
	using namespace sparse;

	vector<key_type> lambda(index.size());

	if( task.lambda ) lambda = *task.lambda;

	vector<key_type> error = (task.bilateral - matrix * task.velocity);
	
	typename sparse::storage< key_type, std::vector< dof::velocity >  >::type responses;
	
	// constraint response TODO alloc
	dof::velocity resp, ith_resp;
	dof::momentum impulse;
	
	// constraint task
	solver::task tmp( ith_resp );	
	
	math::real eps = 0;
	math::natural k = 0;

	// core::timer timer; 
	iter.go([&] {
	    
	    math::real eps2 = 0;
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
		  
		  // TODO optimize sum using dense ?
		  resp += delta * responses[c][i];

		  impulse += delta * Ji;
	      
		  eps2 += delta * delta;
		};
		
	      };
	    
	    core::shuffle( this->ordering );
	    core::each( ordering, main );
	    
	    // eps = sparse::norm();

	    eps = std::sqrt( eps2 / lambda.keys().dim() );
	    ++k;
	    return eps;
	  });

		
	// script::exec("log.now().solve = { iter = %%, error = %%, target = %%, time = %%}", 
	// 	     k, eps, iter.epsilon, timer.start() );
	
	if(task.lambda) task.update(*task.lambda, lambda);

	solver::task final = task;
	final.momentum += impulse;

	solver.solve(final);
	
	// and that should do it
	// task.velocity += resp;
      };

    };


    template<class Solver>
    gs< typename std::decay<Solver>::type > make_gs(Solver&& solver,
						    const constraint::bilateral::matrix& mat) {
      return {std::forward<Solver>(solver), mat};
    }


  }

}



#endif
