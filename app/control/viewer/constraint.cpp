#include "control.h"

#include <math/func/euclid.h> 
#include <math/func/numerical.h>
#include <math/func/ref.h>

#include <gl/common.h>
#include <gui/tweak.h>

#include <tool/contacts.h>

namespace gui {
  namespace viewer {


    void toggle(core::actions& actions, const std::string& on, const std::string& off) {
      using core::hook;

      actions[ hook(on) ] = core::noop();
      actions[ hook(off) ] = core::noop();
      
      actions[ on ] = [&actions, on, off] {

	actions[ hook(on) ]();
	
	auto self = actions[ on ];
	actions[ off ] = [&actions, self, on, off] {

	  actions[ hook(off) ]();
	  
	  actions[ on ] = self;
	  actions.erase( off );
	};

	actions.erase( on );
      };
      
    };

    static void setup_base(core::actions& actions) {

      toggle(actions, "Track", "Untrack" );
      toggle(actions, "Soft", "Hard" );
      
    }




    void control::fixed_constraints(phys::system& system, math::real dt) {

      auto c = new phys::constraint::bilateral(6);
      system.constraint.bilateral.storage.push_back( core::unique(c) );

      system.constraint.bilateral.matrix[c][obj.get()] = math::mat::Identity(6, obj->lie.alg().dim());
      system.constraint.bilateral.values[c].setZero(6);
      
    }


    void control::lookat() {
      if( ! tracking ) return;

      using namespace math;
      // 1 creer une feature d'orientation de la tete
      auto feature = new tool::track::feature<SO3>;

      auto head = tracking->skeleton().find("head");

      using func::operator<<;
      feature->map = func::rotation<>() << func::coord<SE3>( head );
      
      
      // 2 creer la target a partir d'une frame
      struct look : tool::track::target<math::SO3> {
	
	const phys::rigid::config& pose;
	const natural head;
	std::unique_ptr<gui::frame> frame;

	look(const phys::rigid::config& pose, natural head) 
	  : pose(pose), head(head),
	    frame( core::unique<gui::frame>() ) {
	  frame->menu.title = "Look Target";

	  // TODO menu to track something else
	}

	~look() { 
	  if(frame) frame->removeFromMouseGrabberPool();
	  gui::frame::pending.push_back( std::move( frame ) );
	}

	T<math::SO3> desired(real ) const {
	  using namespace math;
	  vec3 view = (frame->transform().translation() - pose(head).translation()).normalized();
	  
	  // view is local Z
	  quat orient; orient.setFromTwoVectors(vec3::UnitZ(), view);
	  
	  vec3 up = vec3::UnitY();
	  
	  vec3 local_up = orient.conjugate() * (up - view * view.dot(up));
	  
	  quat corr; corr.setFromTwoVectors(up, local_up);
	  

	  return math::body( SO3(orient * corr), vec3::Zero() );
	}
	
      };

      auto target = new look(pose, head);
      
      feature->target = core::unique(target);

      frames.insert( target->frame.get() );
      auto chunk = tracker.add( feature );

      
      auto& actions = target->frame->menu.actions;

      
      // 3 setup de la frame
      setup_base( actions );
      setup_actions( actions, chunk, target );

      actions[ "Remove" ] = [this, chunk, target] {
	this->frames.erase( target->frame.get() );
	this->tracker.remove( chunk->feature());
      };

      // 4 place frame: 10 units ahead of current view
      SE3 config = pose(head);
      target->frame->transform( config * SE3::translation(10 * vec3::UnitZ()));
      

    }

    

    struct time_warp {
      typedef math::real domain;
      typedef math::real range;

      const math::real& speed;

      typedef math::func::euclid::scalar<math::real> impl_type;

      range operator()(const domain& x) const { return impl_type(speed)(x); }
      math::T<range> diff(const math::T<domain>& dx) const { return impl_type(speed).diff(dx); }

    };
    

    
    control::handle control::add_handle(math::natural index, const math::vec3& local) {
    
      using namespace math;
    
      const std::string name = tracking->skeleton().topology[index].name;
      auto fun = func::absolute<>(local) << func::coord<SE3>(index);
       
      return add_handle(fun, name);
    }

    control::handle control::add_com() {
      return add_handle(phys::rigid::com(tracking->skeleton()), "CoM");
    }


