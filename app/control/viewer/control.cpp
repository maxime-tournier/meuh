#include <core/stl.h>
#include "control.h"


// bli 
#include <gl/skeleton.h>
#include <mocap/curves.h>
#include <gui/viewer/text.h>

#include <math/vec.h>
#include <math/lie/pga.h>

#include <math/func/SE3.h>

#include <core/macro/here.h>
#include <core/macro/debug.h>

#include <gl/common.h>
#include <gl/line.h>
#include <math/func/coord.h>

#include <QMouseEvent>
#include <math/se3.h>

#include <core/io.h>
#include <core/resource.h>
#include <math/func/numerical.h>
#include <gl/quad.h>

#include <phys/dof.h>

#include <phys/constraint/bilateral.h>

#include <math/tuple.h>
#include <math/func/coord.h>
#include <math/func/vector.h>

#include <phys/constant.h>

#include <QMutexLocker>
#include <QCoreApplication>

#include <phys/rigid/com.h>
#include <gui/tweak.h>

#include <QFileDialog>

#include <control/solver/sqp.h>
#include <control/solver/ours.h>

#include <sparse/linear/diagonal.h>
#include <gl/blend.h>

#include <core/share.h>
#include <core/io.h>

#include <geo/obj.h>
#include <geo/obj/file.h>
#include <geo/mesh.h>

#include <math/func/euclid.h>

#include <math/kinematics.h>
#include <core/log.h>

#include <list>
#include <core/unique.h>

#include <math/mosek.h>
#include <gl/frame.h>

#include <gui/playbar.h>
#include <math/func/index.h>
#include <phys/rigid/relative.h>

#include <tool/contacts.h>
#include <tool/picker.h>

#include <phys/solver/qp.h>
#include <math/covector.h>

#include <tool/track.h>

#include <QApplication>
#include <core/meuh.h>
#include <core/string.h>

#include <gui/viewer/text.h>
#include <core/raii.h>
#include <math/func/ref.h>


#include <tool/viewer.h>


namespace gui {

  namespace viewer {
    
    using core::log;
    using tool::convert;
 
    
    math::real control::time() const { return _time.control->time()->get(); }
    
