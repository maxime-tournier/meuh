#include "control.h"

#include <core/share.h>
#include <gl/common.h>

#include <QFileDialog>
#include <gui/tweak.h>

#include <gl/quad.h>
#include <gl/frame.h>
#include <gl/blend.h>

#include <geo/obj.h>
#include <geo/obj/file.h>
#include <gui/viewer/text.h>

#include <tool/viewer.h>

#include <core/io.h>

namespace gui {
  namespace viewer {
    using tool::convert;

    
    static gl::texture chess, none;
   
    
    static QPixmap load_texture(const std::string& relative) {
      return QPixmap(core::share("/texture" + relative).c_str());
    }

  
    void control::init() {
       light.reset( new qglviewer::Camera );
       gui::fancy::init(this, light.get());
       
       // gl::skeleton::init();

      chess.gen( bindTexture(load_texture("/chess-light.png"), chess.target()) ).nearest().unbind();
      none.gen( bindTexture(load_texture("/none.png"), none.target()) ).nearest().unbind();

      gl::light::model::ambient( 0.35 * gl::white());
      
      // FIXME MEMLEAK
      auto geo = new gui::tweaker<unsigned int>(ngeo);
      geo->slot.action = [this](unsigned int n) { this->reset_object(n); };
      
     
      // TODO connect 

      animation
	.add("Playback", _time.playbar.get() )
	.add("Tracking",  [this] {
	    
	    const QString filename = QFileDialog::getOpenFileName(this,
								  tr("Open Capture"), 
								  QDir::homePath() + "/bvh", 
								  tr("BVH Files (*.bvh)"));
	    if(!filename.isNull()) this->track(filename);
	   
	    
	  })

	.add("Learning", [this] {
	    
	    const QString filename = QFileDialog::getOpenFileName(this,
								  tr("Open Capture"), 
								  QDir::homePath() + "/bvh", 
								  tr("BVH Files (*.bvh)"));
	    if(!filename.isNull()) this->learn(filename);
	   
	    
	  })
	.add("Ground", gui::tweak(ground.equation(3), 0.25)->widget() )
	.add("Keyframe Speed", gui::tweak(keyframe_speed, 0.25)->widget() )
	;
      
      controller
	.add("Proportional", gui::tweak(tracker.base_p, 10.0)->widget() )
	.add("Derivative", gui::tweak(tracker.base_d, 0.1)->widget() )
	.add("Balance", gui::tweak(gamma, gamma)->widget() )
	.add("AM", gui::tweak(weight.am, weight.am)->widget() )
	.add("Reference", gui::tweak(reference, reference)->widget() )
	// .add("Keep Contact", gui::tweak(this->keep_contact, 0.0)->widget() )
	.add("Strength", gui::tweak(strength, strength)->widget() )
	.add("Laziness", gui::tweak(laziness, laziness )->widget() )
	// .add("Reflexes", gui::tweak(this->reflexes, 1.0)->widget() )
	;
      
      params
	.add("Geodesic Count", geo->widget() )
	.add("Unactuated", gui::tweak(unactuated, unactuated)->widget() )
	.add("Bretelles", gui::tweak(flags.bretelles, flags.bretelles)->widget() )
	// .add("Relax", gui::tweak(flags.relax, false)->widget() )
	.add("Approx", gui::tweak(phys::approx::level, phys::approx::level)->widget() )
	.add("Damping Model", gui::tweak(tool::character::damping::kind, tool::character::damping::kind)->widget() )
	// .add("Control", gui::tweak(flags.control, true)->widget() )
	
	.add("Inertia Forces", gui::tweak(flags.inertia, flags.inertia)->widget() )
	.add("Angular Limits", gui::tweak(flags.enforce_limits, flags.enforce_limits)->widget() )
	.add("Contacts", gui::tweak(flags.contacts, flags.contacts)->widget() )
	.add("Travelling", gui::tweak(flags.travelling, flags.travelling)->widget() )
	.add("Gravity", gui::tweak(flags.gravity, flags.gravity)->widget() )
	.add("Time Step", gui::tweak(dt, dt)->widget( ) )
	.add("Momentum", gui::tweak(flags.momentum, true)->widget() )

	.add("Single Frame", [this] { this->animate(); this->updateGL(); } )
	.add("Write Kinetic", [this] { this->write_kinetic(); } )
	.add("Write Angular", [this] { this->write_angular(); } )
	.add("Write Linear", [this] { this->write_linear(); } )
	;

      flags.show_hints = false;

      on_mouse_press[ Qt::LeftButton ] [ Qt::ShiftModifier ] = [&](QMouseEvent* e) {
	selection_mode = normal;
	this->select(e);
      };

      on_mouse_press[ Qt::LeftButton ] [ Qt::ControlModifier ] = [&](QMouseEvent* e) {
	selection_mode = shoot;
	this->select(e);
      };
      
      on_mouse_press[ Qt::RightButton ] [ Qt::ShiftModifier ] = [&](QMouseEvent* e) {
	if(!this->mouseGrabber()) {
	  selection_mode = menu;
	  this->select(e);
	}
      };


      character_menu.title = "Character";
      
      character_menu.actions["Set as &Rest Pose"] = [&] {
	this->obj->save_rest();
      };

      character_menu.actions["Set as &Init Pose"] = [&] {
	this->obj->init = this->obj->conf;
      };

      character_menu.actions["&Damping..."] = [&] {
	this->popup( gui::tweak(this->obj->damping, this->obj->damping)->widget(), "Damping");
      };

      character_menu.actions["&Stiffness..."] = [&] {
	this->popup(gui::tweak(this->obj->stiffness, this->obj->stiffness)->widget(), "Stiffness");
      };
      
      character_menu.actions["&Look at"] = [&] { this->lookat(); };
      
      character_menu.actions["&CoM Control"] = [&] { this->add_com(); };

      character_menu.actions["&Feet Control"] = [&] { 
	this->add_handle( this->tracking->skeleton().find("lfoot"), math::vec3::Zero());
	this->add_handle( this->tracking->skeleton().find("rfoot"), math::vec3::Zero());
      };

      character_menu.actions["&Walk..."] = [&] { this->walk(); };
      
      toggle( character_menu.actions, "Push Mode", "IK Mode" );

      character_menu.actions[ core::hook("Push Mode") ] =[&] { 
	this->flags.push_mode = true;

	character_menu.actions["Push Strength..."] = [&] {
	  this->popup(gui::tweak(push.stiffness, push.stiffness)->widget(), "Push Strength" );
	};

      };
      
      character_menu.actions[ core::hook("IK Mode") ] = [&] {
	this->flags.push_mode = false;
	character_menu.actions.erase("Push Strength...");
      };
      

      
      setAddKeyFrameKeyboardModifiers(Qt::MetaModifier);
      // com_add();

      // setAnimationPeriod( dt / phys::unit::ms );
      setAnimationPeriod( 0 );

      // startAnimation();
      setMouseTracking(true);

      // larger picking window
      setSelectRegionWidth(8);
      setSelectRegionHeight(8);


    }



