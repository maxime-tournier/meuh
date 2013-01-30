#include "castor.h"

#include <math/mosek.h>


#include <core/log.h>

namespace control {
  namespace solver {

    castor::castor(const phys::system& phys,
		 const control::system& control,
		 math::real dt) 
      : phys(phys),
	control(control),
	cache(phys, dt),
	feature( control.feature.matrix )
    {


    }


    phys::dof::velocity castor::operator()(const phys::dof::momentum& f) const {
      
      using namespace math;

      math::mosek mosek;
      
      const math::mosek::variable v(0, cache.dofs.dim());

      const math::mosek::constraint bilateral(0, cache.p);
      const math::mosek::constraint unilateral = bilateral.after( cache.q );
      const math::mosek::constraint friction = unilateral.after( cache.r );
      
      mosek.init( cache.dofs.dim(), 
		  cache.dim() );
            
      if( bilateral.dim() ) { 
	mosek.A(bilateral, v, cache.J.dense);
	mosek.b(bilateral, cache.J.keys.dense(phys.constraint.bilateral.values), math::mosek::bound::fixed );
      }
      
      if( unilateral.dim() ) { 
	mosek.A(unilateral, v, cache.N.dense);
	mosek.b(unilateral, cache.N.keys.dense(phys.constraint.unilateral.values), math::mosek::bound::lower );
      }
    
      if( friction.dim() ) { 
	mosek.A(friction, v, cache.T.dense);
	mosek.b(friction, vec::Zero(friction.dim()), math::mosek::bound::fixed );
      }
      
      // TODO FIXME !
      math::mat Q = cache.mass.begin()->second;
      math::vec c = -cache.dofs.dense(f);
      
      if( feature.keys.dim() ) {
	Q += feature.dense.transpose() * feature.dense;
	c += -feature.dense.transpose() * feature.keys.dense( control.feature.values );
      }
           
      mosek.Q(v, Q);
      mosek.c(v, c);
      
      return cache.dofs.sparse( mosek.solve(v) );
      
    };


  }
}
