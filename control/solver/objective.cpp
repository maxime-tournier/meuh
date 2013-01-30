#include "objective.h"

#include <phys/solver/staggered.h>
#include <phys/solver/bilateral.h>
#include <phys/solver/unilateral.h>

#include <iostream>
#include <math/algo/cpg.h>
#include <math/algo/bokhoven.h>
#include <math/algo/lol.h>
#include <math/algo/modulus.h>
#include <math/mosek.h>

#include <core/macro/debug.h>




namespace control {
  namespace solver {

    static const math::real sigma = 1;

    objective::~objective() { }
    
    objective::objective( const phys::actuator::pack& actu,
			  const phys::constraint::bilateral::pack& obje,
			  const phys::solver::staggered& staggered,
			  math::natural iter,
			  math::real laziness)
     
      : idealized(staggered.friction, 0.01),
	
	act(actu.matrix, idealized),
	uni(staggered.unilateral.pack.matrix, idealized),
	
	obj(obje.matrix),
	values(obje.values),
	
	bounds(actu.bounds),
	
	staggered(staggered),
	iter(iter),
	laziness(laziness)
	
    { 
      if(actu.matrix.empty() || obj.matrix.empty() ) return;
      // if(idealized.matrix.empty()) return;
      using namespace math;

      const natural m = uni.keys.dim();
      const natural k = act.keys.dim();
      const natural o = obj.keys.dim();
      
      // OBJECTIVE
       // obj for normal
      mat U; U.resize(o, m);
      for(natural i = 0; i < m; ++i) {
	U.col(i) = obj.dense * 
	  obj.dofs.dense( uni.dofs.sparse( uni.linear.col(i)) );
      }
      
      // obj for actuator
      mat V; V.resize(o, k);
	       
      for(natural i = 0; i < k; ++i) {
	V.col(i) = obj.dense * 
	  obj.dofs.dense( act.dofs.sparse( act.linear.col(i)));
      }
      
      // order: normal, actuation
      Q.resize(o, k + m ); 
      Q << U, V
	;

      QtQ = (Q.transpose() * Q);
      // QtQ.setZero();

      // NORMAL CONSTRAINT
      const mat X = uni.compliance();
      mat Y; Y.resize(m, k);
 
      for(natural i = 0; i < k; ++i) {
      	Y.col(i) = uni.dense * uni.dofs.dense( act.dofs.sparse( act.linear.col(i) ) );
      }

      // mat Z; Z.resize(k, m);
      // for(natural i = 0; i < m; ++i) {
      // 	Z.col(i) = act.dense * act.dofs.dense( uni.dofs.sparse( uni.linear.col(i) ) );
      // }
      
      // QtQ.setZero();

      QtQ.topLeftCorner(m, m) += (laziness / 4 + sigma) * X;
      QtQ.topRightCorner(m, k) += laziness * Y / 2;
      QtQ.bottomLeftCorner(k, m) += laziness * Y.transpose() / 2;
      // QtQ.bottomRightCorner(k, k) +=  laziness * act.compliance();
      
      // QtQ.diagonal().head(m) += 1e-4 * sigma * vec::Ones(m);

      // QtQ.bottomRightCorner(k, k).diagonal() += laziness * act.keys.dense(bounds).cwiseInverse();
      QtQ.bottomRightCorner(k, k) +=  laziness * act.compliance();

      // QtQ.bottomRightCorner(k, k).diagonal() += 1e-4 * act.keys.dense(bounds).cwiseInverse();
      // QtQ.bottomRightCorner(k, k).diagonal() += 1e-5 * vec::Ones(k);

      // 
      // QtQ.bottomRightCorner(k, k).setZero();
      // qtq.bottomRightCorner(k, k).diagonal() +=  5 * act.keys.dense(bounds).cwiseInverse();
      
      // if( math::nan(QtQ) ) { std::cout << "ALAAARM " << std::endl; }

      mat A;
      A.resize(m, m + k );
      
      A << X, Y;			// N v > n
      
      // mat final = 0.5 * (QtQ + QtQ.transpose());

      // auto svd = math::svd(final);
      
      // std::cout << "eigen: " << svd.eigen().transpose() << std::endl;
      // const mat fix = svd.fix(1e-8);
      // std::cout << "fixed eigen: " << math::svd(fix).eigen().transpose() << std::endl; 
      
      
      qp.reset(new math::algo::mosek_qp(QtQ, A));
      qp->mosek.bounds(MSK_BK_LO, vec::Zero(m), vec::Constant(m, +MSK_INFINITY) ); // lambda > 0
    }


    
   

