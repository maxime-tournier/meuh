#include <sparse/matrix.h>
#include <sparse/prod.h>

#include "simulator.h"

#include <core/timer.h>
#include <script/meuh.h>

#include <sparse/norm.h>
#include <sparse/scalar.h>
#include <sparse/sum.h>

#include <phys/error.h>

namespace tool {

  using namespace phys;
  
  void simulator::startAnimation() {
    if( background ) player.play();
    QGLViewer::startAnimation();
  }

  void simulator::stopAnimation() {
    auto lock = this->lock();
    if( background ) player.pause();
    QGLViewer::stopAnimation();
    // std::this_thread::sleep_for( std::chrono::seconds(1) );
  }


  simulator::~simulator() {
    player.exit();
  }

  simulator::simulator(QWidget* parent) 
    : viewer(parent),
      post_steps(1),
      api("simulator")
  { 
    
    api
      .ref("t", player.t)
      .ref("dt", player.dt)
      .ref("correction", flags.correction)

      .fun("reset", [&] { this->reset(); } ) // derived
      .fun("step", [&](math::natural n) {
	  for(math::natural i = 0; i < n; ++i) {
	    animate();
	    updateGL();
	  }
	})
      .fun("post_steps", [&](math::natural n) {
	  post_steps = n;
	})
      .fun("fps", [&] {
	  return player.fps;
	})
      .fun("background", [&](bool b) {
	  if(b) {
	    saveAnimation();
	    background = true;
	    setAnimationPeriod( 1000.0 / 60.0 );
	    restoreAnimation();
	  } else {
	    player.pause();
	    setAnimationPeriod( 0 );
	    background = false;
	  }
	});
      ;
    
    script::require("simulator");
    script::require("log");
    
    flags.correction = true;

    t = 0;
    dt = 0.01;
    post_steps = 0;
   
    background = false;

    player.start();
  }
  

  void simulator::fix(const phys::dof::velocity&,
		      const phys::dof::velocity& ) {

  }

  math::real simulator::time() const { return player.t; }

  void simulator::reset() { 
    player.t = 0; 
    script::exec("hook( simulator.post_reset )");
  }
  
  void simulator::constraints() { };

  void simulator::animate() {
    auto lock = this->lock();
    
    if( player.animator ) {
      if(!background) player.step();
      script::exec("hook( simulator.post_animate )");
      return;
    }
    
    return;
    
    try {
      core::timer timer;

      // setup system geometry
      setup();

      // solve
      auto p = momentum( t, dt );
      auto v = solve( p );

      // integration
      integrate(v, dt);

      phys::dof::velocity sum;
    
      // compute system response
      for(math::natural i = 0; i < post_steps; ++i) {

	constraints();

	const bool fast = i > 0;
	factor( fast );
      
	phys::dof::velocity tmp;
      
	if( flags.correction ) tmp = sum;
	auto delta = correction( tmp );  

	sum += delta;
      
	correct( delta );
      }

      fix(v, sum);

    
      // post
      math::real time = timer.start();
      script::exec("log.now().time = %%", time );
      script::exec("hook( simulator.post_animate )");
    
      t += dt;
    }
    catch( const phys::error& e ) {
      core::log( e.what() );
      stopAnimation();
    }
    
  }

  simulator::lock_type simulator::lock() {
    return lock_type{mutex};
  }

  dof::momentum simulator::momentum(math::real , math::real ) { return dof::momentum(); }

  void simulator::integrate(const phys::dof::velocity& , math::real ) { }
  void simulator::correct(const phys::dof::velocity&) { }

  void simulator::factor( bool ) { }
  phys::dof::velocity simulator::solve(const phys::dof::momentum& ) { return {}; }
  
  void simulator::setup() { } 
  dof::velocity simulator::correction(const phys::dof::velocity& ) { return {}; }
}