    void control::init_script() {
      // access variables
      app
	.ref("time", _time.control->time()->get())
	.ref("dt", dt)
	.ref("control", flags.control )
	.ref("bretelles", flags.bretelles )
	.ref("gravity", flags.gravity )
	.ref("contacts", flags.contacts )
	.ref("momentum", flags.momentum )
	.ref("show_momenta", flags.show_momenta )
	.ref("nappe", flags.nappe )
	.ref("trace", flags.trace )
	.ref("travelling", flags.travelling )
	.ref("laziness", laziness )
	.ref("friction", flags.friction )
	.ref("limits", flags.enforce_limits )
	.ref("reference", reference )
	.ref("unactuated", unactuated )
	.ref("push_stiff", push.stiffness)
	.ref("approx", phys::approx::level)
	.ref("damp_model", tool::character::damping::kind)
	.ref("fixed", flags.fixed)
	.ref("euler", flags.euler)
	.ref("inertia", flags.inertia)
	.ref("com_height", weight.com_height) // desired
	.ref("balance", gamma)
	.ref("correct", flags.correct)
	.ref("speed", keyframe_speed)
	.ref("tracking", flags.tracking )
	.ref("mu", contacts.mu )
	.ref("am", weight.am )
	.ref("strength", strength)
	;
      
      // access functions
      app
	.fun("restart", [&]() { this->reset_needed = true; } )
	.fun("animate", [&](bool value) { if(value) { this->startAnimation(); } else {this->stopAnimation(); } } )
	.fun("quit", [&]() { QApplication::quit(); } )
	.fun("debug", [&](bool value) { core::log().enabled = value;  } )
	.fun("log", [&](const std::string& value) { core::log(value);  } )
	.fun("selected", [&]() -> std::string  { return this->selected;  } )
	.fun("ccfk", [&] { this->obj->pga.kind = tool::pga::first; } )
	.fun("ccsk", [&] { this->obj->pga.kind = tool::pga::second; } )
	.fun("set_geo", [&](math::natural n) {  if(!n) return ; this->ngeo = n; this->reset_object(n); } )
	.fun("get_geo", [&] {  return this->obj->pga.n; })
	.fun("meuh", [] { return core::meuh::root(); } )
	.fun("gui", [&](bool value) { 
	    animation.show( value );
	    controller.show( value );
	    params.show( value );
	  } )
	.fun("coord", [&](math::natural i) {
	    return script::ref(std::get<2>(obj->conf)(i) );
	  })
	.fun("damping", [&] {
	    return script::ref( this->obj->damping );
	  })
	.fun("stiffness", [&] {
	    return script::ref( this->obj->stiffness );
	  })
	.fun("length", [&](double d) { this->_time.control->end( this->_time.control->start() + d ); } )
	.fun("walk", [&] {emit walk_needed(); } )
	
	.fun("push", [&]( math::natural dof, bool random ) { this->apply_push( dof, math::vec3::Zero(), random ); } )
	.fun("dof", [&](const std::string& string) -> math::natural{ return this->tracking->skeleton().find(string); } )
	.fun("save", [&] { this->obj->init = this->obj->conf; } )
	.fun("load", [&](const std::string& filename) { 
	    emit load_needed(filename.c_str());
	    // tool::viewer::push([this,filename]{
	    // 	load(filename.c_str());
	    // 	core::log("finished loading");
	    //   });
	  } )
	.fun("height", [&]() -> math::real { return phys::rigid::com(this->tracking->skeleton())(pose).y(); })
	.fun("filename", [this] { return tracking->filename; } )
	
	.fun("handle", [this](int dof, math::real weight) { 

	    auto h = dof >=0 ? add_handle(dof, math::vec3::Zero()) : add_com();
	    
	    if( weight >= 0 ) {
	      h.settings->soft = true;
	      h.settings->weight = weight;
	    }
	  })
	
	.fun("track", [this](int dof, math::real weight) { 

	    auto h = dof >=0 ? add_handle(dof, math::vec3::Zero()) : add_com();
	    h.target->track = true; 
	    
	    if( weight >= 0 ) {
	      h.settings->soft = true;
	      h.settings->weight = weight;
	    }
	  })
	.fun("playback", [&](math::real t) { playback(t); })
	.fun("incline", [&](math::real alpha) {
	    using namespace math;
	    vec3 n = std::cos(alpha) * vec3::UnitY() + std::sin(alpha) * vec3::UnitZ();
	    ground = geo::plane( ground.origin(), n);
	  })
	.fun("show_limit", [&](math::natural i ) {
	    tool::viewer::push([this, i] {
		if( i == 0 ) limits_viewer->hide();
		else {
		  limits_viewer->index = i;
		  limits_viewer->show();
		}
	      });
	    
	  })
	.fun("cam", [&](math::natural i) {
	    camera()->playPath(i);
	  })
	.fun("has_contacts", [&]() -> bool { return contacts.center.get(); })
	;
      
      engine.exec("require 'control'");
    }

    
    
