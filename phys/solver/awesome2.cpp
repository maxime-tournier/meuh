#include "awesome2.h"


#include <sparse/linear/diagonal.h>
#include <sparse/linear/nan.h>
#include <sparse/linear.h>
#include <core/stl.h>

#include <core/macro/here.h>
#include <core/macro/debug.h>

#include <math/svd.h>

#include <core/log.h>

namespace phys {
  namespace solver {


   
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

    
    static awesome2::compliance_type make_compliance(awesome2& solver) {
      awesome2::compliance_type res;

      // actuators
      if( solver.a ) {
	std::cout << "actuators" << std::endl;
	
	res[ solver.act ] [ solver.act ] = solver.A.compliance();
      }
      
      // bilaterals

      if( solver.p ) {
	std::cout << "bilaterals" << std::endl;
	
	res[ solver.lambda ] [ solver.lambda ] = solver.J.compliance();

	if( solver.a ) res[ solver.lambda ] [ solver.act ]     = solver.J.dense * solver.A.linear;
	if( solver.q ) res[ solver.lambda ] [ solver.normal ]  = solver.J.dense * solver.N.linear;
	if( solver.r ) res[ solver.lambda ] [ solver.tangent ] = solver.J.dense * solver.T.linear;
      
      }
    
      // normal
      if( solver.q ) {
	std::cout << "normal" << std::endl;

	res[ solver.normal ] [ solver.normal ] = solver.N.compliance();

	if( solver.a ) res[ solver.normal ] [ solver.act ]     = solver.N.dense * solver.A.linear;
	if( solver.p ) res[ solver.normal ] [ solver.lambda ]  = solver.N.dense * solver.J.linear;
	if( solver.r ) res[ solver.normal ] [ solver.tangent ] = solver.N.dense * solver.T.linear;
      
      }

      // tangent
      if( solver.r ) {
	std::cout << "tangent" << std::endl;

	res[ solver.tangent ] [ solver.tangent ] = solver.T.compliance();

	if( solver.a ) res[ solver.tangent ] [ solver.act ]    = solver.T.dense * solver.A.linear;
	if( solver.p ) res[ solver.tangent ] [ solver.lambda ] = solver.T.dense * solver.J.linear;
	if( solver.q ) res[ solver.tangent ] [ solver.normal ] = solver.T.dense * solver.N.linear;
      }
      
      // TODO asserts !

      return res;
    }



    // matrice qui selectionne les normales des cones
    math::mat cone_normal(const awesome2& solver) {
      using namespace math;
      assert(solver.q && solver.r);

      

      assert( solver.N.keys.dim() >= solver.k );
      
      // la i-eme ligne prend la bonne normale dans N
      mat Res = mat::Zero(solver.k, solver.q);
      
      natural coff = 0;
      core::each(solver.system.constraint.cones, [&](const phys::constraint::cone* c) {
	  assert(c->normal->dim == 1);
	  
	  const natural noff = solver.N.keys.off(c->normal);
	  
	  Res(coff, noff) = c->mu;
	  
	  ++coff;
	});
      
      return Res;
    }


     // matrice qui selectionne les tangentes des cones
    math::mat cone_tangent(const awesome2& solver) {
      using namespace math;
      assert(solver.q && solver.r);
      
      assert( solver.T.keys.dim() % solver.k  == 0 );
      
      // la i-eme ligne somme les coefficients tangents du cone
      mat Res = mat::Zero(solver.k, solver.r);
      
      natural coff = 0;
      core::each(solver.system.constraint.cones, [&](const phys::constraint::cone* c) {
	  	  
	  const natural toff = solver.T.keys.off(c->tangent);
	  Res.row(coff).segment(toff, c->tangent->dim).setOnes();
	  
	  ++coff;
	});
      
      return Res;
    }
    



    static void constraint_row(math::mosek& qp, math::mosek::constraint c, 
			       const std::map<math::mosek::variable, math::mat >& data) {
      using namespace math;
      core::each(data, [&](const mosek::variable& v, const mat& m) {
	  qp.A(c, v, m);
	} );
      
    }




    using sparse::linear::operator+;
    using namespace math;
    awesome2::awesome2(const phys::system& system,
		     const dof::metric& obj,
		     math::real dt,
		     math::real epsilon) 
      : system(system),
	obj(obj),
	mass( make_integration(system, dt )  ),
	mass_inv( sparse::linear::diag(mass).inverse() ),
	response(mass, mass_inv),
	dofs( sparse::keys( mass ) ),
	
	// dense matrix + responses
	J( system.constraint.bilateral.matrix, response ),
	N( system.constraint.unilateral.matrix, response ),
	T( system.constraint.friction.matrix, response ),
	A( system.actuator.matrix, response ),
	
	// dimensions
	n( dofs.dim() ),
	a( A.keys.dim() ),
	p( J.keys.dim() ),
	q( N.keys.dim() ),
	r( T.keys.dim() ),
	k( system.constraint.cones.size() ),
	
	// variables
	v( 0, n ),
	act( mosek::variable::after(v, a) ),
	lambda( mosek::variable::after(act, p) ),
	normal( mosek::variable::after(lambda, q) ),
	tangent( mosek::variable::after(normal, r) ),

	x( mosek::variable::after(tangent, q) ),
	y( mosek::variable::after(x, r) ),
	