    math::vec compute_full(const objective& solver, const phys::dof::velocity& resp) {
      if(!solver.qp) return math::vec();
      using namespace math;
       
      const math::natural m = solver.uni.keys.dim();
      const math::natural o = solver.obj.keys.dim();
      const math::natural k = solver.act.keys.dim();

      // bounds
      const vec bounds = 10 * solver.act.keys.dense( solver.bounds ); // HARDCODE
	// 1e2 * vec::Ones(k);
      assert( bounds.rows() == int(k) );
      
      solver.qp->mosek.bounds(MSK_BK_RA, -bounds, bounds, m); // -bounds < mu < bounds
     
      const vec nf = solver.uni.dense * solver.uni.dofs.dense(resp);
      const vec n = solver.uni.keys.dense(solver.staggered.unilateral.pack.values) - nf;

      const vec af = solver.act.dense * solver.act.dofs.dense(resp);
      
      // QP c term
      vec rhs = solver.obj.keys.dense( solver.values ); // TODO precompute
      rhs.noalias() -= solver.obj.dense * solver.obj.dofs.dense(resp);
      
      assert(rhs.rows() == int(o));

      vec c = -solver.Q.transpose() * rhs;
      // vec c = vec::Zero(m + k);
          
      // c.head(m) -=  0.5 * sigma * n;

      // c.head(m) += // 0.5 * 
      // 	sigma * nf;

      c.tail(k) += 0.5 * solver.laziness * af;

      assert( c.rows() == int(m + k) );
      
      // QP solve
      // std::cout << "solving actuation" << std::endl;
      const vec mu = solver.qp->solve(c, n);

      assert(!nan(mu) );
      // std::cout << "actuator mu : " << mu.tail(k).transpose() << std::endl;
      // std::cout << "uni mu : " << mu.head(m).transpose() << std::endl;
      
      return mu;
    }

    

    math::vec compute_act(const objective& solver, const phys::dof::velocity& resp) {
      const math::vec full = compute_full(solver, resp);

      if ( full.empty() ) return math::vec();
      return full.tail(solver.act.keys.dim());
    }
    
    // phys::velocities objective::operator()(const phys::impulses& f) const {
   
    //   phys::velocities resp = staggered.bilateral(f);
    //   math::vec mu = solver::compute(*this, resp);
      
    //   if(mu.empty() ) return resp;
      
    //   using sparse::linear::operator+;
    //   return resp + actuator.dofs.sparse(actuator.linear * mu);
    // };
    
    
    phys::dof::momentum objective::planning(const phys::dof::momentum& f) const {
      const phys::dof::velocity resp = idealized(f);
      math::vec mu = solver::compute_full(*this, resp);
      
      if(mu.empty() ) return resp;
      
      const math::natural m = uni.keys.dim();
      const math::natural k = act.keys.dim();

      using sparse::linear::operator+;
      
      const phys::dof::momentum full = 
	act.dofs.sparse( act.dense.transpose() * mu.tail(k) ) + 
	uni.dofs.sparse( uni.dense.transpose() * mu.head(m) );
      
      return idealized( f + full );
    }
    
    
    phys::dof::momentum objective::force(const phys::dof::momentum& f) const {
      const phys::dof::velocity resp = idealized(f);

      math::vec mu = solver::compute_act(*this, resp);
      
      if(mu.empty() ) return phys::dof::momentum();
      const math::natural k = act.keys.dim();
	
      return act.dofs.sparse( act.dense.transpose() * mu.tail(k) );
    }


  }
}