    control::control(QWidget* parent) 
      : tool::viewer(parent),
	ground(math::vec3(0, 0, 0), math::vec3::UnitY() ),
	parent(parent),
	animation("Animation"),
	controller("Control"),
	params("Solver"),
	app(engine.lua(), "control")	
    { 
      ngeo = 12;

      reset_needed = false;

      random_angle = math::random<math::real>::uniform(0, 2 * math::pi);

      _time.control = core::unique<tool::anim::control>();
      _time.playbar = core::unique<gui::playbar>(_time.control.get());

      flags.push_mode = false;
      push.stiffness = 200;
      
      connect( this, SIGNAL( walk_needed() ),
	       this, SLOT(walk()));

      connect( this, SIGNAL( load_needed( const QString& ) ),
	       this, SLOT( load( const QString& )));

      flags.show_momenta = false;
      flags.fixed = false;
      flags.control = true;
      flags.correct = false;
      flags.friction = true;
      flags.nappe = false;
      flags.trace = false;
      
      stance = 0.5;

      unactuated = 0;

      flags.bretelles = false;
      // flags.relax = false;
      weight.am = 1.0;
      gamma = 1.0;
      
      strength = 1500;
      laziness = 0.1;

      reference = 0.01;
      flags.inertia = true;
      flags.enforce_limits = true;
      flags.contacts = true;
      flags.gravity = true;
      flags.travelling = false;
      flags.tracking = false;
      dt = 5e-2;
      
      weight.com_height = 15;
      flags.euler = false;
      edited = trajectories.end();
      contacts.mu = 1.0;
    }

 

  

    void control::com_del() { 
      if(!com.handle) return;
      
      com.handle->removeFromMouseGrabberPool();
      
      com.handle = 0;
      com.automatic = false;
    }



    
    void control::popup(QWidget* wid, const QString& name) {
      wid->setWindowTitle( name );
      wid->move( QCursor::pos() );
      wid->show();
    }

   

    void control::make_ball() {
      if( box ) return;

      // ball setup
      box.reset( new tool::rigid );
      box->mesh( geo::obj::mesh( core::share("/mesh/sphere.obj" ) ) );
      
      auto p  = new geo::collision::sphere;
      p->geo.radius = 1.0;
      
      box->collision.primitive.reset( p );
      
      box->update = [&, p] { p->geo.center = box->frame().translation(); };

      box->body.inertia = math::vec3::Ones();
      box->body.mass = 400 * phys::unit::g;
      box->frame( math::SE3::translation( 5 * math::vec3::UnitY() ) );
      
      box->update();
    }

    void control::drawWithNames() {
      // picker.pickables.clear();

      picker.draw();
      
      
      // if(! tracking ) return;

      // models["character"].visual.draw( pose ); 
      
    }


    // void control::update_target(int x, int y) {
  
    // }
  
    
    void control::shoot_ball(const math::vec3& from, const math::vec3& to, math::real vel) {
      QMutexLocker lock(&mut_reset);
      if(!box) make_ball();
      
      using namespace math;
      SE3 tmp = SE3::translation(from);
      box->frame(tmp);
      // linear(box->velocity) = tmp.rotation().inverse( vel * (to - from) );
      std::get<1>(box->momentum) = box->body.mass * vel * (to - from);
    }

    void control::postSelection(const QPoint& point) {
        
      using namespace math;
      
      // find the selectedPoint coordinates, using camera()->pointUnderPixel().
      bool found;
      qglviewer::Vec selectedPoint = camera()->pointUnderPixel(point, found);
      core::log()("selection mode:", selection_mode);
	
      if( found && selectedName() >=0 ) {
	// local selection conf
	const real dist = camera()->projectedCoordinatesOf(selectedPoint).z;
	    
	// z-buffer entry
	const qglviewer::Vec loc(point.x(), point.y(), dist);
	    
	// unproject
	const qglviewer::Vec target = camera()->unprojectedCoordinatesOf(loc);
	    
	const math::vec3 abs = convert(target);

	switch( selection_mode ) {
	case normal: {
	  picker.at( selectedName() ).pick(abs);
	} break;
	    
	case menu: {
	  character_menu.popup(mapToGlobal(point));
	} break;
	    
	case shoot:  {
	  shoot_ball(convert(camera()->position()), abs, 2.0);
	} break;
	};
	  
      }
      
    }


  