    control::handle control::add_handle(const math::func::any< phys::rigid::config, math::vec3 >& fun,
					const std::string& name){
      if(!tracking) return {0, 0, 0 };

      using namespace math;
      
      auto feature = new tool::track::feature<vec3>;
      
      using func::operator<<; 
      feature->map = fun;
      
      auto target = new tool::track::flex<vec3>;

      target->frame = core::unique<gui::frame>();
      target->track = false;
      target->frame->transform( math::SE3::translation(feature->map(pose)) );

      auto curve = feature->map << tracking->absolute() << time_warp{keyframe_speed}; 
      
      feature->target = core::unique(target);
      
      frames.insert( target->frame.get() );
      auto chunk = tracker.add( feature );		 
      
      // frame name
      target->frame->menu.title = name;
      
      auto& actions = target->frame->menu.actions;

      setup_base( actions );
      setup_actions(actions, chunk, target );
      
      auto traj_data = tracking->clip.sampling.sample(feature->map << tracking->absolute());
      
      std::map<real, vec3> tmp;
      
      traj_data.each([&](math::natural i) {
	  tmp[ tracking->clip.sampling.time(i)  ] = traj_data(i);
	});
      
      auto it_traj = trajectories.insert(trajectories.end(), tool::trajectory() );
      it_traj->data(tmp);
      target->recorded.curve = func::num( func::ref( *it_traj ) << time_warp{keyframe_speed} );
      
      // setup pickables
      std::vector<tool::pickable> pickables;
      core::each(tmp, [&, it_traj](math::real t, const math::vec3& value) {
	  tool::pickable pick;

	  pick.draw = [this,t, it_traj] {       
	    using namespace gl;
	    point_size(8);
	    begin(GL_POINTS, [&] {
		vertex( it_traj->operator()(t) );
	      });
	    point_size(1);
	  };
      
	  pick.pick = [this, t, it_traj] (const math::vec3& ) {
	    it_traj->gui_edit(frames, t, 5);
	  };
      
	  pickables.push_back( pick );
	});

      toggle(actions, "Edit", "Done editing");

      actions[core::hook("Edit")] = [this, it_traj, pickables] {
	edited = it_traj;
	
	core::each(pickables, [this](const tool::pickable& p) {
	    picker.add(p);
	  });
	
      };

      auto done_edit = [this, it_traj, pickables] {
	edited = trajectories.end();
	core::each(pickables, [this](const tool::pickable& p) {
	    picker.remove(p.id());
	  });
      };

      actions[core::hook("Done editing")] = done_edit;
      
      actions[ "Select" ] = [this, name] {
	this->selected = name;
      };

      actions[ core::hook("Track") ] = [this, target] { 
	target->track = true; 
      };
      
      actions[ core::hook("Untrack") ] = [this, target] { 
	target->track = false; 
      };
      
      actions[ "Remove" ] = [this, chunk, target, name, pickables, it_traj, done_edit] {
	if( selected == name ) selected = "";
	frames.erase( target->frame.get() );
	tracker.remove( chunk->feature());

	done_edit();
      };

      
      // allow scripting of handle properties
      app.ref(name + "_soft", chunk->soft ); // TODO deal with frame removal !
      app.ref(name + "_weight", chunk->weight ); // TODO deal with frame removal !
      

      return {feature, target, chunk};
    }


