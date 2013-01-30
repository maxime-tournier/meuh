#include "graph.h"

#include <gl/common.h>
#include <gl/init.h>

#include <core/stl.h>

#include <math/vec.h>
#include <math/form.h>


#include <QApplication>
#include <QVBoxLayout>
#include <QKeyEvent>

#include <phys/system.h>
#include <core/unique.h>

#include <phys/solver/qp/direct.h>
#include <phys/solver/bilateral.h>

#include <sparse/linear/diagonal.h>

#include <math/func/vector.h>
#include <core/log.h>

#include <script/console.h>
#include <script/bundle.h>

#include <core/raii.h>

#include <phys/solver/linear_time.h>
#include <gl/blend.h>

#include <sparse/linear.h> 
#include <sparse/linear/diagonal.h>
#include <sparse/linear/transpose.h>

#include <tool/fix.h>
macro_pthread_fix;

 
using namespace math;


namespace gui {

  static core::logger log;

  

  
  
  void graph::init() {
    
    setSceneRadius(30);
    showEntireScene();
    setAnimationPeriod( 0 );
    startAnimation();
    setMouseTracking(true);

    frame.resize(2);

    for(natural i = 0; i < frame.size(); ++i) {
      frame[i] = new gui::frame;

      constraint_type::attach at;
      at.key = new phys::constraint::bilateral(3);
      constraint.fixed[ frame[i] ] = at;
    }
    
    
    
    reset(15);

    // // Light setup
    // glDisable(GL_LIGHT0);
    // glEnable(GL_LIGHT1);

    // // Light default parameters
    // const GLfloat light_ambient[4]  = {1.0, 1.0, 1.0, 1.0};
    // const GLfloat light_specular[4] = {1.0, 1.0, 1.0, 1.0};
    // const GLfloat light_diffuse[4]  = {1.0, 1.0, 1.0, 1.0};

    // glLightf( GL_LIGHT1, GL_SPOT_EXPONENT, 3.0);
    // glLightf( GL_LIGHT1, GL_SPOT_CUTOFF,   10.0);
    // glLightf( GL_LIGHT1, GL_CONSTANT_ATTENUATION,  0.1f);
    // glLightf( GL_LIGHT1, GL_LINEAR_ATTENUATION,    0.03f);
    // glLightf( GL_LIGHT1, GL_QUADRATIC_ATTENUATION, 0.03f);
    // glLightfv(GL_LIGHT1, GL_AMBIENT,  light_ambient);
    // glLightfv(GL_LIGHT1, GL_SPECULAR, light_specular);
    // glLightfv(GL_LIGHT1, GL_DIFFUSE,  light_diffuse);
    
    core::link(&fix);

    dt = 0.01;
    
  }

  static math::real make_weight(const math::vec& lambda) { return -lambda.norm(); }


  static phys::solver::graph::weights_type weights(const phys::constraint::bilateral::vector& lambda) {

    phys::solver::graph::weights_type res;
    sparse::map(res, lambda, make_weight);
    
    return res;

  }


  template<class OnNeighbours>
  static phys::solver::graph::weights_type error_weights(const phys::system& system,
							 phys::dof::momentum f,
							 const OnNeighbours& on_neighbours) {
    using namespace phys;
    using namespace sparse::linear;
    
    // aliases
    const constraint::bilateral::matrix& J = system.constraint.bilateral.matrix;
    const constraint::bilateral::vector& b = system.constraint.bilateral.values;

    // response
    dof::velocity v = diag(system.resp) * f;
    
    // error
    constraint::bilateral::vector e = b - J * v;
    
    solver::graph::weights_type res = weights(e);
    
    // priority queue
    std::map<math::real, constraint::bilateral::key> queue;
    core::each(res, [&](constraint::bilateral::key k, math::real wk) {
	auto it = queue.find(wk);

	if(it == queue.end()) {
	  queue[wk] = k;
	} else {
	  core::log("HURRR");
	}
	
      });
    res.clear();
    
    while(!queue.empty()) {
      
      // pop tail O(1)
      auto c = queue.rbegin()->second;
      res[c] = queue.rbegin()->first;
      queue.erase( --queue.rbegin().base() );
      
      // solve constraint: JMinvJ^T lambda = e

      // assemble matrix O(log m)
      constraint::bilateral::matrix Jc, JcMinv;
      Jc[ c ] = J.find(c)->second; // O(log m)
      
      JcMinv = Jc * diag(system.resp); // O(log n)
      constraint::bilateral::quadratic K = JcMinv * transp(Jc); // O(1)
      
      assert(K.size() == 1);
      assert(K.begin()->second.size() == 1);
      
      auto Kinv = K.begin()->second.begin()->second.ldlt(); 

      // actual solve O(1)
      constraint::bilateral::vector lambda;
      lambda[c] = Kinv.solve( e.find(c)->second );
      
      // update force: unneeded O(log n)
      f += transp(Jc) * lambda;
      
      // update velocity O(log n)
      v += transp(JcMinv) * lambda;

      // update error/queue for neighbouring constraints
      on_neighbours(c, [&](constraint::bilateral::key nc) {
	  constraint::bilateral::matrix Jnc;
	  // O(log m)
	  Jnc[nc] = J[nc];
	  
	  // O(log m + log n)
	  auto& enc = e[nc];
	  
	  // O(log m)
	  queue.erase(make_weight(enc));

	  enc = b[nc] - (Jnc * v).begin()->second;
	  queue[make_weight(enc)] = nc;
	});
    }


    return res;
  }


 

