#include "viewer.h"

#include <math/form.h>
#include <math/vec.h>

#include <phys/step/simple.h>
#include <phys/step/kinematic.h>
#include <phys/step/post_stab.h>
#include <phys/step/fast_proj.h>

#include <phys/solver/simple.h>
#include <phys/solver/cg.h>

#include <sparse/sum.h>

#include <phys/solver/schur.h>
#include <phys/solver/test.h>

#include <tool/benchmark.h>



#include <math/cg.h>
#include <math/minres.h>

#include <math/pcg.h>
#include <math/pminres.h>

#include <sparse/weight.h>
#include <sparse/diag.h>

#include <core/unique.h>


#include "pminres.h"

using namespace math;

void viewer::bilateral() {

  if( cloth ) {
    
    // length constraints
    cloth->bilateral( data.system );

    // pin constraints
    core::each(pinned, [&](gui::frame* f, const pinned_type& at) {
	auto dof = &cloth->dofs[at.index];
    	vec3 diff =  dof->g() - f->transform().translation();
	
    	auto& J = data.system.constraint.bilateral.matrix;
    	auto& b = data.system.constraint.bilateral.values;
    	auto& c = data.system.constraint.bilateral.corrections;
	
	real norm = 2 * diff.norm();

    	// 1-dimensional
    	J(at.key, dof) = diff.normalized().transpose();
	
    	if( norm < 1e-5) core::log(HERE, "ALARM !");
	
    	b( at.key ).setZero();
    	c( at.key ).setConstant( (at.rest * at.rest  - diff.squaredNorm()) / norm ) ;
	
    	// 3-dimensional
    	// J(at.key, at.dof).setIdentity();
    	// c(at.key) = -diff;
	
      });


  }  
}

