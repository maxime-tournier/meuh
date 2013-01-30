#include "woot.h"

#include <math/algo/iter.h>
#include <sparse/linear.h>

#include <core/log.h>
#include <phys/solver/si.h>
#include <phys/solver/vsi.h>

#include <math/mosek.h>

namespace control {
  namespace solver {

    
    woot::woot(const phys::system& system, math::real dt, math::real epsilon)
      : cache(system, dt),
	epsilon(epsilon)
    {
      
    }
    


    

    phys::dof::velocity woot::operator()(const phys::dof::momentum& f,
					   control::actuator::vector& act,
					   const objective& obj) const {
      using namespace math;
      using namespace phys;
      core::log log;
      
      algo::stop stop;
      stop.it(1000).eps(1e-14);
      const phys::solver::si si{cache, stop};
      
      if(!obj.dim() ) return si(f);

      using sparse::linear::operator+;
      
      // 1 preparer les n actuations
      const phys::solver::affine<control::actuator> A(cache.system.actuator.matrix,
						   cache.response);
      const natural a = A.keys.dim();
      
      vec ai = A.keys.dense( act );
      const vec ra = A.linear * ai;
      
      log("AI LOL", ai.transpose());

      // sample actuation
      std::vector< dof::momentum > D;
      
      // sample count
      const natural d = 2 * a;

      // finite difference step
      const real h = 40; 
      
      // enforce actuator bounds
      const vec bounds = A.keys.dense( cache.system.actuator.bounds );

      const vec fai = A.dense.transpose() * ai;
      mat Act = mat::Zero(ai.rows(), d+1);

      for(natural i = 0; i < d; ++i) {
		
	vec tmp;
	Act.col(i) = ai;
	
	// offset ai
	if( i < A.keys.dim() ) {
	  Act.col(i)(i) += h;
	} else {
	  Act.col(i)(i - a) -= h;
	}

	tmp = A.dense.transpose() * Act.col(i);
	// tmp = tmp.cwiseMin( bounds ).cwiseMax(-bounds);r
	D.push_back(A.dofs.sparse( tmp ) );
      }

      Act.col(d) = ai;
      D.push_back(A.dofs.sparse( A.dense.transpose() * ai) );

      
      std::vector< dof::velocity > v;
      core::each(D, [&](const dof::momentum& a) {
	  v.push_back( si(f + a, true));
	});
      

      mat Taiste = mat::Zero(obj.dim(), d+1);
      Taiste.each_col([&](natural j) {
	  Taiste.col(j) = obj(v[j], D[j]);
	});
      
      math::mosek mosek;
      
      mosek.init(d + 1, 1);
      mosek.Q(mosek.variables(), Taiste.transpose() * Taiste);
      
      // convex combination
      mosek.A(mosek.constraints(), mosek.variables(), vec::Ones(mosek.variables().end).transpose());
      mosek.b(mosek.constraints(), vec1::Ones(), mosek::bound::fixed); 
      
      mosek.bounds(mosek.variables(), vec::Zero(mosek.variables().end), mosek::bound::lower);
      
      const vec lambda = mosek.solve(mosek.variables());
      
      // // objective jacobian
      // mat Jobj; Jobj.resize( obj.dim(), a);
      // Jobj.each_col([&](natural j) {
      // 	  Jobj.col(j) = (obj(v[j], D[j]) - obj(v[j + a], D[j + a])) / (2 * h);
      // 	});
      
      // // current value
      // dof::momentum current_act = A.dofs.sparse( A.dense.transpose() * ai);
      // vec value = obj( si( f + current_act),
      // 		       current_act  );
      // assert( value.rows() == Jobj.rows() );
      
      

      // log("objective jacobian: ", Jobj.rows(), " ", Jobj.cols());

      // // mat JJt = Jobj * Jobj.transpose();
      // mat JtJ = Jobj.transpose() * Jobj;
      
      // // levenberg-marquardt damping:
      // // JtJ.diagonal().array() *= (1 + epsilon);
      // JtJ.diagonal().array() += epsilon;

      // vec gradient = JtJ.fullPivLu().solve(-Jobj.transpose() * value - epsilon * ai);
      // assert(!nan(gradient));
      
      // // vec gradient = Jobj.transpose() * lambda;
      // // const real max = std::sqrt( Jobj.cols() * 4 * h * h );
      // // if(gradient.norm() > max) { 
      // // 	log("projected from", gradient.norm(), " to ", max);
      // // 	gradient.normalize(); 
      // // 	gradient *= max;
      // // }
      
      // // actuation stepping
      // real step = 1;
      // ai += step * gradient; // FUUUUU
      
      ai = Act * lambda;
      
      ai = ai.cwiseMin( bounds ).cwiseMax(-bounds);
      act = A.keys.sparse(ai);
      
      log("act: ", ai.transpose());
      // log("step: ", (step * gradient).transpose());
      
      // final solve
      return si( f + A.dofs.sparse( A.dense.transpose() * ai) );
      
    }



  }
}
