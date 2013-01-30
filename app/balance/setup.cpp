#include "viewer.h"

#include <phys/rigid/com.h>
#include <phys/rigid/momentum.h>
#include <phys/constant.h>

#include <math/svd.h>


#include <gl/skeleton.h>


#include <math/func/maybe.h>



void viewer::reset() {
  // setup();
  
  if( character ) { 
    auto cb = pre_animate.lock();
    core::push_back( *cb, [&] { character->reset(); } );
    character->act.setZero();

    // character->H.setZero();
    // character->L.setZero();
  }

  *post_draw.lock() = 0;
  support.center.reset();

  simulator::reset();
}


static const phys::constraint::bilateral test(6);



// setup character
void viewer::setup() {
  if( !character ) return;
  
  // saveAnimation();
  // stopAnimation();

  // TODO make an entry point in script ?
  
  using namespace math;

  // lagrangian setup
  auto conf = character_type::state_type::conf();
  auto abs = character->absolute() << conf;
    
  auto com = phys::rigid::com(character->skeleton) << abs;
  
  // gravity
  const vec3 G = phys::constant::g * vec3::UnitY();
  auto gravity = func::euclid::dot1<vec3>(G) << com;
  
  
  // potential energy
  auto V = 
    func::make_maybe( [&] { return flag.gravity; }, gravity, character->zero() ) + 
    character->potential() +
    math::func::ref( character->interaction )
    ;

  // total lagrangian
  auto L = character->kinetic() - V;
  
  // integrator
  // auto integrator = phys::integrator::euler::symplectic(L, character->lie, 1);

  phys::step::solver_maker_type solver_maker = [&](const math::mat& M) -> phys::step::solver_type  {
    
    using namespace phys;

    step::pack< constraint::unilateral > unilateral;
    step::pack< constraint::bilateral > bilateral;
    
    collisions();
    
    // unilateral.matrix = contacts.N.geometry;
    // bilateral.matrix = contacts.T.geometry;

    unilateral.matrix = contacts.K.geometry;
    unilateral.vector = contacts.K.corrections;
    
    // math::iter iter;
    
    // iter.bound = 60;
    // iter.epsilon = 1e-16;
    
     
    math::natural a = character->inner_dofs();
    
    
    math::mat MM = M; 
    
    // levmar damping
    MM.diagonal().tail( a ) *= ( 1 + dt * character->damping );
    
    
    // implicit modal spring
    // MM.diagonal().tail( a ) += (dt * dt) * character->stiffness * vec::Ones(a);

    math::mat Minv;
    
    switch( solver ) {
    case uncontrolled: 
      {
	step::kinematic kinematic(bilateral, unilateral);
	
	return step::constrained{MM, kinematic, iter,
	    this->lambda,
	    this->mu};
      }
    case fake: {
      auto control = this->control(MM, Minv);
      
      if( (!flag.bretelles) && (!control.obj.matrix.empty()) ) {
	bilateral.matrix(&test, character.get()) = control.obj.matrix.transpose().topRows(6) * control.obj.matrix;
	bilateral.vector(&test) = control.obj.matrix.transpose().topRows(6) * control.obj.vector;
      }
      
      step::kinematic kinematic(bilateral, unilateral);


      return step::fake_control{MM, kinematic, control, iter, 
	  this->lambda,
	  this->mu};
    }
      
    case full: {
      
      Minv = math::svd(MM).inverse();
      step::kinematic kinematic(bilateral, unilateral);
      
      return step::control_constrained{MM, Minv, kinematic, this->control(MM, Minv), iter, character->act,
	  this->lambda,
	  this->mu};
    }
    }
    
    throw core::error("derp");
  };
  

  auto integrator = phys::step::symplectic(L, solver_maker, this->steps, character->lie, flag.pull);
  
  
  // animation loop setup
  player.animator = [&,integrator](math::real , math::real dt) {
    
    {
      auto cb = pre_animate.lock();
      if( *cb ) (*cb)();
      *cb = 0;
    }
    {
      auto cb = post_draw.lock();
      *cb = 0;
    }

    // update state
    character->bundle = integrator(character->bundle, dt);
    
    // update drawing proxy
    *character->proxy.lock() = character->absolute()( character->g() );
    *character->dproxy.lock() = d( character->absolute() )( character->g() ) (character->v());
    
    using namespace math;

    // update com trajectory display
    vec3 com = phys::rigid::com( character->skeleton )( *character->proxy.lock() );
    
    // update lua com position
    if( !math::nan(com)) {
      script::exec("app.char.com = {%%, %%, %%}", com.x(), com.y(), com.z() );
    }

    vec vel = vec::Zero(character->skeleton.lie().alg().dim());
    euclid::get(vel, *character->dproxy.lock() );

    vec3 H = phys::rigid::momentum::angular(character->skeleton, *character->proxy.lock() ) * vel;
    vec3 L = phys::rigid::momentum::linear(character->skeleton, *character->proxy.lock() ) * vel;
    
    // vec3 Hdot = (H - character->H) / dt;
    // vec3 Ldot = (L - character->L) / dt;
    
    // vec3 lhs = Hdot.normalized();
    // vec3 rhs = (Ldot + character->skeleton.mass * phys::constant::g * vec3::UnitY()).normalized();
    
    // core::log("sanity check:", lhs.dot(rhs));

    auto forces = contacts.forces_cone( mu );
    auto points = contacts.points;

    auto z = contacts.zmp( forces );
    
    vec3 zmp = vec3::Zero();

    if( std::abs(z.dir.y() ) > math::epsilon ) {
      real height = -2;
      real alpha = (height - z.base.y()) / z.dir.y();
      
      zmp = z.base + alpha * z.dir;
      // core::log("zmp:", zmp.transpose());
    }
    
    // character->H = H;
    // character->L = L;
    
    auto cb = post_draw.lock();
    core::push_back(*cb, [&,com, H, L, zmp, dt, forces, points] {
	using namespace gl;

	if( !flag.hints ) return;
	
	// gl::vertex( com );
	gl::disable(GL_DEPTH_TEST);
	gl::color( gl::orange() );
	gl::line( com, com + dt * H );

	gl::color( gl::blue() );
	gl::line( com, com + dt * L );
	
	if( support.center ) {
	  
	  gl::point(ground.model.geo.proj(com));
	  
	  gl::color( gl::orange() );
	  gl::point_size(8);
	  gl::point( zmp );
	  gl::point_size(4);
	  
	  point_size(4.0);
	  gl::color( green() );
	  gl::point( *support.center );
	}

	
	gl::enable(GL_DEPTH_TEST);

	gl::color( gl::red() );
	for(natural i = 0; i < forces.size(); ++i) {
	  line(points[i], points[i] + forces[i] );
	}


      }); 
    
  };
  
 

  // picking
  picker.clear();
  character->proxy.lock()->each([&](math::natural i) {
      
      if( exclude.find( character->skeleton.body(i).name ) != exclude.end() ) return;
      
      tool::pickable p;

      // how to draw each bone
      p.draw = [&, i] {
	// auto lock = player.lock();
	auto proxy = character->proxy.lock();
	gl::skel::draw_box(character->skeleton, 
			   *proxy,
			   i);
	
      };

      // what happens on pick
      math::real stiff = pick.stiff;
      p.pick = [&, i, stiff](const math::vec3& at) -> math::vec3 { 

	core::log("picked", character->skeleton.body(i).name);
	
	using namespace math::func;
	auto ith = coord<phys::rigid::config>(i);
	
	math::vec3 fixed;
	
	{
	  // auto lock = player.lock();
	  auto proxy = character->proxy.lock();
	  pick.local = ith(*proxy).inverse()( at );

	  // sanity clamp
	  pick.local = (0.5 * character->skeleton.body(i).dim).cwiseMin(pick.local);
	  pick.local = (-0.5 * character->skeleton.body(i).dim).cwiseMax(pick.local);

	  fixed = ith(*proxy)(pick.local);
	
  
	  // core::log("local:", pick.local.transpose());
	}
	  
	typedef character_type::state_type state_type;
	  
	const auto map = rigid_map<>(pick.local) << ith << character->absolute() << state_type::conf();
	const auto target = cst< character_type::state_type::type>(at);
	
	{
	  auto cb = pre_animate.lock();
	  auto fun =  (0.5 * stiff) * norm2( target - map);
	  core::push_back(*cb, [&, fun] { character->interaction = fun; });
	}

	return fixed;
      };
      
      // what happens on mouse move (during pick)
      p.move = [this, i, stiff](const math::vec3& at){ 
	
	// // auto lock = player.lock();
	using namespace math::func;
	auto ith = coord<phys::rigid::config>(i);

	typedef character_type::state_type state_type;

	const auto map = rigid_map<>(pick.local) << ith << character->absolute();
	const auto target = cst< character_type::state_type::type>(at);

	{
	  // auto lock = player.lock();
	  auto cb = pre_animate.lock();
	  auto fun = (0.5 * stiff) * norm2( target - (map << state_type::conf()));
	  core::push_back(*cb, [&, fun] { 
	      character->interaction = fun;
	      // character->interaction = (0.5 * stiff) * norm2( character->lie.log() << state_type::conf(),
	      // 						      character->lie.alg());
	    });
	}
	


	{
	 
	  pick_cb = [&,at, map] {
	    math::vec3 pick_abs = map( character->g() );
	    using namespace gl;
	    // disable(GL_LIGHTING);
	    color(yellow());
	    line(pick_abs, at);
	    // enable(GL_LIGHTING);
	  };
	}
      };
      
      // what happens on picking end
      p.end = [this] {
	
	{
	  auto cb = pre_animate.lock();
	  // auto lock = player.lock();
	  core::push_back(*cb, [&] { 
	      character->interaction = character->zero(); 
	    });

	  pick_cb = 0;

	}
      };

      picker.add( p );

    });


  // features manager
  features.reset( new tool::control(character->lie.alg().dim()));
  
  // restoreAnimation();
}