	// constraints
	dynamics( 0, n ),
	bilateral( mosek::constraint::after( dynamics, p ) ),
	unilateral( mosek::constraint::after( bilateral, q ) ),
	friction( mosek::constraint::after( unilateral, r ) ),

	cone( mosek::constraint::after(friction, k) ),

	// compliance 
	compliance( make_compliance(*this))
    { 
      
      const mat& Q = obj.quadratic.begin()->second;
      const vec& c = obj.linear.begin()->second;

      core::log log;

      log(" n: ", n, 
	  " p: ", p,
	  " q: ", q,
	  " r: ", r );
      
      // init
      qp.init(n + a + p + q + r +    q + r,
	      n + p + q + r + k);
      
      assert( cone.end == qp.constraints().end ); 
      assert( y.end == qp.variables().end ); 

      qp.debug( true );
      

      // dynamics FIXME
      qp.A(dynamics, v, mass.begin()->second);
      if( a ) qp.A(dynamics, act, -A.dense.transpose() );
      if( p ) qp.A(dynamics, lambda, -J.dense.transpose() );
      if( q ) qp.A(dynamics, normal, -N.dense.transpose() );
      if( r ) qp.A(dynamics, tangent, -T.dense.transpose() );

      log( "constraint rows" );
      if( p ) constraint_row(qp, bilateral, compliance[ lambda ] );
      if( q ) constraint_row(qp, unilateral, compliance[ normal ] );
      if( r ) constraint_row(qp, friction, compliance[ tangent ] );


      // ** bounds ** 
      // unilaterals bounds
      if( q ) {
	log( "bli unilaterals" );

	qp.bounds(normal, vec::Zero(q), mosek::lower);
	qp.bounds(x, vec::Zero(q), mosek::lower);

	qp.A(unilateral, x, -compliance[ normal ] [ normal ]);
	
	qp.Q(x, 1e14 * mat::Identity(q, q));
      }

      if( r ) {
	assert( k );
	
	log( "bli friction" );

	qp.bounds(tangent, vec::Zero(r), mosek::lower );
	
	qp.A(friction, y, compliance[ tangent ] [ tangent ]);
	qp.Q(y, 1e5 * compliance[ tangent ] [ tangent ] );

	const mat CN = cone_normal(*this); 
	const mat CT = cone_tangent(*this); 

	qp.A(cone, tangent, CT);
	qp.A(cone, normal, -CN); // TODO scale diag by coulomb coeff
	qp.b(cone, vec::Zero(k), mosek::upper);
	
      }

      // actuator bounds
      if( a ) {
	const vec act_bounds = A.rhs( system.actuator.bounds );
	qp.bounds(act, -act_bounds, act_bounds);
      }


      // qp.Q(vc, M);
      
      if( a ) {
	log( "bli actuation" );
	
	const vec act_bounds = A.rhs( system.actuator.bounds );
	const vec damp =  (10 * act_bounds.array() + 1).inverse();

	real lazy = epsilon;
	// qp.Q(act, (lazy * damp).asDiagonal() );
	qp.Q(act, epsilon * compliance[ act ] [ act ] );
      }

      // objective function

      // TODO fixme !
      const mat& M = mass.begin()->second;
      mat Qv = Q + M;
      vec cv = c;
      
      if( q ) {

	// // no normal resultant
	// piN = math::svd(N.dense.transpose(), 1e-1).inverse();

	// qp.A(complementarity, v, piN * Mint);
	// // qp.A(complementarity, act, piN * A.dense.transpose() );

	// qp.b(complementarity, vec::Zero(q), mosek::fixed);

      }


      // // do NOT want normal velocity

      // if( q ) {
      // 	const real w = 1e3;

      // 	Qv += w * N.dense.transpose() * N.dense;
      // 	cv -= w * N.dense.transpose() * N.rhs(system.constraint.unilateral.values);



      // }


      // qp.Q(v, Qv);
      // qp.c(v, cv);



    }
    
    
    dof::velocity awesome2::operator()(const dof::momentum& f) const {
      using namespace math;
      core::log log;
      
      log("response");
      const vec resp = dofs.dense( response(f) );
      
      // rhs 
      qp.b(dynamics, dofs.dense(f), mosek::fixed );
      
      if( p ) {
	qp.b(bilateral, J.rhs(system.constraint.bilateral.values) - J.dense * resp, mosek::fixed );
      }
      
      if( q ) { 
	qp.b(unilateral, N.rhs(system.constraint.unilateral.values) - N.dense * resp, mosek::fixed);
      }

      if( r ) { 
	qp.b(friction, - T.dense * resp, mosek::fixed);
      }
      
      // qp.write("/tmp/mosek.opf");

      // et boum
      try {
	return dofs.sparse( qp.solve( v ) );
      } 
      catch( const mosek::infeasible& e) {
	log("infeasible !");

	return dofs.sparse( qp.relax().solve(v) );
      }
      catch( const mosek::exception& ) {
	
	// std::cout << "*** removing bilaterals" << std::endl;
	// qp.b(bilaterals);
	
	// std::cout << "*** removing unilaterals" << std::endl;
	// qp.b(unilaterals);
	log("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");

	// return dofs.sparse( qp.solve(v) );
	return dofs.sparse( resp );
      }
      
    };
    
    
    
  }
}
