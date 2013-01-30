// #include <sparse/matrix.h>
// #include <sparse/weight.h>

// #include "main.h"

// #include <gl/common.h>
// #include <gl/color.h>
// #include <gl/init.h>

// #include <core/stl.h>

// #include <math/vec.h>
// #include <math/form.h>


// #include <QApplication>
// #include <QVBoxLayout>
// #include <QKeyEvent>

// #include <phys/system.h>
// #include <core/unique.h>

// #include <math/func/vector.h>
// #include <core/log.h>

// #include <script/api.h>

// #include <core/raii.h>

// #include <gl/blend.h>
// #include <math/mat.h>

// #include <phys/constant.h>

// #include <phys/solver/cholesky.h>
// #include <phys/solver/ref.h>

// #include <gl/raii.h>

// #include <phys/constraint/bilateral.h>
// #include <phys/solver/bill.h>

// #include <phys/solver/simple.h>
// #include <phys/solver/minres.h>

// #include <math/random.h>

// #include <gui/convert.h>


// #include <phys/step/kinematic.h>




// #include <math/mat.h>
// #include <math/vec.h>
// #include <math/quat.h>

#include <gui/app.h>
#include "viewer.h"
#include <script/meuh.h>

// #include <tool/fix.h>
// macro_pthread_fix;


 
using namespace math;


// namespace {
//   static core::logger log;

// }

  
// void linear::init() {
    
//   setSceneRadius(30);
//   showEntireScene();
//   setAnimationPeriod( 0 );
//   startAnimation();
//   setMouseTracking(true);

//   dt = 0.01;
//   t = 0;

//   frame.resize(2);
//   for(natural i = 0; i < frame.size(); ++i) {
//     frame[i] = new gui::frame;
//   }
  
 
//   script::exec("hook(linear.post_init)");
// }


// void linear::compare() {
//   saveAnimation();
//   stopAnimation();

//   using namespace phys::solver;
  
  
//   restoreAnimation();
// }


// void linear::draw() {
//   using namespace gl;
  
//   if( obj.empty() ) return;

//   line_width(4);
 

 
    
//   cull::enable();
//   cull::face(GL_BACK);
    
//   // blend()([&]{
//   gl::color( color.cloth );
//   glEnable( GL_POLYGON_OFFSET_FILL );
//   glPolygonOffset( 1.f, 1.f );
//   begin(GL_TRIANGLES, [&] {
//       core::each( display.mesh.quads, [&](const geo::mesh::quad& q) {
// 	  const vec3& a = obj[ q.vertex[0] ].g();
// 	  const vec3& b = obj[ q.vertex[1] ].g();
// 	  const vec3& c = obj[ q.vertex[2] ].g();
// 	  const vec3& d = obj[ q.vertex[3] ].g();
		
// 	  normal( (b - a).cross(c - a) );
// 	  vertex( a, b, c);
		
// 	  normal( -(b - a).cross(c - a) );
// 	  vertex( a, c, b);
		
// 	  normal( (c - a).cross(d - a) );
// 	  vertex( a, c, d);
		
// 	  normal( -(c - a).cross(d - a) );
// 	  vertex( a, d, c);
// 	});
//     });
//   glDisable( GL_POLYGON_OFFSET_FILL );
//   cull::disable();
//     // });
    

//   auto transfert = [&](math::real x) -> math::real { 
//     return std::sqrt( std::sqrt( x ) ); 
//   };

//   math::real min = 1e42, max = 0;
//   solver.mu.each([&](phys::constraint::bilateral::key c,
// 		     sparse::vec_chunk v) {
// 		   const math::real theta = v.norm();
		   
// 		   if( theta < min ) min = theta;
// 		   if( theta > max ) max = theta;
// 		 });
  
  
//   if(flags.show_graph) {
//     // auto depth = raii::disable(GL_DEPTH_TEST);
//     auto light = raii::disable(GL_LIGHTING);
    
//     blend()([&]{
// 	begin(GL_LINES, [&] {
	    
// 	    core::each(boost::edges(constraint.mesh), [&](const mesh_type::edge_descriptor e) {
		