    void control::walk() {
      
      using namespace math;

      auto lf = this->add_handle( this->tracking->skeleton().find("lfoot"), vec3::Zero());
      auto rf = this->add_handle( this->tracking->skeleton().find("rfoot"), vec3::Zero());
      
      assert(lf.target && rf.target);
	
      lf.target->frame->menu.actions["Soft"]();
      rf.target->frame->menu.actions["Soft"]();

      vec3 l = lf.target->frame->transform().translation();
      vec3 r = rf.target->frame->transform().translation();
      
      vec3 mid = (l + r) / 2;
      vec3 com = phys::rigid::com(obj->skeleton)(pose);
      
      vec3 proj = com + (mid - com).y() * vec3::UnitY();
      vec3 orth = mid - proj;
            
      l -= orth;
      r -= orth;
      
      real off = 2;
      real height = -ground.equation(3) - off;
      l.y() = height;
      r.y() = height;

      lf.target->frame->transform( SE3(l, lf.target->frame->transform().rotation()));
      rf.target->frame->transform( SE3(r, rf.target->frame->transform().rotation()));
      
      real weight = 15;
      
      auto lsettings = lf.settings;
      auto rsettings = rf.settings;
      
      lsettings->weight = weight;
      rsettings->weight = weight;

      auto set_stance = [&, lsettings, rsettings, weight](math::real alpha){
	if( alpha < 0) alpha = 0;
	if( alpha > 1) alpha = 1;
	
	stance = alpha;
	    
	lsettings->weight = 2 * std::min(stance, 0.5) * weight;
	rsettings->weight = 2 * std::min(1 - stance, 0.5) * weight;
      };

      
      auto feet_weight = [lf, rf](math::real alpha) {
	lf.settings->weight = alpha;
	rf.settings->weight = alpha;
      };

      controller.add("Stance control", gui::tweak(stance, 0.5, set_stance)->widget());

      app.fun("stance", set_stance);
      app.fun("feet_weight", feet_weight);
      
      // 
      auto land = [this, lf, rf, off] {
	if( !contacts.center ) {
	  
	  vec3 left = ground.proj(lf.feature->map(pose));
	  vec3 right = ground.proj(rf.feature->map(pose));

	  vec3 com = ground.proj( phys::rigid::com(tracking->skeleton())(pose));
	  vec3 mid = 0.5 * (left + right);
	  
	  left = com + left - mid;
	  right = com + right - mid;

	  left -= off * ground.normal();
	  right -= off * ground.normal();
	  
	  lf.target->frame->transform( SE3(left, lf.target->frame->transform().rotation()));
	  rf.target->frame->transform( SE3(right, rf.target->frame->transform().rotation()));
	}
      };

      app.fun("land", land);
      
    }


    void control::setup_actions( core::actions& actions,
				 tool::track::chunk_base< object_type::conf_type >* chunk,
				 tool::track::target_base* target) {
      
      actions[ core::hook("Soft") ] = [this, chunk, &actions] { 
	chunk->soft = true; 
	
	actions["Weight"] = [this, chunk] {
	  this->popup(gui::tweak(chunk->weight, chunk->weight)->widget(), "Weight");
	};
	
      };
      
      actions[ core::hook("Hard") ] = [chunk, &actions] { 
	chunk->soft = false; 
	actions.erase("Weight");
      };
      

    }
    


    void control::limit_constraints(phys::system& system, math::real dt) {
      using namespace math;
      if(!obj) return;
      if(! limits.data ) return;

      
      limits.constraint.clear();

      // relative orients
      auto orients = func::vector< func::rotation<> >() << obj->joint();
      auto at = orients(obj->conf);

      // TODO BUG ? alignment in map ?
      std::map<natural, lie::ellipsoid<SO3> > lims;
      
      core::each(*limits.data, [&](math::natural i, const lie::ellipsoid<SO3>& e) {
	  if( !e.contains(at(i)) ) {
	    lims[i] = e;
	  }
	});
      
      if( lims.empty() ) return;
      
      // limits.constraint.reset( new phys::constraint::unilateral(lims.size()) );
      
      // mat J; J.resize( lims.size(), obj->dim());
      // vec c; c.resize( lims.size() );
      
      // core::log()("active limits:", lims.size());
      
      std::vector<real> norms;
      
      core::each(lims, [&](natural i, const lie::ellipsoid<SO3>& e) {
	  using func::operator<<;
	  const auto fun = Lie<SO3>::logarithm() << func::lie::L(e.center.inverse()) 
						 << func::coord<SO3>(i) << orients;
	  const auto diff = fun( obj->conf );
	  
	  const vec Jt = -obj->lie.coalg().coords(math::pull(fun, obj->conf, diff.transpose() * e.metric).body() );
	  if(Jt.norm() > 1e-1 ) {
	    auto c = new phys::constraint::unilateral(1);

	    system.constraint.unilateral.storage.push_back( core::unique(c));
	    
	    system.constraint.unilateral.matrix[c][obj.get()] = Jt.transpose();
	    system.constraint.unilateral.values[c] = vec1::Constant(0);
	  }
	  
	});
      return;
    }


