#include "fast_proj.h"

#include <phys/dof.h>
#include <phys/solver/task.h>
#include <phys/step/data.h>
#include <phys/step/simple.h>

#include <sparse/sum.h>
#include <sparse/scalar.h>
#include <sparse/diff.h>
#include <sparse/prod.h>

#include <phys/constraint/bilateral.h>


namespace phys {
  namespace step {

    // animator_type fast_proj(const data& d, const solver_type& solver, const force_type& f, math::natural steps) {
      
    //   animator_type unconstrained = step::simple(d, f);
       
    //   return [&d, unconstrained, solver,f, steps](math::real t, math::real dt) {

    // 	// unconstrained step
    // 	unconstrained(t, dt);
	
    // 	// projections
    // 	dof::velocity total;
    // 	dof::velocity c;
    // 	phys::solver::task task(c);
	
    // 	for(math::natural i = 0; i < steps; ++i) {
	  
    // 	  // generates solver and evaluates constraints
    // 	  auto s = solver();
	  
    // 	  task.bilateral = d.system.constraint.bilateral.corrections;
    // 	  s.solve( task );
	  
    // 	  d.engine.correct( c );
	  
    // 	  total += c;
    // 	}
	
    // 	// adapt velocities
    // 	dof::velocity v = total / dt;
    // 	dof::momentum p = d.system.mass * v;
	
    // 	d.engine.add(v, p); 
    //   };
    // }





    animator_type fast_proj(const data& d, const solver_type& solver, const force_type& f, const math::natural& steps) {
      
      return [&d, solver,f, &steps](math::real t, math::real dt) {

	// projections
	dof::velocity last;
	dof::velocity v;
	phys::solver::task task(v);
	
	phys::constraint::bilateral::vector lambda;
	task.lambda = &lambda;
	
	task.momentum = d.momentum( f(t, dt), dt );

	for(math::natural i = 0; i < steps; ++i) {
	  
	  // generates solver and evaluates constraints
	  auto s = solver();
	  
	  task.bilateral = 
	    (1/dt) * d.system.constraint.bilateral.corrections + d.system.constraint.bilateral.matrix * v
	    ;
	  
	  s.solve( task );
	  
	  // muhahahaha
	  d.engine.correct( (0.5 * dt ) * (v - last) );
	  task.momentum = d.system.mass * v;
	  
	  last = v;
	}
	
	d.engine.set(v,  d.system.mass * v); 
      };
    }


    animator_type fast_proj(const data& d, const solver_type& solver, const force_type& f, const math::natural& steps,
			     const std::function<void(const phys::solver::task&) >& bench) {
      
      return [&d, solver,f, &steps, bench](math::real t, math::real dt) {

	// projections
	dof::velocity last;
	dof::velocity v;
	phys::solver::task task(v);
	
	phys::constraint::bilateral::vector lambda;
	task.lambda = &lambda;
	
	task.momentum = d.momentum( f(t, dt), dt );

	for(math::natural i = 0; i < steps; ++i) {
	  
	  // generates solver and evaluates constraints
	  auto s = solver();
	  
	  task.bilateral = 
	    (1/dt) * d.system.constraint.bilateral.corrections + d.system.constraint.bilateral.matrix * v
	    ;
	  
	  s.solve( task );
	  
	  if( i == 0 ) { 
	    // task is beeing fed *THIS* timestep lambdas
	    bench( task );
	  }  
	  
	  // muhahahaha
	  d.engine.correct( (0.5 * dt ) * (v - last) );
	  task.momentum = d.system.mass * v;
	  
	  last = v;
	}
	
	d.engine.set(v,  d.system.mass * v); 
      };
    }




    // // TODO COPYPASTA
    // animator_type fast_proj(const data& d, const solver_type& solver, const force_type& f, math::natural steps,
    // 			    const std::function<void(const phys::solver::task&) >& bench) {
      
    //   animator_type unconstrained = step::simple(d, f);
      
    //   return [&d, unconstrained, solver,f, steps, bench](math::real t, math::real dt) {

    // 	// unconstrained step
    // 	unconstrained(t, dt);
	
    // 	// projections
    // 	dof::velocity total;
    // 	dof::velocity c;
    // 	phys::solver::task task(c);
	
    // 	constraint::bilateral::vector lambda;
    // 	task.lambda = &lambda;
	
    // 	for(math::natural i = 0; i < steps; ++i) {
	  
    // 	  // generates solver and evaluates constraints
    // 	  auto s = solver();
	  
    // 	  task.bilateral = d.system.constraint.bilateral.corrections;
    // 	  s.solve( task );
	  
    // 	  if( i == 0 ) { 
    // 	    // task is beeing fed *THIS* timestep lambdas
    // 	    bench( task );
    // 	  }  
	  
    // 	  d.engine.correct( c );
	  
    // 	  total += c;
	  

    // 	}
	
    // 	// adapt velocities
    // 	dof::velocity v = total / dt;
    // 	dof::momentum p = d.system.mass * v;
	
    // 	d.engine.add(v, p); 
    //   };
    // }


    
  }
}