void viewer::init() {
  
  // animation loop
  auto force = [&](math::real t, math::real dt) {
    
    phys::dof::force res;
    if( cloth ) res += cloth->force();
    
    return res;
  };



  // task contains the freshly computed lambdas for *this* step, so
  // back them up and use this backup for temporal coherence
   
  auto benchmark = [&](const phys::solver::task& task) mutable {
    
    assert( task.lambda );
    phys::constraint::bilateral::vector lambda_next = *task.lambda;

    const phys::solver::simple simple(data.system);

    auto& big_J = data.system.constraint.bilateral.matrix;

    phys::solver::any sub = simple;
    
    auto big_resp = [&](const math::vec& x) -> phys::dof::velocity {
      phys::dof::velocity res;
      phys::solver::task t(res);
      
      t.momentum = task.momentum + sparse::transp(big_J) * sparse::dense(big_J.rows(), x);
      t.bilateral = task.bilateral;
      
      sub.solve(t);
      return res;
    };

    
    math::iter iter;
    iter.bound = this->iterations;
    iter.epsilon = 1e-16;

    // plot name
    const std::string plot = core::arg("test[%%]", this->player.t);
    
    auto linear_error = [&](std::function< phys::dof::velocity (const math::vec& x) > vel) 
    -> std::function< void(math::natural, const math::vec& ) > {
      return [&, vel](math::natural k, const math::vec& x) {
      
	phys::constraint::bilateral::vector residual = 
	data.system.constraint.bilateral.matrix * vel(x) - task.bilateral;

	script::exec("plot.push(%%, %%, %%)", plot, k, sparse::norm(residual) );
      };
    };
    
    auto non_linear_error = [&](std::function< phys::dof::velocity (const math::vec& x) > vel) 
    -> std::function< void(math::natural, const math::vec& ) >
    {
      
      return [&, vel](math::natural k, const math::vec& x) {
	if(!cloth) return;
      
	auto v = vel(x);
      
	math::vector< math::vec3 > delta;
	delta.resize( cloth->dofs.size() );
      
	// TODO dt ?
	math::natural i = 0;
	core::each( cloth->dofs, [&](const cloth_type::obj_type& obj) {
	    delta(i) = dt * v(&obj);
	    ++i;
	  });

	math::real error2 = cloth->error2(delta);

	core::each(pinned, [&](gui::frame* f, const pinned_type& at) {
	    
	    vec3 diff = cloth->dofs[at.index].g() + delta(at.index) - f->transform().translation();
	    error2 += diff.squaredNorm() - at.rest * at.rest;
	  });
	
	script::exec("plot.push(%%, %%, %%)", plot, k, std::sqrt( error2 ) );
      };
    };


    iter.cb = [&](math::natural k, math::real eps) {
      script::exec("plot.push(%%, %%, %%)", plot, k, eps);
    };
    
    tool::bench::solver_set set;
    
    using namespace phys::solver;
    auto big = schur::make_bilateral(data.system);

    set["minres"] = schur::make_iterative( math::minres(big), iter);
    // set["cg"] =     schur::make_iterative( math::cg(system), iter );
    
    {
     
      auto backup = iter.cb;
      iter.cb = 0;
	
      auto id = [&](const math::vec& x) -> const math::vec& { return x; };

      set["minres - nonlinear"] = schur::make_iterative( lol::pminres(big, id, non_linear_error(big_resp)), iter );
      iter.cb = backup;
    }


    // jacobi preconditionner: TODO move to schur ?
    {
      phys::constraint::bilateral::vector diag(data.system.constraint.bilateral.matrix.rows(),
					       math::vec());
    
      data.system.constraint.bilateral.matrix.each([&](phys::constraint::bilateral::key r,
						       phys::dof::key c,
						       sparse::mat_chunk Jrc) {
						     diag(r) += Jrc.rowwise().squaredNorm();
						   });
    
      auto jacobi = [&, diag](const math::vec& x) -> math::vec {
	return x.cwiseProduct( diag.dense().cwiseInverse() );
      };
    
      // set["pminres - jacobi"] = schur::make_iterative( math::pminres(system, jacobi), iter );
    //   set["pcg - jacobi"] = schur::make_iterative( math::pcg(system, jacobi), iter );
    }    
    
    // incomplete cholesky
    {
      auto graph = test::graph( data.system );
      
      auto make_prec = [&](const test::edge_set& edges) -> std::function< math::vec ( const math::vec& ) > {
	auto stuff = test::data_type{ test::span(graph, edges) };

	stuff.factor_precond(data.system);

	test::linear linear( std::move(stuff) );

	return [&,linear](const math::vec& x) -> math::vec {
	  using namespace phys;
	
	  dof::velocity v;
	  phys::solver::task t(v);
	  t.bilateral = sparse::dense(data.system.constraint.bilateral.matrix.rows(), x);
	
	  constraint::bilateral::vector lambda;
	  t.lambda = &lambda;

	  linear.precond( t ); 
	
	  return sparse::dense(data.system.constraint.bilateral.matrix.rows(), lambda);
	};
      };

      // rcm 
      auto rcm_edges = test::cuthill_mckee(graph);
      auto rcm_prec = make_prec( rcm_edges  );
      set["pminres - ic - rcm"]  = schur::make_iterative( math::pminres(big,rcm_prec), iter );
      
      {
	auto backup = iter.cb;
	iter.cb = 0;
	
	set["pminres - ic - rcm - nonlinear"] = schur::make_iterative( lol::pminres(big, rcm_prec, non_linear_error(big_resp)), iter );
	iter.cb = backup;
      }

      // mst
      phys::constraint::bilateral::weight weights;

      this->lambda.each([&](phys::constraint::bilateral::key c,
			    sparse::vec_chunk v) {
			  weights(c) = -v.norm();
			});
	
      auto mst_edges = test::mst( graph, weights );

      if(cloth) cloth->in_tree = test::acyclic(mst_edges, graph );
      auto mst_prec = make_prec( mst_edges );
      
      {
	auto backup = iter.cb;
	iter.cb = 0;
	
	set["pminres - ic - mst - nonlinear"] = schur::make_iterative( lol::pminres(big, mst_prec, non_linear_error(big_resp)), iter );
	
	iter.cb = backup;
      }
      set["pminres - ic - mst - linear"] = schur::make_iterative( pminres(big, mst_prec), iter );
      

      // prop
      auto prop_edges = test::prop( graph, data.system, task );
      auto prop_prec = make_prec( prop_edges );

      {
	auto backup = iter.cb;
	iter.cb = 0;
	
	set["pminres - ic - prop - nonlinear"] = schur::make_iterative( lol::pminres(big, prop_prec, non_linear_error(big_resp)), iter );
	
	iter.cb = backup;
      }
      set["pminres - ic - prop - linear"] = schur::make_iterative( pminres(big, prop_prec), iter );
      
      


      // small schur
      if( false ) {
      	test::data_type stuff{test::span(graph, mst_edges)};
      	stuff.factor( data.system );
      	test::linear linear( stuff );
      
      	auto J = stuff.span.loop_closures(data.system);
      	auto small = schur::make_bilateral(linear, J);

	auto small_resp = [&, J, linear](const math::vec& x) -> phys::dof::velocity {
	  phys::dof::velocity res;
	  phys::solver::task t(res);
      
	  t.momentum = task.momentum + sparse::transp(J) * sparse::dense(J.rows(), x);
	  t.bilateral = task.bilateral;
      
	  linear.solve(t);
	  return res;
	};



	auto backup = iter.cb;
	iter.cb = 0;

	auto id = [](const math::vec& x) -> math::vec { return x;};
	
	set["minres - schur - mst"] = schur::make_iterative( lol::pminres(small, id, linear_error(small_resp)), iter );
	set["minres - schur - mst - nonlinear"] = schur::make_iterative( lol::pminres(small, id, non_linear_error(small_resp)), iter );
	 
      	// auto mst = schur::make_iterative( math::minres(small), iter );
      	// set["minres - schur - mst"] = mst;

	iter.cb = backup;
      }

      // {
      // 	test::data_type stuff{test::span(graph, rcm_edges)};
      // 	stuff.factor_precond( data.system );
      // 	test::linear linear( stuff );
      
      // 	auto J = stuff.span.loop_closures(data.system);
      // 	auto small = schur::make_bilateral(linear, J);

      // 	auto mst = schur::make_iterative( math::minres(small), iter );
      // 	set["minres - schur - rcm"] = mst;
      // }

    } 
    
    // display error after solve to be sure
    auto after = [&] {
      core::log("sanity check:", sparse::norm( data.system.constraint.bilateral.matrix * task.velocity - task.bilateral ));
    };

    tool::bench::solver(plot, set, after)(task);

    this->lambda = lambda_next;
  };
  
  
  auto solver = [&]()  -> phys::solver::any {
    
    // updates bilateral
    this->bilateral();
    
    const phys::solver::simple simple(data.system);
    
    math::iter iter;
    iter.bound = this->iterations;
    iter.epsilon = 1e-16;
    
    using namespace phys::solver;
    auto system = schur::make_bilateral(data.system);
    auto res = schur::make_iterative( math::minres( system ), iter );
    
    // res.warm = true;

    // // small schur
    // test::graph_type graph = test::graph( data.system );
    // test::data_type stuff{test::span(graph, test::cuthill_mckee(graph))};
    //   stuff.factor(data.system);
    //   test::linear linear( stuff );

    //   auto small = schur::make_bilateral(linear, stuff.span.loop_closures(data.system) );

    //   core::push_back(iter.cb, [&](int k , real) {
    // 	  core::log("schur", k);
    // 	});

    //   auto bob = schur::make_iterative( math::minres(small), iter );

    // return bob;

    return res;
  }; 

  player.animator = phys::step::simple(data, force);
  player.animator = phys::step::kinematic(data, solver, force);
  player.animator = phys::step::post_stab(data, solver, force);

  player.animator = phys::step::fast_proj(data, solver, force, post_steps, benchmark);
  // player.animator = phys::step::full_proj(data, solver, force, post_steps);
  
  setAnimationPeriod(0);
  startAnimation();
}