     void control::draw() {

      for(int i = 0; i < 12; ++i) {
	auto bob = camera()->keyFrameInterpolator(i);
	if( bob ) bob->setInterpolationPeriod(5);
      }

      using namespace gl;
      const math::natural size = 1 << 15;
       
      auto plane = [&] {
	// quad();
	chess.bind();
	matrix::mode(GL_TEXTURE);
	matrix::push();
	scale(size >> 4);
	quad();
	matrix::pop();
	matrix::mode(GL_MODELVIEW);
      };

      auto placed_plane = [&, this] {
	gl::matrix::push();
	using namespace math;
	quat q;
	translate( this->ground.origin() );
	q.setFromTwoVectors(vec3::UnitZ(), this->ground.normal());
	rotate(q);
	scale( size );
	plane();
	gl::matrix::pop();
      };


      auto draw_handles = [&] {
	gl::color( gl::white() );
	
	// frame size
	math::natural s = 5;
	
	if(com.handle) com.handle->draw(s);

	// tracker.each([&](const tracker_type::base_type& h ) {
	//     h.frame->draw(s);
	//   });
	core::each(frames, [&](gui::frame* h) {
	    h->draw(s);
	  });

	if( push.active() ) {
	  const math::vec3 point = pose(push.dof)(push.local);
	  const math::vec3 start = point - 5 * push.dir;
	  
	  gl::color( gl::red() );
	  
	  // this->drawArrow(convert(hat), convert(to));
	  this->drawArrow(convert(start), convert(point));
	}
      };


      


      auto content = [&] {
	// this->none.bind();
	
	chess.bind();
	color( html( 0xaaff77) ); placed_plane(); 
	chess.unbind();

	color( white() );
	none.bind();
	// skeleton( skeleton::full ).draw_center( tracking->skeleton.topology, pose );

	// pose.each([&](math::natural i) {
	//     gl::frame{pose(i)}([&]() {
	// 	QGLViewer::drawAxis();
	//       });
	//   });
	
	// color( transp( white()));
	// blend()( [&]() { 
	if( tracking ) models["character"].visual.draw( pose );
	// });

	if( box ) { box->draw(); }

	none.unbind();

      };
      
      if( tracking ) {
      	light->lookAt( convert( pose( tracking->skeleton().root ).translation() ) );
      }
      
      
      auto hints = [&] {
	if( collisions )  { 
	
	  collisions();
	
	}

      };
      
      auto momenta = [&]{ 
	for(auto it = this->to_draw.rbegin(), end = this->to_draw.rend(); it != end; ++it) {
	  (*it)();
	}
	// core::each(this->to_draw, [](const core::callback& f) {
	//     f();
	//   });
      };

      auto edition =[this] {
	disable(GL_LIGHTING);
	blend() ( [&] {
	    line_width(3.0);
	    color( transp( orange(), 0.8) );
	    
	    if( edited != trajectories.end() ) {
	      edited->draw();
	    }
	    

	  });
	enable(GL_LIGHTING);
      };

      auto misc = [&] {
	if( this->flags.show_hints ) hints();
	if( this->flags.show_momenta ) momenta();
	draw_handles();
	edition();
      };

      gui::fancy::cast( camera(), light.get(), [&] { content(); misc(); } );
      gui::fancy::draw( camera(), light.get(), content );
      
      misc();
      
      script().lua().string("hook(draw)");
     }
    

    
    void control::setup_models() {
      models.clear();
      
      // 1 ground
      models[ "ground" ].collision.primitives[0] = new geo::collision::plane(ground);
      
      // 2 character
      assert( tracking );
      
      std::map<std::string, math::real> factor;
      
      factor[ "head end" ] = 1.4;
      factor[ "head" ] = 1.4;

      factor[ "upperneck" ] = 1.4;
      factor[ "lowerneck" ] = 1.4;
      
      factor[ "lhand" ] = 1.5;
      factor[ "rhand" ] = 1.5;

      factor[ "lfoot" ] = 2.4;
      factor[ "rfoot" ] = 2.4;

      // factor[ "ltoes" ] = 1;
      // factor[ "ltoes" ] = 1;
      
      auto fact = [&](std::string n) {
	auto i = factor.find(n);
	if( i != factor.end() ) return i->second;
	return 1.0;
      };

      std::set< std::string > blacklist;
      
      blacklist.insert("upperneck");
      // blacklist.insert("upperback");
      
      
      tracking->skeleton().each( [&](math::natural index) {
	  const phys::rigid::body& b = tracking->skeleton().body(index);
	  
	  if( blacklist.find(b.name) == blacklist.end() ) {
	    try {
	      auto mesh = adjust(b, geo::obj::mesh( core::share("/mesh/" + b.name + ".obj" ) ),
				 fact(b.name) );
	      models["character"].geometry.meshes[index] = new geo::mesh( mesh );
	      models["character"].visual = models["character"].geometry.visual();
	      models["character"].collision = models["character"].geometry.collision();
	    } 
	    catch( const core::io::not_found& e) {
	      // model.add(index, adjust(b, geo::obj::mesh( core::share("/mesh/box.obj" ) ))) ;
	    }
	  }
	});
      
      models["character"].collision.update = [&] {
	
	core::each(models["character"].collision.primitives, [&](math::natural i, geo::collision::primitive* p) {
	    
	    auto vert = static_cast<geo::collision::vertices*>(p);
	    const auto& local = models["character"].geometry.meshes.find(i)->second->vertices;
	    
	    local.each([&](math::natural v) {
		vert->data(v) = pose(i)( local(v) );
	      });
	  });
      };

      core::each(models["character"].visual.meshes, [&](math::natural i, gl::mesh* m) {
	  
	  tool::pickable p;

	  p.draw = [&,i,m]{ gl::frame{ pose(i) }( [m]{ m->draw(); } ); };
	  p.pick = [&,i](const math::vec3& at) {
	    
	    // local coords
	    const math::vec3 local = pose(i).inverse() ( at );
	    
	    if(flags.push_mode) {
	      this->apply_push(i, local);
	    } else {
	      add_handle(i, local);
	    }
	  };

	  picker.add( p );
	  
	});

      
    };


  




  }
}