  void graph::draw() {
    using namespace gl;

     // disable(GL_LIGHTING);
    // begin(GL_POINTS, [&] {
	
    // 	core::each(obj, [&](const obj_type& o) {
    // 	    vertex( o.coords );
    // 	  });
    //   });
    // enable(GL_LIGHTING);
    
    // Place light at camera position
    
    // const qglviewer::Vec cameraPos = camera()->position();
    // const GLfloat pos[4] = {cameraPos[0], cameraPos[1], cameraPos[2], 1.0};
    // glLightfv(GL_LIGHT1, GL_POSITION, pos);
    
    // // Orientate light along view direction
    // glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, camera()->viewDirection());
    
   
    line_width(4);
    disable(GL_LIGHTING );
    begin(GL_LINES, [&] {
	color( red() );
	core::each(boost::edges(constraint.mesh), [&](const mesh_type::edge_descriptor e) {
	    
	    if( constraint.mesh[e].acyclic) {
	      natural i = boost::source(e, constraint.mesh);
	      natural j = boost::target(e, constraint.mesh);

	      vertex(obj[i].conf, obj[j].conf);
	    }
	    
	  });

	color( yellow() );
	core::each(constraint.fixed, [&](gui::frame* f, const constraint_type::attach& a) {
	    vertex(obj[a.dof].conf, f->transform().translation());
	  });
      });
    

    color( white () );
    point_size(3);

    begin(GL_POINTS, [&] {
	core::each(obj, [&](const obj_type& o) {
	    vertex(o.conf);
	  });
      });
    
    enable(GL_LIGHTING );
    
    cull::enable();
    cull::face(GL_BACK);
    
    blend()([&]{
	color( transp(white ()) );
	begin(GL_TRIANGLES, [&] {
	    core::each( display.mesh.quads, [&](const geo::mesh::quad& q) {
		const vec3& a = obj[ q.vertex[0] ].conf;
		const vec3& b = obj[ q.vertex[1] ].conf;
		const vec3& c = obj[ q.vertex[2] ].conf;
		const vec3& d = obj[ q.vertex[3] ].conf;
		
		normal( (b - a).cross(c - a).normalized() );
		vertex( a, b, c);
		
		normal( -(b - a).cross(c - a).normalized() );
		vertex( a, c, b);
		
		normal( (c - a).cross(d - a).normalized() );
		vertex( a, c, d);
		
		normal( -(c - a).cross(d - a).normalized() );
		vertex( a, d, c);
	      });
	  });
	cull::disable();
      });
    
    // begin(GL_LINES, [&] {
	
    // 	core::each( boost::edges(constraint.mesh), [&](mesh_type::edge_descriptor e) {
    // 	    vertex( obj[boost::source(e, constraint.mesh)].conf,
    // 		    obj[boost::target(e, constraint.mesh)].conf );
    // 	  });
	
    //   });
    
   

    core::each(frame, [&](gui::frame* f) {
	f->draw();
      });
  }


  void graph::keyPressEvent(QKeyEvent* e) {
     switch(e->key()) {
     case Qt::Key_R: 
       
       on_animate = [&]{ this->reset(width); this->on_animate = 0 ; };
       
       break;
     default:
       QGLViewer::keyPressEvent(e);
     }
  }

