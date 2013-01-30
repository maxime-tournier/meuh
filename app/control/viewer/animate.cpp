#include "control.h"

#include <core/raii.h>

#include <tool/viewer.h>

#include <control/solver/sqp.h>
#include <control/solver/ours.h>

#include <sparse/linear/diagonal.h>
#include <sparse/linear.h>

#include <gl/blend.h>
#include <phys/rigid/relative.h>

namespace gui {

  namespace viewer {
    
    using core::log;
    using tool::convert;

    static phys::rigid::config from_euler(const math::vector< math::vec3 >& angles,
					  const math::vector< math::SO3>& offsets) {
      phys::rigid::config res;
      res.resize( angles.rows() );
      
      using namespace math;
      
      std::vector<natural> order = {2, 1, 0};
      const auto exp = Lie<SO3>().exp();
      
      angles.each([&](natural i ) {
	  SO3 q = SO3::identity();

	  for(natural j = 0; j < order.size(); ++j) {
	    const vec3 axis = vec3::Unit(order[j]);
	   
	    q = q * exp( angles(i)(order[j]) * axis);
	  }
	  res(i) = SE3::rotation(offsets(i).inverse() * q);
	});
      
      return res;
    }




    void control::animate() {
      QMutexLocker lock(&mut_reset);
      
      if(!tracking) return;
      if(!obj) return;
      
      if(reset_needed) start_pose(); 
      
      // TODO obselete ?
      // if(com.automatic) { 
      // 	com_auto();
      // }

      
      auto post_animation = core::raii::fun([&] {
	  using namespace math;
	  // // cleanup
	  // system.clear();
	  _time.control->step( dt );
	
	  // animation hook
	  engine.exec("hook(animate)");

	  // revolve around projected com
	  vec3 com = ground.proj(phys::rigid::com(tracking->skeleton())(pose));
	  
	  vec3 delta = com - convert( camera()->revolveAroundPoint() );
	
	  camera()->setRevolveAroundPoint( convert( com ) );
	  if( flags.travelling ) camera()->setPosition( camera()->position() + convert( delta) );

	  light->setSceneRadius( 200 );
	  light->setAspectRatio(1.0);
	  light->setZClippingCoefficient(1);
	  
	  vec3  zob = com + 100 * ground.normal();
	  
	  light->setPosition( convert(zob) );
	  light->setSceneCenter( camera()->revolveAroundPoint() );
	  
	  setSceneCenter( convert(com) );

	  if( limits_viewer ) {
	    limits_viewer->current = (func::vector<func::rotation<> >() 
				      << phys::rigid::relative(tracking->skeleton()))(pose);
	  }
	  
	});
      

      if(flags.euler ) {
	
	auto angles = euler_pca.ccfk(obj->pga.n)(std::get<2>(obj->conf));
	auto orient = from_euler( angles, euler_offsets );

	orient(tracking->skeleton().root) = math::SE3( std::get<1>(obj->conf), 
						 std::get<0>(obj->conf));

	pose = phys::rigid::absolute{tracking->skeleton()}(orient);
	return ;
      }

      
      // we clamp time value for tracker, to fix silly curve behavior
      // at the boundaries
      const math::real t = std::min(time(), (mocap::end( tracking->clip.data ) - 2*dt) / keyframe_speed);
      tracker.update( t );

      using namespace math;
      
      struct {
	::phys::system phys;
	::control::system ctrl;
      } system;
      
      // tool::track::engine tracker(system.phys, system.ctrl, &obj->dof, tracking->skeleton().lie());
      // tracker.curves = tracking->absolute();
      
      // system setup (masses, stiffness, etc)
      setup(system.phys, system.ctrl, dt);

      // forces
      const phys::dof::momentum f = impulses(system.phys, dt);
      
      // ::control::obj::collection objective;
      // ik_constraints(t, dt);
       
      // com_constraints(system.phys, system.ctrl, dt);

      try {

	// constraints
	contact_constraints(system.phys, dt);
	if(flags.enforce_limits) limit_constraints(system.phys, dt);  
	if(flags.fixed) fixed_constraints(system.phys, dt);

	// control
	reference_objective(system.ctrl);
	balance_objective(system.ctrl);
	// contact_objective(system.ctrl, system.phys, &obj->dof);

	// tracking (no control if no contacts, use flags.tracking to force anyway)
	bool soft = flags.tracking || contacts.center.get();
	tracker.setup(system.phys, system.ctrl, obj->state(), time(), dt, soft, flags.correct);
	
	// discretize friction constraints
	system.phys.constraint.discretize(3);
	
	phys::dof::velocity v;

	if( flags.control ) {
	  if( flags.bretelles ) { 
	    const ::control::solver::ours solver(system.phys, system.ctrl, dt);
	    // const phys::solver::qp::direct solver(system.phys);
	    v = solver(f);
	  } else {
	    phys::dof::velocity v_prev;
	    v_prev[ obj.get() ] = obj->lie.alg().coords( obj->velocity );

	    // const ::control::solver::panda solver(system.phys, system.ctrl, v_prev, dt, flags.relax, laziness, reflexes);
	    const ::control::solver::sqp solver(system.phys, system.ctrl, v_prev, dt, flags.friction, laziness);
	    
	    v = solver(f, obj->act, unilateral);
	    
	    // const ::phys::solver::qp solver(system.phys);
	    // v = solver(f);
	  }
	} else {
	  phys::solver::qp::direct solver(system.phys, dt);

	  if( flags.correct ) {
	    solver.correct(true);
	    phys::dof::velocity corr = solver( phys::dof::momentum() );
	    const vec& vobj = corr.find(obj.get())->second;
	    
	    obj->conf = obj->lie.prod( obj->conf, obj->lie.exp()( obj->lie.alg().elem(vobj)));
	    solver.correct(false);
	  }
	  
	  // const phys::solver::qp::precise solver(system.phys, dt);
	  v = solver(f);
	}
	

	// momentum
	phys::dof::velocity mu = sparse::linear::diag(system.phys.mass) * v;
	
	// log("force norm: ", sparse::linear::norm(f));
	// const phys::dof::velocity v = phys::solver::si(solver.cache,
	// 						 algo::stop().it(2000).eps(1e-18))(f);
	vec vobj = v.find(obj.get())->second;

	// log("velocity norm: ", sparse::linear::norm(v));

	

	const real Ec = 0.5 * sparse::linear::dot(v, mu);
	// log 

	bool log_momenta = false;
	if( log_momenta )
	  {
	    auto dpose = tool::character::absolute(*obj).diff( math::body(obj->conf, obj->lie.alg().elem(vobj)) );
	  
	    if(kinetic.empty() ) { kinetic.push_back( std::make_pair(0, Ec) ); }
	    else {
	      kinetic.push_back( std::make_pair( kinetic.back().first + dt, Ec) );
	    }
	    const vec v = obj->skeleton.lie().alg().coords( dpose.body() );

	    const vec3 H = phys::rigid::momentum::angular(obj->skeleton, pose) * v;
	    const vec3 L = phys::rigid::momentum::linear(obj->skeleton, pose) * v;
	  
	    if(angular.empty() ) { angular.push_back( std::make_pair(0, H ) ); }
	    else {
	      angular.push_back( std::make_pair( angular.back().first + dt, H) );
	    }

	    if(linear.empty() ) { linear.push_back( std::make_pair(0, L ) ); }
	    else {
	      linear.push_back( std::make_pair( linear.back().first + dt, L) );
	    }
	  }


	

	// log("TOTAL KINETIC ENERGY: ", Ec);

	const real Emax = 1e14;

	if( Ec > Emax ) {
	  throw panic();
	}

	if( math::nan(vobj) ) {  
	  throw panic();
	}

	// integration
	// tool::character::integrate(*obj, vobj, mu[ obj.get() ], dt);
	{
	  auto tmp = obj->lie.alg().elem( vobj );
	  obj->integrate(tmp,
	  		 tool::character::momentum(*obj, obj->conf, tmp),
	  		 dt);
	}

	if( box ) { 
	  box->integrate( box->lie.alg().elem(v[ box.get() ]), 
			  box->lie.coalg().elem(mu[ box.get() ]), dt); 
	  box->update();
	}
	
	// clamp( std::get<1>(obj->conf), bounds);
      
	// set global pose

	
	pose = tool::character::absolute(*obj)( obj->conf );
      


	if ( flags.show_momenta ) {
	  vec3 com = phys::rigid::com(obj->skeleton)(pose);
	  auto dpose = tool::character::absolute(*obj).diff( math::body(obj->conf, obj->lie.alg().elem(vobj)) );
	  const vec v = obj->skeleton.lie().alg().coords( dpose.body() );

	  const vec3 H = phys::rigid::momentum::angular(obj->skeleton, pose) * v;
	  const vec3 L = phys::rigid::momentum::linear(obj->skeleton, pose) * v;

	  if( !to_draw.empty() ) to_draw.pop_front();
	  
	  const real ratio = 1000 / push.stiffness;
	 
	  
	  to_draw.push_front([this, dt, com, H, L, ratio] { 
	      using namespace gl;

	      const real radius = 0.5;
	      const natural sub = 24;

	      
	      // color( lighter(blue()) );
	      // if( (dt * H.norm()) > 0.5 ) this->drawArrow(convert(com), 
	      // 					    convert(com + ratio * dt * H), radius, sub);
	      color( orange() );
	      
	      if( (dt * L.norm()) > 0.5)  this->drawArrow(convert(com), 
							  convert(com + ratio * 8 * dt * L), radius, sub);
	    
	    });

	  // nappe
	  to_draw.push_back( [this,dt, com, H, L, ratio] { 
	      using namespace gl;
	      gl::normal( L.cross(H).normalized() );
	      gl::blend()([&] {
		  point_size( 4 );
		  if(flags.trace) {
		    disable( GL_LIGHTING );
		    begin( GL_POINTS, [&] { 
			
			color(  orange() ); 
			vertex(com);
			
			// color( transp( lighter(blue()) ) ); 
			// vertex(com + ratio * dt * H);
		      });
		    enable( GL_LIGHTING );
		  }
		  
		  line_width( 1.0 );
		  if(flags.nappe) begin( GL_LINES, [&] {
		      color( transp(orange()) );
		      vertex(com, com + ratio * dt * L);
		      
		      color( transp( blue() ));
		      vertex(com, com + ratio * dt * H);
		    });
		});
	    });
	  
	}
	    



      } catch (const panic& e) {
	camera()->setRevolveAroundPoint( convert( vec3::Zero() ) );
	core::log("panic !");
	return;
      } catch (const math::mosek::exception& ) {
	core::log("mosek panic !");
	camera()->setRevolveAroundPoint( convert( vec3::Zero() ) );
	return;
      }
      
    }





    
  