// 		const real w = solver.mu( constraint.mesh[e].key).norm();
		
// 		const real alpha = transfert( (w - min) / ( max - min ) );
		
// 		vec3 c = alpha * color.hot + (1 - alpha) * color.cold;

// 		// in graph
// 		if( solver.in_tree && solver.in_tree( constraint.mesh[e].key ) ) {
// 		  gl::color( transp( gl::hsv(c), 0.3) );

		  	
// 		  natural i = boost::source(e, constraint.mesh);
// 		  natural j = boost::target(e, constraint.mesh);

// 		  normal((obj[i].g(), obj[j].g()).normalized());
// 		  vertex(obj[i].g(), obj[j].g());

// 		} else {
// 		  // gl::color( transp( hsv(c), 0.3) );
// 		}

	

	
// 	      });

// 	    core::each(constraint.fixed, [&](gui::frame* f, const constraint_type::attach& a) {
// 		auto o = (const obj_type*)(a.dof);
		
// 		const real w = solver.mu( a.key ).norm();
		
// 		const real alpha = transfert( (w - min) / ( max - min ) );
		
// 		vec3 c = alpha * color.hot + (1 - alpha) * color.cold;

// 		gl::color( transp( gl::hsv(c), 0.3) );
// 		vertex(o->g(), f->transform().translation());
// 	      });
	    
// 	  });
//       });
//   }
  

    
//   core::each(frame, [&](gui::frame* f) {
//       f->draw( sceneRadius() / 3);
//     });

//   camera()->setRevolveAroundPoint( gui::convert( obj[ (obj.size() + width) / 2 ].g() ) );
// }


// void linear::setup(math::natural n) {
//   setup(n, n);
// }

// void linear::setup(math::natural w, math::natural h) {
//   auto lock = this->lock();
//   width = w;
//   height = h;
  
//   // setup physical system
//   solver.clear();
//   data.engine.clear();
  
//   const natural size = width * height;

//   // dofs
//   obj.resize( width * size );
  
//   total_mass = 500 * phys::unit::g;
//   particle_mass = total_mass / size;
  
//   core::each( obj, [&](obj_type& o) {

//       solver.dofs.push_back( &o );
//       solver.setMass(solver.dofs.size() - 1, particle_mass);
      
//       data.engine.add(&o);
//     });

//   // length constraints
//   constraint.mesh = mesh_type( width * height );
  
//   auto pos = [&](natural i) -> vec3& {
//     return obj[i].g();
//   };

//   auto vel = [&](natural i) -> vec3& {
//     return obj[i].v();
//   };

//   auto mom = [&](natural i) -> form3& {
//     return obj[i].p();
//   };

//   // edge creation
//   auto edge = [&](real rest) -> edge_type {
//     edge_type res;
//     res.rest = rest;
    
//     // FIXME memleak
//     res.key = new phys::constraint::bilateral(1);
//     solver.bilateral.push_back( res.key );
    
//     return res;
//   };

 
//   // place vertices
//   for(natural i = 0 ; i < size; ++i ) {
//     const natural x = i / width;
//     const natural y = i % width;
    
//     pos(i) = (sceneRadius()/2) * vec3(y, 0, x) / width;
//     vel(i).setZero();
//     mom(i).setZero();
//   }

//   // create edges
//   natural e = 0;
//   for(natural i = 0 ; i < size; ++i ) {
//     const natural x = i / width;
//     const natural y = i % width;
      
//     bool ok_height = (x < height - 1);
//     bool ok_width = (y < width - 1);
      
//     if( ok_height ) {
//       real rest = (pos(i) - pos(i + width)).norm();
	
//       boost::add_edge(i, i + width, edge(rest), constraint.mesh);
//     }

//     if( ok_width ) { 
//       real rest = (pos(i) - pos(i + 1)).norm();
//       boost::add_edge(i, i + 1, edge(rest), constraint.mesh);
//     }
    
//   }
  
