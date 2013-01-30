#include "soft.h"

namespace phys {
  namespace solver {

    soft::soft(const phys::system& system) : system(system) { }

    void soft::solve(const solver::task& task) const {

      // TODO init guess
      math::real dt = 0.3;
      math::real dt2 = dt*dt;

      math::real omega = 0.1;
      math::real alpha = 0.1;
      math::real stiffness = 1;

      auto& J = system.constraint.bilateral.matrix;
      auto& b = task.bilateral;
      
      auto& v = task.velocity;
      
      using sparse::linear::operator+;
      using sparse::linear::operator*;
      using sparse::linear::operator-;
      dof::momentum rhs = task.momentum + stiffness * ( sparse::linear::transp(J) * b);

      v.clear();
      for(int i = 0; i < 1; ++i) {
	using sparse::linear::operator+;
	using sparse::linear::operator*;
	using sparse::linear::operator-;
	v = (sparse::linear::diag(system.resp) * (rhs - stiffness * (sparse::linear::transp(J) * (J * v) )  ));
	
	core::log("error:", i, sparse::linear::norm(J * v - b) );

      }
      
      
    }
    
  } 
}