    phys::dof::momentum control::impulses(const phys::system& system, math::real dt) {
      using namespace math;
      
      const vec& stiffness = system.stiffness.find( obj.get() )->second;
      
      // forces sum
      vec force = vec::Zero(obj->dim);
      
      // gravity forces
      if(flags.gravity) force += tool::character::weight(*obj);
      
      // // modal spring
      // force.tail(obj->ngeo) += - stiffness.tail(obj->ngeo).cwiseProduct( std::get<1>(obj->conf) );
      
      // push
      if( push.active() ) {
	force += tool::character::force(*obj, push.dof, push.local, push.force() );
	push.pop();
      }
      
      // inertia forces
      if(flags.inertia) force += tool::character::inertia(*obj, 0.5); // a la serpe

      phys::dof::momentum res; 
      
      const vec p = obj->lie.coalg().coords( obj->momentum );
      {
	using euclid::operator*;
	
	if( !flags.momentum ) {
	  obj->momentum = obj->lie.coalg().zero();
	}
	res[ obj.get() ] = obj->lie.coalg().coords( obj->net( obj->lie.coalg().elem(force), dt ) );
      }
      
      if( box ) {
	tool::rigid::momentum_type grav = box->map(vec3::Zero(), - phys::constant::g * box->body.mass * force::UnitY());
	// tool::rigid::momentum_type net = euclid::sum(box->momentum, euclid::scal(dt, grav));

	// res[ &box->dof ] = box->lie.coalg().conf(net).transpose();
	res[ box.get() ] = box->lie.coalg().coords( box->net(grav, dt) );
      }
      
      return res;
    }



      void control::setup(phys::system& system, ::control::system& ctrl, math::real dt) {
      using namespace math;

      // modal stiffness/damping
      
      
      const mat M = tool::character::mass(*obj, obj->conf, dt);
      
      assert( M.fullPivLu().isInvertible());

      // system setup 
      system.mass[ obj.get() ] = M;
      
      // actuation
      mat A = mat::Zero(obj->dim, obj->pga.n);
      A.bottomRows(obj->pga.n).diagonal() = vec::Ones(obj->pga.n); // obj->pga.data.eigen().head( obj->pga.n ).cwiseInverse();
      // // TODO scale by eigenvalue ?
      
      ctrl.actuator.matrix[ &obj->actuator ] [obj.get() ] = A.transpose();

      
      ctrl.actuator.bounds[ &obj->actuator ] = strength *  vec::Ones(obj->pga.n)
	// (vec::Ones(obj->pga.n) + obj->pga.data.eigen().head( obj->pga.n ) / obj->pga.data.eigen()(0) * 10);
      ;

      ctrl.actuator.bounds[ &obj->actuator ].head( unactuated ).setZero();
      
      // box:
      if( box ) {
	system.mass[ box.get() ] = box->body.tensor();
      }
      
    }



  }
  

}