    bool control::handle_mouse_event(const mouse_events& events, QMouseEvent* e ) {
      auto but = events.find(e->button());
      if( but != events.end() ) {

	auto mod = but->second.find( e->modifiers() );
	if( mod != but->second.end() ) {
	  mod->second(e);
	  return true;
	}
      } 
      
      
      return false;
    }


    void control::mouseReleaseEvent(QMouseEvent *e)
    {
      if (!handle_mouse_event(on_mouse_release, e) ) QGLViewer::mouseReleaseEvent(e);
    }
  
    void control::mousePressEvent(QMouseEvent *e)
    {
      if (!handle_mouse_event(on_mouse_press, e) ) QGLViewer::mousePressEvent(e);
    }
    

    
    void control::keyPressEvent(QKeyEvent* e)
    {
      switch(e->key()) {
      case Qt::Key_R: reset_needed = true; break;
      case Qt::Key_H: flags.show_hints = !flags.show_hints; break;
      default:
	engine.exec(std::string("keypress([[") + e->text().toStdString() + "]])");
	QGLViewer::keyPressEvent(e);
      };

    }


    void clamp(math::vec& data, const math::vec& bounds) {
      data.each([&](math::natural i) {
	  if( data(i) > bounds(i) ) { data(i) = bounds(i); }
	  if( data(i) <  -bounds(i) ) {  data(i) = -bounds(i); }

	});
    }


   


  



    control::com_type::com_type() : handle(0), weight(0), orient_control(false),
				    pos(3), orient(3) { }
    
    
 

  
 

   

 

  
 


    void control::apply_push(math::natural i, const math::vec3& local, bool random) {
      push.dof = i;
      push.local = local;
	      
      const math::real alpha = random_angle();
	      
      if(random) {
	push.dir = math::vec3(std::sin(alpha), 0, std::cos(alpha));
      } else {
	push.dir = math::vec3::UnitZ();
      }
      
      // push.go( 0.1 * phys::unit::s );
      // push.frames( 0.5 / dt * phys::unit::s);
      push.frames(1);
    }


  

  

    void control::write_kinetic() {
      std::ofstream out;
      std::string filename;

      switch(phys::approx::level) {
      case phys::approx::good: filename = "good.dat"; break;
      case phys::approx::bad: filename = "bad.dat"; break;
      case phys::approx::ugly: filename = "ugly.dat"; break;
      }
      
      std::string dir = "/tmp";

      out.open(dir + "/" + filename);

      assert( out );
      
      core::each( kinetic, [&](const std::pair<math::real, math::real>& p) {
	  out << p.first << ' ' << p.second << '\n';
	});
      
      out.close();
    }


    void control::write_angular() {
      std::ofstream out;
      std::string filename;

      switch(phys::approx::level) {
      case phys::approx::good: filename = "good.dat"; break;
      case phys::approx::bad: filename = "bad.dat"; break;
      case phys::approx::ugly: filename = "ugly.dat"; break;
      }
      
      std::string dir = "/tmp";

      out.open(dir + "/angular." + filename);

      assert( out );
      
      core::each( angular, [&](const std::pair<math::real, math::vec3>& p) {
	  out << p.second.transpose() << '\n';
	});
      
      out.close();
    }


    void control::write_linear() {
      std::ofstream out;
      std::string filename;

      switch(phys::approx::level) {
      case phys::approx::good: filename = "good.dat"; break;
      case phys::approx::bad: filename = "bad.dat"; break;
      case phys::approx::ugly: filename = "ugly.dat"; break;
      }
      
      std::string dir = "/tmp";

      out.open(dir + "/linear." + filename);

      assert( out );
      
      core::each( linear, [&](const std::pair<math::real, math::vec3>& p) {
	  out << p.second.transpose() << '\n';
	});
      
      out.close();
    }




   

  
    // void control::reset_object(natural n)  {
      
    // }


