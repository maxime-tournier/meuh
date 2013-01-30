#ifndef PHYS_STEP_SYMPLECTIC_H
#define PHYS_STEP_SYMPLECTIC_H

#include <phys/state.h>
#include <math/iter.h>
#include <math/minres.h>

#include <math/func/vectorized.h>

#include <sparse/dense.h>
#include <sparse/prod.h>
#include <sparse/transp.h>

#include <phys/constraint/bilateral.h>
#include <phys/constraint/unilateral.h>


namespace phys {

  namespace step {

    template<class Key>
    struct pack {
      typename Key::matrix matrix;
      typename Key::vector vector;
    };


    struct unconstrained {
      
      const math::mat M;
      math::iter iter;
      
      void operator()(math::vec& x, 
		      const math::vec& f) const;
      
    };

    
    // constraints
    struct kinematic {
      
      const pack< constraint::bilateral > bilateral;
      const pack< constraint::unilateral > unilateral;
      
      const math::natural m;
      const math::natural n;
      
      typedef sparse::offset< dof::key > dofs_type;
      const dofs_type dofs;
      const math::natural k;
      
      mutable dof::velocity vel, JTlambda, NTmu;
      mutable constraint::bilateral::vector Jvel, lambda;
      mutable constraint::unilateral::vector Nvel, mu;
      
      mutable math::vec storage, storageT;

      kinematic(const pack<constraint::bilateral>& bilateral,
		const pack<constraint::unilateral>& unilateral) 
	: bilateral(bilateral),
	  unilateral(unilateral),
	  m( bilateral.matrix.rows().dim() ),
	  n( unilateral.matrix.rows().dim() ),
	  dofs( m ? bilateral.matrix.cols() :
		n ? unilateral.matrix.cols() :
		dofs_type() ),
	  k( dofs.dim() )
	  
      {
	if( k )  {
	  vel = sparse::dense(dofs, math::vec());
	  JTlambda = vel;
	  NTmu = vel;
	}

	if( m && n ) {
	  assert( bilateral.matrix.rows().dim() == bilateral.matrix.rows().dim() );
	}

	if( m || n ) {
	  storage = math::vec::Zero(m + n);
	  storageT = math::vec::Zero( k );
	}

	if( m ) { 
	  Jvel = sparse::dense(bilateral.matrix.rows(), math::vec() );
	  lambda = Jvel;
	}

	if( n ) { 
	  Nvel = sparse::dense(unilateral.matrix.rows(), math::vec() );
	  mu = Nvel;
	}


      };

      math::vec rhs() const {
	math::vec res;
	if( !k ) return res;

	res.resize( m + n );

	if( m ) res.head(m) = sparse::dense(bilateral.matrix.rows(), bilateral.vector );
	if( n ) res.tail(n) = sparse::dense(unilateral.matrix.rows(), unilateral.vector );

	return res;
      }

      const math::vec& apply( math::vec::rchunk x ) const {
	vel.dense() = x;
	
	sparse::prod(Jvel, bilateral.matrix, vel);
	sparse::prod(Nvel, unilateral.matrix, vel);
	
	if( m ) storage.head(m) = Jvel.dense();
	if( n ) storage.tail(n) = Nvel.dense();
	
	return storage;
      };


      const math::vec& applyT( math::vec::rchunk x ) const {
	
	if( m ) lambda.dense() = x.head(m);
	if( n ) mu.dense() = x.tail(n);
	
	sparse::prod(JTlambda, sparse::transp(bilateral.matrix), lambda);
	sparse::prod(NTmu, sparse::transp(unilateral.matrix), mu);
	
	storageT.setZero();
	
	if( m ) storageT += JTlambda.dense();
	if( n ) storageT += NTmu.dense();
	
	return storageT;
      };

      
      
    };
    

    struct constrained {

      const math::mat M;
      const kinematic constraints;
      
      math::iter iter;
      
      constraint::bilateral::vector& lambda;
      constraint::unilateral::vector& mu;
     

      void operator()(math::vec& x, 
		      const math::vec& f) const;


    };
    



    struct control {

      // kinematic objective 
      struct {
	math::mat matrix;
	math::vec vector;
      } obj;

      // dynamic objective
      struct {
	math::mat matrix;
	math::vec vector;
      } metric;

