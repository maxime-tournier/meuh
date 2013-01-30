#include "awesome.h"


#include <sparse/linear/diagonal.h>
#include <sparse/linear/nan.h>
#include <sparse/linear.h>
#include <core/stl.h>

namespace phys {
  namespace solver {


    awesome::part::part(const dof::mass& mass, 
			const system::constraint_type& constraints)
      : matrix(mass),
	matrix_inv( sparse::linear::diag(matrix).inverse() ),
	solver(matrix, matrix_inv),
	bilateral(constraints.bilateral.matrix, solver),
	unilateral(constraints.unilateral.matrix, solver)
    {
      
    }


    struct diag_sum {

      const math::real lambda;

      void operator()(const phys::dof* , math::mat& m, const math::vec& diag) const {
	assert(m.rows() == m.cols());
	assert(m.rows() == diag.rows());
	
	m.diagonal() += lambda * diag;
      }

      void operator()(const phys::dof*, sparse::zero, const math::vec&) const {  }
      void operator()(const phys::dof*, math::mat&, sparse::zero) const {  }
      
    };

    
    static dof::mass make_integration(const phys::system& system, math::real dt ) {
      dof::mass res = system.masses;
      sparse::iter( core::range::all(res), core::range::all(system.damping), diag_sum{dt} );
      sparse::iter( core::range::all(res), core::range::all(system.stiffness), diag_sum{dt * dt} );
      return res;
    }


    using sparse::linear::operator+;
    

    math::mat make_big(const awesome& solver, math::real eps = 0) {
      using namespace math;

      const awesome::part& metric = solver.metric;

      const natural m = metric.bilateral.keys.dim();
      const natural n = metric.unilateral.keys.dim();

      const natural k = m + n;

      if( !k ) return mat();
      mat res; res.resize(k, k);
      
    
      if( m ) {
	res.block(0, 0, m, m) = 
	  metric.bilateral.compliance();

	if( n ) {
	  res.block(0, m, m, n) = metric.bilateral.dense * metric.unilateral.linear;
	}

      }

      if( n ) {
	res.block(m, m, n, n) = metric.unilateral.compliance();

	if( m ) {
	  res.block(m, 0, n, m) = metric.unilateral.dense * metric.bilateral.linear;
	}

      }
      // FIXME
      vec damp = (res.diagonal().array() + 1).cwiseInverse();

      // damp.head(m) = metric.bilateral.scaling;
      // damp.tail(n) = metric.unilateral.scaling;
      
      res.diagonal() += eps * damp;
      
      return res;
      // return 0.5 * (res + res.transpose());
    }


    awesome::awesome(const phys::system& system,
		     const dof::metric& obj,
		     math::real dt,
		     math::real epsilon) 
      : system(system),
	obj(obj),
	integration( make_integration(system, dt ), system.constraint ),
	metric( integration.matrix + obj.quadratic, system.constraint ),
	compliance( make_big(*this, epsilon) ),
	inv( system.constraint.bilateral.matrix.empty() && system.constraint.unilateral.matrix.empty() ? 
	     0 : new inv_type(compliance ) )
    { 
      
    }
    
    
    dof::velocity awesome::operator()(const dof::momentum& f) const {
      assert( ! sparse::linear::nan(f) );
      const dof::velocity resp = integration.solver( f );
      if( !inv ) return std::move(resp);

      using sparse::linear::operator+;

      const dof::vector c = obj.linear + sparse::linear::diag(obj.quadratic) * resp;
      const dof::vector obj_resp = metric.solver(c);
      
      using namespace math;
      
      const natural m = metric.bilateral.keys.dim();
      const natural n = metric.unilateral.keys.dim();
      const natural k = m + n;
      
      vec rhs; rhs.resize(k);

      using sparse::linear::operator-;

      if( m ) {
	rhs.head(m) = integration.bilateral.rhs(system.constraint.bilateral.values);
	rhs.head(m).noalias() += integration.bilateral.dense * sparse::linear::dense(integration.bilateral.dofs, obj_resp - resp);
      }

      if( n ) {
	rhs.tail(n) = integration.unilateral.rhs( system.constraint.unilateral.values );
	rhs.tail(n).noalias() += 
	  integration.unilateral.dense * sparse::linear::dense(integration.unilateral.dofs, obj_resp - resp);
      }
            
      assert( !nan(rhs) );
      const vec lambda = inv->solve(rhs, algo::stop().it( std::sqrt(k)), vec(), m);
      assert( !nan(lambda) );
      
      // constraint force
      vec force = vec::Zero( metric.bilateral.dofs.dim() ); //  TODO attation
      
      if(m) {
	force += metric.bilateral.linear * lambda.head(m);
      }
      
      if(n) { 
	force += metric.unilateral.linear * lambda.tail(n);
      }
      
      return resp + metric.bilateral.dofs.sparse( force ) - obj_resp;
    };
    
    
    
  }
}
