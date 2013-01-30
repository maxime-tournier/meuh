#include "minres.h"

#include <phys/system.h>
#include <math/minres.h>

#include <phys/solver/task.h>

#include <sparse/dense.h>
#include <sparse/prod.h>
#include <sparse/sum.h>
#include <sparse/diff.h>
#include <sparse/scalar.h>
#include <sparse/hadamard.h>

#include <math/algo/cg.h>

namespace phys {
  namespace solver {


    minres::config_type::config_type() 
      :	iterations(1000),
	precision(0)
    {

    }

    minres::minres(const phys::system& system)
      : system(system),

	M( system.mass ),
	J( system.constraint.bilateral.matrix ),
	
	m( M.rows().dim() ),
	n( J.rows().dim() ),
	
	warm( 0 )
      
    {
	
    }
      

    void minres::solve(const task& t) const {
      
      math::vec rhs = math::vec::Zero(m + n);

      rhs.segment(0, m) = sparse::dense( M.rows(), t.momentum );
      rhs.segment(m, n) = sparse::dense( J.rows(), system.constraint.bilateral.values );
	
      const auto& d = system.constraint.bilateral.damping;

      math::vec work;
      work.resize( m + n );
      
      auto kkt = [&](const math::vec& x) -> const math::vec& {
	
	auto v = sparse::dense(M.rows(), x.segment(0, m));
	auto lambda = sparse::dense(J.rows(), x.segment(m, n) );
	
	sparse::dense(work.segment(0, m), M.rows(), system.mass * v + sparse::transp(J) * lambda );
	sparse::dense(work.segment(m, n), J.rows(), J * v + sparse::hadamard(lambda, d) );
	
	return work;
      };
      
    
      math::iter it(config.iterations, config.precision);

      math::vec init;
      if( warm ) init = *warm;
      rhs = math::minres( kkt ).solve( std::move(rhs), it, init );
      // rhs = math::algo::cg( kkt ).solve( std::move(rhs), it, init );
      if( warm ) *warm = rhs;

    
      t.velocity = sparse::dense(M.rows(), rhs.segment(0, m));
      
      if(n && t.lambda) (*t.lambda) = sparse::dense(J.rows(), -rhs.segment(m, n));
    }

    


  }
}
