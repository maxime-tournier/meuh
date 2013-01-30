#ifndef PHYS_SOLVER_GS_H
#define PHYS_SOLVER_GS_H

#include <phys/system.h>
#include <phys/solver/result.h>

namespace  phys {
  namespace solver {


    template<class Solver>
    struct gs { 
      
      const constraint::bilateral::pack pack;
      const Solver& solver;
      const math::real epsilon;
    

      mutable dof::velocity origin;

      gs(const constraint::bilateral::pack& pack,
	 const Solver& solver,
	 const math::real epsilon = 0) 
	: pack(pack),
	  solver(solver),
	  epsilon(epsilon) {

      }


      dof::momentum resp(const dof::momentum& f) const {
	using sparse::linear::operator-;
	return solver( f ) - origin;
      }
      
      dof::momentum ith(constraint::bilateral::key c, const constraint::bilateral::row& data, math::natural i) const {
	assert( i < sparse::linear::dim(c) );
	core::unused{c}; 
	dof::momentum res;

	sparse::map(res, data, [i](const math::mat& J) -> math::vec { return J.row(i).transpose(); } );
	
	return res;
      }

      math::real delta(const dof::momentum& Ji, 
		       const dof::momentum& JTlambda,
		       math::real qi) const {
	const dof::velocity JiMi = resp( Ji );
	
	const math::real d = sparse::linear::fast_dot(JiMi, Ji);
	
	return (qi - sparse::linear::dot( JiMi, JTlambda )) / (1e-5 + d);
      }
      
      static constraint::bilateral::vector zero(const constraint::bilateral::matrix& mat) {
	constraint::bilateral::vector res;

	auto it = res.begin();
	core::each(mat, [&](constraint::bilateral::key c, const constraint::bilateral::row& ) {
	    math::vec tmp = math::vec::Zero(sparse::linear::dim(c));
	    it = res.insert( it, std::make_pair(c, tmp) );
	  });

	return res;
      }
      

      math::real iter(const constraint::bilateral::vector& rhs,
		      dof::momentum& JTlambda,
		      constraint::bilateral::vector& lambda) const {
	
	// adjust lambda keys to matrix keys
	// constraint::bilateral::vector lambda = zero(pack.matrix);

	auto it_lambda = lambda.begin();
	auto it_rhs = rhs.begin();
	
	math::real eps2 = 0;

	math::natural n = 0;
	for(auto it_row = pack.matrix.begin(), end = pack.matrix.end(); it_row != end; 
	    ++it_row, ++it_lambda, ++it_rhs) {
	  
	  for(math::natural i = 0; i < sparse::linear::dim(it_row->first); ++i) {
		       
	     const dof::momentum Ji = ith(it_row->first, it_row->second, i);
	     
	     auto d = delta(Ji, JTlambda, it_rhs->second(i) );
	     it_lambda->second(i) += d;
	     
	     // TODO optimize sparse +=
	     using sparse::linear::operator*;
	     using sparse::linear::operator+=;

	     // JTlambda += d * Ji;
	     JTlambda = sparse::linear::transp(pack.matrix) * lambda;

	     eps2 += d * d;
	     ++n;
	   }
	}
	auto rms =  std::sqrt( eps2 / n );

	core::log("gs rms", rms);
	return rms;
      }

      // TODO warm start
      constraint::bilateral::vector solve(const constraint::bilateral::vector& rhs,
					  math::algo::stop s) const {
	core::log("gs solve");
	// TODO move to ctor ?
	auto bob = solver::chunk<constraint::bilateral>(pack.matrix);
	
	dof::momentum JTlambda = bob.dofs.zero();
	constraint::bilateral::vector lambda = bob.keys.zero();
	
	math::algo::iter_eps( s, [&](math::real& eps) {
	    eps = this->iter(rhs, JTlambda, lambda);
	  });
	
	return lambda;
      }
      


      dof::velocity operator()(const dof::momentum& f,
				  const result& res = result()) const {
	 
	 const dof::velocity free = solver(f);

	 if(pack.matrix.empty()) return free;

	 origin = solver( dof::momentum() );
	 
	 using sparse::linear::operator*;
	 using sparse::linear::operator-;
	
	const constraint::bilateral::vector rhs = pack.values - pack.matrix * free;
	

	math::algo::stop s(1e-7, 500);
	constraint::bilateral::vector lambda = solve(rhs, s);
	
	// if( res.lambda ) lambda = *res.lambda;	
	
	using sparse::linear::operator*;
	using sparse::linear::operator+;
	
	return solver( f + sparse::linear::transp(pack.matrix) * lambda, res);
       };

    };
  }
}

#endif