    // TODO this should be static right ?
    geo::mesh control::adjust(const phys::rigid::body& b, const geo::mesh& m, math::real factor) const {
      geo::mesh res = m;
      
      const geo::aabb aabb = m.aabb();
      const math::real length = aabb.dim().y();
      const math::real target = factor * b.dim.y();
      
      res.vertices.each( [&](math::natural i) {
	  res.vertices(i) *= target/length;
	});
      
      return res;
    }


    void control::start_pose() {

      object_type::conf_type at;
      
      // auto start = tracking->relative()( mocap::start( tracking->clip.data ) );
      
      using namespace math;
      // std::get<1>(at) = func::vec( func::rotation<>() )( start.tail(start.rows() - 1) );
      
      obj->reset( obj->init );

      pose = tool::character::absolute(*obj)( obj->conf );
      
      _time.control->restart();
      
       
      kinetic.clear();
      angular.clear();
      linear.clear();
      to_draw.clear();

      reset_needed = false;
      
      vec3 target = phys::rigid::com(tracking->skeleton())(pose);
      vec3 offset =  vec3(-50, 30, 50);
      camera()->setPosition( tool::convert(target + offset ));
      camera()->lookAt( tool::convert(target) );
      
      engine.exec("hook(restart)");
    }
    

    void control::reset() { 
      
      if( !tracking ) return;
      
      core::log("reset !");

      com_del();

      start_pose();
      
      camera()->setRevolveAroundPoint( convert( phys::rigid::com(obj->skeleton)(pose) ) );
      
      reset_needed = false;
      
    }




    static std::vector<math::natural> to_remove(const tool::mocap& pack) {
      std::vector<math::natural> res;
      
      std::vector<std::string> names = { // "rtoes", "ltoes", 
					 "lfingers", "rfingers",
 					 "lthumb", "rthumb",
					 "lhand", "rhand" };
      
      for(math::natural i = 0; i < names.size(); ++i) {
	const math::natural p = pack.skeleton().find( names[i] );
	res.push_back( p );
      }
      
      return res;
    }


    control::~control() { }


    static math::vector<math::vec3> to_euler(const math::vector<math::SO3>& g) {
      math::vector<math::vec3>res;
      res.resize(g.rows());

      res.each([&](math::natural  i) {
	  res(i) = g(i).zyx();
	});

      return res;
    }

    math::natural control::learn(const tool::mocap& pack) {
      
      auto idx = tracking_map(pack);
  
      using namespace math;
      
      using func::operator<<;
      auto map = func::vector< func::rotation<> >() << phys::rigid::relative{tracking->skeleton()}
						 << math::func::index::map< math::SE3 >{idx} << pack.absolute();

      auto data = pack.clip.sampling.sample( map );

      auto root = tracking->skeleton().root;
      data.each([&](natural i) { 
	  if( i == 0 ) return;
	  
	  core::each( to_remove(pack), [&](natural j) {
	      data(i)(j) = data(0)(j);
	    });
	  
	  // remove root variations
	  data(i)(root) = data(0)(root);
	});
      natural pga_modes =  pga.compute(data, algo::stop().eps(1e-9), lie::of( data(0) ), 0.95 );

      // euler pca
      vector< vector <vec3> > euler;
      euler.resize( tracking->clip.sampling.frames );
      
      SO3 off = SO3::identity();

      bool apply_offset = true;
      if( apply_offset ){
	off = Lie<SO3>().exp()( 0.8 * pi * vec3::UnitZ() );
      }
      
      euler_offsets = vector<SO3>::Constant(data(0).rows(), off);
      euler_offsets(root) = SO3::identity();
      
      euler.each([&](natural i) {
	  euler(i) = to_euler(lie::prod( euler_offsets, data(i)) );
	});      
      
      natural euler_modes = euler_pca.compute(euler, algo::stop().eps(1e-9), lie::of( euler(0) ), 0.95 );
      core::log("pga vs euler:", pga_modes, " / ", euler_modes );

      return pga_modes;
    }