      // actuation basis/bounds
      struct {
	math::mat matrix;
	math::vec vector;
      } act;

    };

    struct fake_control {
      const math::mat M;
      
      const step::kinematic kinematic;
      const step::control control;

      math::iter iter;

      constraint::bilateral::vector& lambda;
      constraint::unilateral::vector& mu;
      
      void operator()(math::vec& x, 
		      const math::vec& f) const;
    };



    struct control_constrained {
      const math::mat M;
      const math::mat Minv;
      
      const step::kinematic kinematic;
      const step::control control;

      math::iter iter;
      math::vec& act;

      constraint::bilateral::vector& lambda;
      constraint::unilateral::vector& mu;
      
      mutable math::vec warm;
    
      
      void operator()(math::vec& x, 
		      const math::vec& f) const;

    };



    typedef std::function< void(math::vec& v, const math::vec& f) > solver_type;
    typedef std::function< solver_type (const math::mat& M ) > solver_maker_type;
    

    template<class G>
    typename state<G>::integrator_type symplectic(const typename state<G>::lagrangian_type& L,
						  solver_maker_type solver_maker,
						  math::natural steps = 1,
						  const math::lie::group<G>& lie = math::lie::group<G>(),
						  bool do_pull = true) {
      assert( solver_maker );

      typedef phys::state<G> state_type;
      typedef typename state_type::bundle_type bundle_type;
      
      typedef typename state_type::algebra algebra;
      typedef typename state_type::coalgebra coalgebra;

      const math::euclid::space< algebra > alg = lie.alg();
      const math::euclid::space< coalgebra > coalg = lie.coalg();
      
      const math::natural n = alg.dim();
      
      auto res = [L, n, lie, alg, solver_maker, coalg, steps, do_pull](const bundle_type& x_k, math::real dt) 
	-> bundle_type
	{
	using namespace math::func;

	tuple_get<0, bundle_type> g;
	tuple_get<1, bundle_type> v;
	tuple_get<2, bundle_type> p;

	const typename math::lie::group<G>::exponential exp = lie.exp();
	const typename math::lie::group<G>::logarithm log = lie.log();

	// current state
	const typename state_type::type state = join(g, v)(x_k);
	
	// mass tensor at g_k
	const typename state_type::mass_type mass = state_type::mass(L, state);
	
	using namespace math;
	math::mat M = state_type::matrix(mass, lie);
	
	const G d_k = exp( alg.scal(dt, v(x_k)) );
	const coalgebra pulled_pk = do_pull ? lie.adT(d_k) ( dT( ref(log) )( lie.inv(d_k) )( p(x_k) ) ) : p(x_k);
	
	// current velocity estimate
	algebra v_tmp = v(x_k);

	// dense vectors for solving
	math::vec rhs = math::vec::Zero(n);
	math::vec sol = math::vec::Zero(n);
	
	solver_type solver = solver_maker(M);
	
	for(natural i = 0; i < steps; ++i) {

	  // potential/inertial force at g_k with current velocity estimate
	  const coalgebra f_k = state_type::force(L, std::make_tuple(g(x_k), v_tmp))( g(x_k) );
	  
	  // rhs
	  coalgebra net = coalg.sum(pulled_pk, coalg.scal(dt, f_k) );
	   
	  // rhs pullback 
	  const algebra v_next = v_tmp;
	  const algebra minus_e_next = alg.scal(-dt, v_next);
	    
	  // pulled by current estimate
	  if( do_pull ) net = dT( ref(exp) )( minus_e_next ) ( net );
	  
	  // fill dense 
	  coalg.get(rhs, net);
	  alg.get(sol, v_tmp);
	  
	  // actual solve
	  solver(sol, rhs);
	  
	  alg.set(v_tmp, sol);
	}

	bundle_type x_next;
	  
	G& g_next = std::get<0>(x_next);
	algebra& v_next = std::get<1>(x_next);
	coalgebra& p_next = std::get<2>(x_next);
	
	v_next = v_tmp;
	g_next = lie.prod(g(x_k), exp(alg.scal(dt, v_next)) );
	p_next = mass( v_next );
	  
	return x_next;
      };
	
      return res;
    }
      
  }
}


#endif