//   // pin constraints
//   core::each(frame, [&](gui::frame* f) {
//       constraint_type::attach at;
//       // at.key = new phys::constraint::bilateral(3);
//       at.key = new phys::constraint::bilateral(1); 
//       solver.bilateral.push_back(at.key);
//       constraint.fixed[ f ] = at;
//     });
  
  
//   math::real off = 1.0;
  
//   // pin top left/right 
//   frame[0]->transform( SE3::translation( pos(0) + off * vec3(-1, 1, 0) ) );
//   constraint.fixed[ frame[0] ].dof = &obj[0];
//   constraint.fixed[ frame[0] ].rest = (frame[0]->transform().translation() - obj[0].g()).norm();
  
//   if( frame.size() > 1 ) {
//     frame[1]->transform( SE3::translation( pos(width - 1) + off * vec3(1, 1, 0) ) );
//     constraint.fixed[ frame[1] ].dof = &obj[width - 1];
//     constraint.fixed[ frame[1] ].rest = (frame[1]->transform().translation() - obj[width - 1].g()).norm();
//   }
  
//   // display mesh
//   display.mesh.clear();
    
//   for(natural i = 0 ; i < size; ++i ) {
//     const natural x = i / width;
//     const natural y = i % width;

//     bool ok_height = (x < height - 1);
//     bool ok_width = (y < width - 1);

//     if( ok_height && ok_width ) {
//       display.mesh.quads.push_back(geo::mesh::quad{i, i + 1, i + width + 1, i + width } );
//     }
//   };

  
//   // 
//   const math::real gamma = 1;
  
//   // constraints
//   // setup constraints
//   length_constraints = [&, gamma] {
    
//     core::each( boost::edges(constraint.mesh), [&](const mesh_type::edge_descriptor& e) {
// 	natural i = boost::source(e, constraint.mesh);
// 	natural j = boost::target(e, constraint.mesh);
	
// 	auto info = constraint.mesh[e];
	
// 	const vec3 diff = obj[i].g() - obj[j].g(); 

// 	if( diff.norm() < 1e-5 ) core::log("ALARRRM");
	
// 	const vec3 n = diff.normalized();
	
// 	const real scale = 1; // (1 + solver.mu(info.key).norm());

// 	auto& J = solver.system.constraint.bilateral.matrix;

// 	J(info.key, &obj[i]) = scale * 2 * diff.transpose(); //  n.transpose();
// 	J(info.key, &obj[j]) = -2 * scale * diff.transpose(); // - n.transpose();
	
// 	auto& b = solver.system.constraint.bilateral.values;
// 	b(info.key).setZero();
	
// 	auto& c = solver.system.constraint.bilateral.corrections;
// 	c(info.key).setConstant( scale * (info.rest * info.rest - diff.squaredNorm())  );
//       });
//   };
  
  
//   pin_constraints = [&, gamma ] {
    
//     core::each(constraint.fixed, [&](gui::frame* f, const constraint_type::attach& at) {
// 	vec3 diff = ((obj_type*)at.dof)->g() - f->transform().translation();

// 	auto& J = solver.system.constraint.bilateral.matrix;
// 	auto& b = solver.system.constraint.bilateral.values;
// 	auto& c = solver.system.constraint.bilateral.corrections;
	
// 	// 1-dimensional
// 	J(at.key, at.dof) = 2 * diff.transpose();
// 	if( diff.norm() < 1e-5) core::log(HERE, "ALARM !");

// 	b( at.key ).setZero();
// 	c( at.key ).setConstant( at.rest * at.rest  - diff.squaredNorm() ) ;
	
// 	// 3-dimensional
// 	// J(at.key, at.dof).setIdentity();
// 	// c(at.key) = -diff;
	
//       });
    
//   };

  
//   // pin_constraints = 0;
//   solver.non_linear_error = [&] (const phys::dof::velocity& v) {
//     math::real res = 0;
    
//     core::each( boost::edges(constraint.mesh), [&](const mesh_type::edge_descriptor& e) {
// 	natural i = boost::source(e, constraint.mesh);
// 	natural j = boost::target(e, constraint.mesh);
	