void viewer::compliance(math::real comp)  {

  assert(cloth);

  
  if(comp) {
    core::each(cloth->constraints, [&](const phys::constraint::bilateral& c) {
	data.system.constraint.bilateral.damping(&c).setConstant(comp / (dt * dt) );
      });
  } else {
    data.system.constraint.bilateral.damping.clear();
  }
}

viewer::viewer() : api("linear") {
  // manipulator frames
  frame.resize( 2 );
  for(natural i = 0; i < frame.size(); ++i) {
    frame[i] = new gui::frame;
  }
 
  api
    .fun("setup", [&](math::natural i, math::natural j) {
	saveAnimation();
	stopAnimation();

	setup( core::unique<cloth_type>(i, j) );
	
	restoreAnimation();
      })
    .fun("post", [&](math::natural i) {
	post_steps = i;
      })
    .fun("compliance", [&](math::real c) {
	compliance(c);
      })
    .fun("color", [&](math::natural c) {
	assert(cloth);
	cloth->color = c;
      })
    .fun("iter", [&](math::natural i) {
	iterations = i;
      });
  

  post_steps = 10;
  iterations = 10;

  script::require("plot");
  script::require("linear2");  
}

void viewer::reset() {
  if(cloth) cloth->reset();
  
  lambda.clear();

  math::real off = cloth->scale / 10.0;
   
  // pin top left/right 
  frame[0]->transform( SE3::translation( cloth->dofs[0].g() + off * vec3(-1, 1, 0) ) );
  pinned[ frame[0] ].index = 0;
  pinned[ frame[0] ].rest = (frame[0]->transform().translation() - cloth->dofs[0].g()).norm();
  
  if( frame.size() > 1 ) {
    frame[1]->transform( SE3::translation( cloth->dofs[ cloth->width - 1].g() + off * vec3(1, 1, 0) ) );
    pinned[ frame[1] ].index = cloth->width - 1;
    pinned[ frame[1] ].rest = (frame[1]->transform().translation() - cloth->dofs[cloth->width - 1].g()).norm();
  }
  
  script::exec("test = {}");

  simulator::reset();
}