     void control::contact_constraints(phys::system& system, math::real dt) {
      using namespace math;

      if(!flags.contacts) {
	contacts.center.reset( 0 );
	return;
      }
      
      std::vector<vec3> character, all;
      std::vector<real> alpha;

      natural lf = tracking->skeleton().find("lfoot");
      natural rf = tracking->skeleton().find("rfoot");

      auto weight = [&, lf, rf](natural i) -> real {
	if( i == lf ) return this->stance;
	if( i == rf ) return 1 - this->stance;
	return 0.5;
      };
      
      {
	geo::collision::system collisions;
	tool::contacts contacts;
      
	models["character"].collision.update();
	models["ground"].collision.update();

	core::each( models["character"].collision.primitives, [&](natural dof, geo::collision::primitive* p) {

	    contacts.infos[ p ].dof = obj.get();
	    contacts.infos[ p ].pullback = [&, dof](const vec3& at, const vec3& n) -> vec { 
	      const vec3 local = pose(dof).inverse()( at );
	      
	      // haxxor to get ground contacts !! FIXME !
	      if( at.y() < 0) {
		character.push_back(at);
		real w = weight(dof);
		alpha.push_back( w  );
	      }

	      all.push_back(at);

	      return tool::character::force(*obj, dof, local, n.transpose());
	    };
	    
	  });

	// ground don't react to collisions
	collisions.primitives[ models["ground"].collision.primitives.begin()->second ] = 
	  [&](natural i, const geo::collision::hit& h) { };
	
	if( box ) {
	  auto p = box->collision.primitive.get();
	  
	  contacts.infos[ p ].dof = box.get();
	  contacts.infos[ p ].pullback = [&](const vec3& at, const vec3& n) {
	    const vec3 local = box->frame().inverse()(at);
	    all.push_back(at);

	    return box->lie.coalg().coords(box->map(local, n.transpose()));
	  };
	  
	}

	// setup collision detection engine
	contacts.setup( collisions );

	// detects/fills contact info
	natural count = collisions.detect();

	// setup physical system with contact info
	contacts.setup( system, this->contacts.mu  );
      }
      
      // constraint part
      // if(character.size() >= 80 ) throw panic();

      if(character.size()) {
	
	real alpha_sum = 0;
	vec3 sum = vec3::Zero();

	for(natural i = 0; i < character.size(); ++i) {
	  sum += alpha[i] * character[i];
	}
	
	core::each(alpha, [&](real a) { alpha_sum += a; });
	
	if( alpha_sum == 0 ) contacts.center.reset(0);
	else  {
	  vec3 center = sum / alpha_sum;
	
	  // average with last center if any to prevent too fast
	  // variations
	
	  const real zob = 0.05;
	  if( contacts.center ) center = zob * center + (1 - zob) * (*contacts.center) ;
	
	  contacts.center.reset( new vec3(center) );
	  assert( !nan(*contacts.center));
	}
      } else {
	contacts.center.reset(0);
      }
      

      const vec3 com = phys::rigid::com(obj->skeleton)(pose);
      
      // display
      collisions = [this, all, com] {
	
	gl::disable(GL_LIGHTING);
	gl::point_size(10);
	gl::color( gl::red() );
	
	if( contacts.center ) {

	  // TODO ground normal
	  math::vec3 off(0, 1e-2, 0);
	  
	  gl::begin( GL_POINTS, [&] {
	      gl::vertex(*contacts.center + off);
	      
	      core::each(all, [&](const math::vec3& v) {
		  gl::vertex(v + off);
		});
	    });
	}
	
	gl::disable(GL_DEPTH_TEST);
	gl::begin( GL_POINTS, [&] {
	  
	    // core::each(all, [&](const math::vec3& v) {
	    // 	gl::vertex(v);
	    //   });

	    if(contacts.center) { 
	      gl::color(gl::blue() );
	      gl::vertex(*contacts.center);
	      
	      gl::color(gl::green() );
	      gl::point_size(100);
	      gl::vertex(com);
	      gl::vertex(com.x(), 0.0, com.z());

	    }

	   

	    // gl::point_size(200);
	    // gl::color(gl::orange() );
	    // gl::vertex(lf, rf, hd);
	  });
	gl::enable(GL_DEPTH_TEST);
	
	gl::enable(GL_LIGHTING );
      };
    }



  }
}