// 	auto info = constraint.mesh[e];
	
// 	vec3 diff = obj[i].g() - obj[j].g(); 
	
// 	// fake step
// 	v.find(&obj[i], [&](sparse::const_vec_chunk vi) {
// 	    diff += vi;
// 	  });

// 	v.find(&obj[j], [&](sparse::const_vec_chunk vj) {
// 	    diff -= vj;
// 	  });

// 	math::real delta = info.rest - diff.norm();
// 	res += delta * delta;
//       });

//     core::each(constraint.fixed, [&](gui::frame* f, const constraint_type::attach& at) {
	
// 	vec3 diff = ((obj_type*)at.dof)->g() - f->transform().translation();

// 	// fake step
// 	v.find(at.dof, [&](sparse::const_vec_chunk v) {
// 	    diff += v;
// 	  });
	
// 	math::real delta = at.rest - diff.norm();
// 	res += delta * delta;
//       });
    
//     return res;
//   };
  
//   solver.non_linear_error = 0;




//   // fuuu
  
//   auto p = [&](math::real t, math::real dt) { return this->momentum(t, dt); };
  
//   auto s = [&]() -> phys::solver::any {
//     constraints();					

//     phys::solver::simple simple( solver.system );
//     auto res = phys::solver::make_cg( simple, solver.system  );


//     res.config.precision = solver.config().cg().getError();
//     res.config.iterations = solver.config().cg().getIterations();
    
//     //  res.warm = &warm;
    
//     return res;
//   };

//   auto integrator = [&](const phys::dof::velocity& v, math::real dt) { this->integrate(v, dt); };
  
//   player.animator = phys::step::kinematic(solver.system, s, p, integrator);
  
//   // warm = math::vec();
// };
 

// void linear::constraints() {
//     solver.system.constraint.bilateral =  phys::system::constraint_type::bilateral_type();
    
//   if( pin_constraints ) pin_constraints();
//   if( length_constraints ) length_constraints();
// }

// void linear::setup() {
  
 
// }

   


// phys::dof::velocity linear::correction( const phys::dof::velocity& delta_sum) {
//   using namespace sparse;

//   solver.momentum = -(solver.system.mass * delta_sum);
//   solver.correct();

//   return solver.delta;
// }

// void linear::fix(const phys::dof::velocity& v,
// 		 const phys::dof::velocity& delta) {
//   // phys::dof::momentum Mdelta = solver.system.mass * delta;
//   // solver.velocity = v - (sparse::dot(v, Mdelta) / sparse::dot(delta, Mdelta)) * delta;
//   solver.velocity = v + delta / dt;
  
//   data.engine.set( solver.velocity, solver.system.mass * solver.velocity );
// }

// static phys::constraint::bilateral::weight weights(const phys::constraint::bilateral::vector& lambda) {

//   phys::constraint::bilateral::weight res;

//   lambda.each( [&](phys::constraint::bilateral::key k, sparse::const_vec_chunk v) {
//       res(k) = -v.squaredNorm();
//     });
  
//   return res;
// }

// void linear::factor(bool fast) {
  
//   solver.factor( fast );
// }


// phys::dof::velocity linear::solve(const phys::dof::momentum& momentum) {
  
//   phys::solver::task task(solver.velocity);
//   task.momentum = momentum;

//   solver.momentum = momentum;
  
//   phys::solver::simple(solver.system).solve( task );
//   // solver.solve();
  
//   return solver.velocity;
// }


// linear::Solver::Solver () {
//   minres = false;
// }

// linear::linear(QWidget* parent) 
//   : tool::simulator( parent ),
//     app("linear")
// {
//   weight.epsilon = 1;
//   flags.show_graph = false;

//   solver_kind = cg;
//   graph_kind = 0;
 
//   setAxisIsDrawn(true);
  
//   // those are HSV !
//   color.hot = math::vec3(0, 1, 1);
//   color.cold = math::vec3(math::pi / 3, 0, 0.8);
  
//   color.cloth = gl::html( 0x334477 );
    