  void graph::animate() {
    
    if( on_animate ) on_animate();

    // external forces
    phys::dof::momentum f;
    
    // gravity
    for(natural i = 0 ; i < obj.size(); ++i ) {
      f[ &obj[i] ] = - system.mass[ &obj[i] ] * vec3::UnitY();
    }

    // springs
    auto track_force = [&](natural i, gui::frame* g) {
      
      const real stiffness = 1500;
      
      f[&obj[i]] += stiffness * ( g->transform().translation() - obj[i].conf);

      system.mass[ &obj[i] ].diagonal() = vec3::Ones() + (dt * dt * stiffness) * vec3::Ones();
      system.resp[ &obj[i] ].diagonal() = system.mass[ &obj[i] ].diagonal().cwiseInverse();
    };


    // fixed extremities
    track_force(0, frame[0]);
    track_force(width - 1, frame[1] );
    
    // net impulse
    phys::dof::momentum net;
    
    for(natural i = 0 ; i < obj.size(); ++i ) {
      const phys::dof::key d = &obj[i];
      net[ d ] = system.mass [ d ] * obj[i].velocity + dt * f [ d ];
    }

    
    // constraints

    // graph update
    phys::constraint::bilateral::vector values;
    core::each(constraint.update, [&](phys::constraint::bilateral::key, 
      				      const core::callback& u) {
      		 u();
      	       });

    
    auto g = phys::solver::graph::make(system.constraint.bilateral.matrix,
				       weights(constraint.lambda));
				       // error_weights(system, net));
    solver->span(g);

    phys::solver::result res;
    res.lambda = &constraint.lambda;
  
    // velocity solve
    const phys::dof::velocity v = (*solver)( net, res );
    const phys::dof::momentum p = sparse::linear::diag(system.mass) * v;
    
    for(natural i = 0 ; i < obj.size(); ++i ) {
      const phys::dof::key d = &obj[i];

      auto chunk = v.find(d);
      
      if( chunk != v.end() ) {
    	obj[i].integrate(chunk->second, p.find(d)->second.transpose(), dt);
      }
    }
    
    
    // post-stabilization
    const bool correct = true;
    if( correct ) {
     
      phys::constraint::bilateral::vector backup = constraint.lambda;

      values = system.constraint.bilateral.values;
      system.constraint.bilateral.values = system.constraint.bilateral.corrections;

      const phys::dof::velocity c = (*solver)( phys::dof::momentum(), res );
    
      for(natural i = 0 ; i < obj.size(); ++i ) {
    	const phys::dof::key d = &obj[i];
      
    	auto chunk = c.find(d);

    	if( chunk != c.end() ) {
    	  // log("correction chunk:", chunk->second.transpose()); 
    	  obj[i].conf += chunk->second;
    	}
      }
      
      system.constraint.bilateral.values = values;
      constraint.lambda = backup; 
    }


    
    // clean lambda from dynamically allocated constraints (to avoid
    // unneeded size increase)
    core::each( system.constraint.bilateral.storage,
		[&](const std::unique_ptr< const phys::constraint::bilateral >& c) {
		  constraint.lambda.erase(c.get());
		});
    system.constraint.clear();
  }