void viewer::setup(std::unique_ptr< cloth_type >&& c) {
  if( cloth ) {
    data.clear();
  }

  cloth = std::move(c);
  
  core::each(cloth->dofs, [&](obj_type& obj) {
      data.engine.add( &obj );
    });

  cloth->mass( data.system );  


  // pin constraints
  pinned.clear();
  
  core::each(frame, [&](gui::frame* f) {
      pinned_type at;
      at.key = new phys::constraint::bilateral(1); 
      // TODO MEMLEAK LOL

      pinned[ f ] = at;
    });
  
 
  
  reset();
  
}


void viewer::draw() {
  using namespace gl;
  
  if( cloth ) {
    color( grey() );
    cloth->draw();
 
  
  core::each(pinned, [&](gui::frame* f, const pinned_type& a) {
      f->draw( sceneRadius() / 3);
      
      auto o = &cloth->dofs[a.index];
      
      // const real w = solver.mu( a.key ).norm();
      // const real alpha = transfert( (w - min) / ( max - min ) );
      // vec3 c = alpha * color.hot + (1 - alpha) * color.cold;
      
      line_width(2.0);
      disable(GL_LIGHTING);
      begin(GL_LINES, [&] {
	  color( transp( red(), 0.3) );
	  vertex(o->g(), f->transform().translation());
	});
      enable(GL_LIGHTING);
    });
  }
}