//   app
//     .ref("width", width)
//     // .ref("eps", weight.epsilon )
//     .ref("show_graph", flags.show_graph)
//     .ref("solver_kind", solver_kind)
//     // .ref("graph_kind", graph_kind)
//     ;


//   app
//     .fun("eps", [&](math::real e) {
// 	solver.config().cg().setError( e );
//       })
//     .fun("graph", [&](math::natural i) {
// 	solver.config().graph().setType( Graph::Type(i) );
//       })
//     .fun("damping", [&](math::real i) {
// 	solver.config().cg().setDamping( i );	
//       })
//     .fun("iter", [&](math::natural i) {
// 	solver.config().cg().setIterations( i );	
//       })
//     .fun("color", [&](math::natural i) {
// 	color.cloth = gl::html(i);
//       })
//     .fun("stiffness", [&](math::real s) {
// 	stiffness = s;
//       })
//     .fun("prec", [&](bool b) {
// 	solver.config().cg().setPrec( b );
//       })
//     .fun("dim", [&](math::natural w, math::natural h) {
// 	saveAnimation();
// 	stopAnimation();
// 	tool::simulator::reset();
// 	setup(w, h);
// 	restoreAnimation();
//       })
//     .fun("solve", [&] {
// 	solver.factor( false );
// 	script::exec("plot.show( cg )");
//       })
//     .fun("minres", [&](bool b) {
// 	solver.config().cg().setMinres( b );
//       })
//     ;
  
//   stiffness = 0;
  
//   script::require("linear");
// }




// // template<template<class> class Solver>
// // struct solver {
// //   phys::solver::linear linear;
// //   phys::constraint::bilateral::matrix cycles;
// //   const Solver<phys::solver::reference<phys::solver::linear> > full;
  
// //   template<class Isolated>
// //   solver(math::real epsilon, 
// // 	 const phys::constraint::bilateral::matrix& matrix,
// // 	 const Isolated& isolated)
// //     // TODO broken
// //     : // cycles(sparse::keep(matrix, sort( isolated(linear)))),
// //       full( ref(linear),
// // 	    cycles,
// // 	    epsilon) {
    
// //   }

// //   solver(const solver& other) 
// //     : linear(other.linear),
// //       cycles(other.cycles),
// //       full( ref(linear),
// // 	    cycles,
// // 	    other.full.epsilon) {
    
// //   }

// //   void solve( const phys::solver::task& task) const { full.solve(task); }
  
// //   template<class C>
// //   static std::set<C> sort(const std::vector<C>& data) {
// //     return std::set<C>(data.begin(), data.end());
// //   }
  
// // };



// // phys::solver::any linear::make_solver() {

// //   // return phys::solver::soft(system);
  
// //   auto normal = [&](phys::solver::linear& linear) {
// //     linear.init(system);
// //     return linear.setup();
// //   };
  
// //   auto lambda = [&](phys::solver::linear& linear) {
// //     linear.init(system);
// //     return linear.setup( linear.mst( weights(this->lambda) ) );
// //   };

// //   auto propagate = [&](phys::solver::linear& linear) {
// //     phys::dof::velocity res;
// //     phys::solver::task task(res);

// //     // TODO derp
// //     task.momentum = engine.net( force(t, dt), dt );
// //     task.bilateral = system.constraint.bilateral.values;
    
// //     linear.init(system);
// //     return linear.setup( linear.propagation(task) );
// //   };

  
// //   auto breadth = [&](phys::solver::linear& linear) {
    
// //     linear.init(system);
// //     return linear.setup( linear.bfs_weight(weights(this->lambda)) );
// //   };

  
// //   std::function< phys::solver::linear::isolated_type (phys::solver::linear& ) > isolated;

// //   switch(graph_kind) {
// //   case dfs: isolated = normal;
// //     break;
// //   case mst: isolated = lambda;
// //     break;
// //   case prop: isolated = propagate;
// //     break;
// //   case bfs: isolated = breadth;
// //     break;
// //   default: throw core::error(HERE);
// //   };