    std::map<math::natural, math::natural> control::tracking_map(const tool::mocap& data ) const {
      // name map
      std::map< std::string, math::natural > tr, lrn;
      
      tracking->skeleton().each([&](math::natural i) {
	  tr[ tracking->skeleton().topology[i].name ] = i;
	});

      data.skeleton().each([&](math::natural i) {
	  lrn[ data.skeleton().topology[i].name ] = i;
	});


      // map: learning to tracking
      std::map<math::natural, math::natural> res;
      
      // TODO make this robust
      core::each(lrn, [&](const std::string& name, math::natural i) {
	  res[i] = tr[ name ];
	});

      return res;
    }
    
    
    void control::setup_limits() {
      assert(learning);
      assert(tracking);
      
      auto map = tracking_map(*learning);

      using namespace math::func;
      using math::func::operator<<;
      
      auto data = learning->clip.sampling.sample(  math::func::vector< math::func::rotation<> >() 
						   << phys::rigid::relative{tracking->skeleton()} 
						   << index::map< math::SE3>{map}  << learning->absolute() );
      auto lim = tool::angular_limits{math::algo::stop().it(5), math::algo::stop().it(5), 0 }(data, tracking->skeleton().root);
      
      limits.data = core::unique<tool::angular_limits::result_type>(lim);
      
      // limits_viewer.reset( new tool::limits_viewer );
      // limits_viewer->limits = *limits.data;
      // limits_viewer->data = data;
    }

     // // acquire euler bounds
      // math::vector<math::vec3> low = tracking->clip.angles(0);
      // math::vector<math::vec3> up = tracking->clip.angles(0);
      
      // tracking->clip.angles.each([&](math::natural i) {
      // 	  tracking->clip.angles(i).each([&](math::natural j) {
	      
      // 	      for(math::natural k = 0; k < 3; ++k) {
      // 		math::real alpha = tracking->clip.angles(i)(j)(k);
      // 		if(alpha  < low(j)(k) ) low(j)(k) = alpha;
      // 		if(alpha  > up(j)(k) ) up(j)(k) = alpha;
      // 	      }
      // 	    });
      // 	});
      
      // math::vector< math::vector<math::SO3> > random;
      // random.resize(data.rows());

      // random.each();


    void control::track(const QString& filename ) {
      tracking.reset( new tool::mocap(filename.toStdString()) ); 
      
      _time.control->start( mocap::start(tracking->clip.data) );
      _time.control->end( mocap::end(tracking->clip.data) );

      ngeo = learn(filename);
      reset_object(ngeo);
      
      setup_models();
    }


    math::natural control::learn(const QString& filename ) {
      assert(tracking);
      
      learning.reset( new tool::mocap(filename.toStdString()) ); 
      
      setup_limits();

      return learn(*learning);

    }


    void control::load(const QString& filename) {
      stopAnimation();
      obj.reset( 0 );
      track(filename);
      reset();
      
      engine.exec("hook(load)");
      
      // walk();
    }
    

    void control::reset_tracker() {
      assert( obj );
      
      tracker.context = core::unique< tool::track::context<object_type::conf_type> >( obj.get(), obj->lie );
      tracker.context->pose = tool::character::absolute(*obj);
      
    } 


    void control::reset_object(math::natural n ) {
      QMutexLocker lock(&mut_reset);
      
      core::log("reset object:", n);

      if(!obj) {
	obj = core::unique<object_type>(tracking->skeleton(), pga, n);

	auto start = tracking->relative()( mocap::start( tracking->clip.data ) );

	std::get<0>(obj->init) = start ( tracking->skeleton().root ).rotation();
	std::get<1>(obj->init) = start ( tracking->skeleton().root ).translation();

      } else {
	
	obj = obj->copy( n );
      }

      reset_tracker();

      
      
    }


    void control::playback(math::real t) {
      pose = tracking->absolute()( t );
      obj->reset(obj->init);
    }
    
  }
 
}