  void graph::reset(math::natural n) {

    width = n;
    height = n;
    
    obj.resize( width * height);
    constraint.mesh = mesh_type( width * height );

    auto pos = [&](natural i) -> vec3& {
      return obj[i].conf;
    };

    auto vel = [&](natural i) -> vec3& {
      return obj[i].velocity;
    };

  

    auto edge = [&](real rest) -> edge_type {
      edge_type res;
      res.rest = rest;
      res.constraint = new phys::constraint::bilateral(1);
      return res;
    };

    // place vertices
    for(natural i = 0 ; i < n * n; ++i ) {
      const natural x = i / width;
      const natural y = i % width;
      
      pos(i) = (sceneRadius()/2) * vec3(y, 0, x) / width;
      vel(i).setZero();
    }

    // create edges
    for(natural i = 0 ; i < n * n; ++i ) {
      const natural x = i / width;
      const natural y = i % width;
      
      bool ok_height = (x < height - 1);
      bool ok_width = (y < width - 1);
      
      if( ok_height ) {
	real rest = (pos(i) - pos(i + width)).norm();
	
	boost::add_edge(i, i + width, edge(rest), constraint.mesh);
      }

      if( ok_width ) { 
	real rest = (pos(i) - pos(i + 1)).norm();
	boost::add_edge(i, i + 1, edge(rest), constraint.mesh);
      }
      
      if( ok_height && ok_width ) {
	// real rest = (pos(i) - pos(i + width + 1)).norm();
	// boost::add_edge(i, i + width + 1, edge(rest), constraint.mesh);

	// rest = (pos(i + 1) - pos(i + width)).norm();
	// boost::add_edge(i + 1, i + width, edge(rest), constraint.mesh);
      }

    }

    frame[0]->transform( SE3::translation( pos(0) ) );
    frame[1]->transform( SE3::translation( pos(width - 1) ) );

    system.clear();
    
    // masses
    core::each( obj, [&](const obj_type& o) {
	system.mass[ &o ].setIdentity(3, 3);
	system.resp[ &o ].setIdentity(3, 3);
      });
    
    constraint.lambda.clear();
    

    constraint.update.clear();

    // setup constraints
    core::each( boost::edges(constraint.mesh), [&](const mesh_type::edge_descriptor& e) {
	auto c = constraint.mesh[e].constraint;

	natural i = boost::source(e, constraint.mesh);
	natural j = boost::target(e, constraint.mesh);
	
	const real rest = constraint.mesh[e].rest;
	
	constraint.update[c] = [&,i,j,c,e,rest] {
	  
	  const vec3 diff = obj[i].conf - obj[j].conf; 
	  const vec3 n = diff.normalized();
	  
	  auto& row = system.constraint.bilateral.matrix[ c ];
      
	  row[ &obj[i] ] = n.transpose();
	  row[ &obj[j] ] = -n.transpose();
      
	  const real gamma = 1;
  
	  system.constraint.bilateral.values[ c ] = vec1::Zero(); 
	  system.constraint.bilateral.corrections[ c ] = gamma * vec1::Constant( (rest  - diff.norm()) );

	  if( solver ) {
	    constraint.mesh[e].acyclic = 
	      solver->acyclic().id.constraint.find(c) != solver->acyclic().id.constraint.end();
	  }	 
	  
	};
	
      });

    constraint.fixed[ frame[0] ].dof = 0;
    constraint.fixed[ frame[1] ].dof = width - 1;
    
    core::each(constraint.fixed, [&](gui::frame* f, const constraint_type::attach& a) {
	
	// constraint.update[a.key] = [&,a,f]{
	//   const vec3 diff = f->transform().translation() - obj[a.dof].conf;
	  
	//   // TODO only once 
	//   system.constraint.bilateral.matrix[a.key][&obj[a.dof].dof] = mat33::Identity();
	  
	//   system.constraint.bilateral.values[a.key] = vec3::Zero();
	//   system.constraint.bilateral.corrections[a.key] = diff;
	  
	// };
	
      });

    
    core::each(constraint.update, [&](phys::constraint::bilateral::key, 
				      const core::callback& u) {
		 u();
	       });
    
    // setup solver
    solver.reset( new solver_type(system) );
    

    // display mesh
    display.mesh.clear();
    
    for(natural i = 0 ; i < n * n; ++i ) {
      const natural x = i / width;
      const natural y = i % width;

      bool ok_height = (x < height - 1);
      bool ok_width = (y < width - 1);

      if( ok_height && ok_width ) {
	display.mesh.quads.push_back(geo::mesh::quad{i, i + 1, i + width + 1, i + width } );
      }
    };

   

  };
  
}


int main(int argc, char** argv) {
  QApplication app( argc, argv );

  QWidget* wid = new QWidget;
  QVBoxLayout* lay = new QVBoxLayout;
  
  gui::graph* viewer = new gui::graph;

  QObject::connect(&app, SIGNAL( lastWindowClosed(void)),
		   &app, SLOT( quit(void)));
  
  lay->addWidget(viewer);
  
  wid->setLayout( lay );
  wid->show();


  script::console console(viewer->script());
  console.start();
  
  
  script::bundle bob( viewer->script().lua(), "app" );
  bob
    .ref("width", viewer->width )
    .ref("dt", viewer->dt);
  
  return app.exec();
}