// //   switch(solver_kind) {
// //   case cholesky:
// //     return ::solver< phys::solver::cholesky >(weight.epsilon, system.constraint.bilateral.matrix, isolated );
// //   case cg:
// //     return ::solver< phys::solver::cg >(weight.epsilon, system.constraint.bilateral.matrix, isolated );
// //   default: throw core::error(macro_here);
// //   };
  
 
// // }



// phys::dof::force linear::force(math::real t, math::real dt)  {
//   phys::dof::momentum f( solver.system.mass.keys().size() );
  
//   // gravity
//   core::each( obj, [&](const obj_type& o) {
//       f(&o) += - solver.system.mass( &o, &o ) * (phys::constant::g * math::vec3::UnitY());
//     });


//   // springs
//   auto track_force = [&](natural i, gui::frame* g) {
      
//     const real stiffness = 5 * phys::unit::N / phys::unit::m;
      
//     f( &obj[i] ) += stiffness * ( g->transform().translation() - obj[i].g());

//     // system.mass[ &obj[i] ].diagonal() = (1 + (dt * dt * stiffness)) * vec3::Ones();
//     // system.resp.[ &obj[i] ].diagonal() = system.mass[ &obj[i] ].diagonal().cwiseInverse();
//   };


//   auto cross = [&] {
//     if( stiffness <= 0 ) return;
    
//     core::each( boost::edges(constraint.mesh), [&](const mesh_type::edge_descriptor& e) {
// 	natural i = boost::source(e, constraint.mesh);
// 	natural j = boost::target(e, constraint.mesh);

// 	bool same_row = std::abs(int(i) - int(j)) == 1;
// 	bool same_col = std::abs(int(i) - int(j)) == width;
      
// 	real stiff = stiffness * 100 * particle_mass;

// 	if( same_row ) {
// 	  natural row = i / width;
// 	  if( row < height - 1 ) {

// 	    natural s = std::min(i, j);
// 	    natural t = std::max(i, j) + width;
	  
// 	    vec3 diff = obj[s].g() - obj[t].g();
// 	    vec3 n = diff.normalized();

// 	    real rest = constraint.mesh[e].rest * std::sqrt(2);
	  
// 	    vec3 force = -stiffness * n * (diff.norm() - rest);

// 	    f(&obj[s]) += force;
// 	    f(&obj[t]) -= force;
	  
// 	  }	
// 	} else if( same_col ) {
// 	  natural col = i % width;
// 	  if( col < width - 1 ) {
// 	    natural s = std::min(i, j) + 1;
// 	    natural t = std::max(i, j);

// 	    vec3 diff = obj[s].g() - obj[t].g();
// 	    vec3 n = diff.normalized();

// 	    real rest = constraint.mesh[e].rest * std::sqrt( 2.0 );
	  
// 	    vec3 force = - stiffness * n * (diff.norm() - rest);

// 	    f( &obj[s] ) += force;
// 	    f( &obj[t] ) -= force;
	  
// 	  }
// 	} else { 
// 	  core::log("i:", i, "j:", j, "diff:", std::abs(i - j), "width:", width);
// 	  throw core::error(HERE);
// 	}      
      
//       });
//   };  

//   // cross();

//   // fixed extremities
//   // if( !pin_constraints ) {
//   //   track_force(0, frame[0]);
//   //   track_force(width - 1, frame[1] );
//   // }
  
//   return f;
// }

// phys::dof::momentum linear::momentum(math::real t, math::real dt) {
//   return data.engine.momentum( force(t, dt), dt );
// }

// void linear::integrate(const phys::dof::velocity& v, math::real dt) {
//   using sparse::operator*;
//   data.engine.integrate(v, solver.system.mass * v, dt);
// }


// void linear::correct(const phys::dof::velocity& delta) {

//   // correct positions
//   data.engine.correct( delta );

// }



int main(int argc, char** argv) {
  gui::app app(argc, argv);
  
  // ::linear* viewer = new ::linear;
  
  viewer* wid = new viewer;
  wid->show();
  
  script::exec("app = require 'linear2'");
  
  return app.console().exec();
}
