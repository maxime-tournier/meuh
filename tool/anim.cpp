#include "anim.h"

#include <phys/unit.h>
#include <core/unique.h>

#include <core/log.h>
#include <core/macro/here.h>

#include <gui/fix.h>

namespace tool {
  namespace anim {


    static core::logger log;

    using namespace math;
    
    time::time() : value(0) { }

    const real& time::get() const { return value; }

    void time::set(real t) { 
      value = t; 
      // core::log()("time changed:", t);
      emit changed(t); 
    }
    
    real control::start() const { return _start; }
    real control::end() const { return _end; }
    real control::speed() const { return _speed; }
    bool control::loop() const { return _loop; }
    real control::interval() const { return _qtimer.interval() * phys::unit::ms; }
    
    control::control() : _time( core::unique<anim::time>() ){
      reset();
      connect();
    }

    void control::reset() {
      _start = 0; 
      _end = 0;
      _speed = 1;

      _loop = false;
    }
     
    anim::time* control::time() const { assert(_time); return _time.get(); }

    void control::start(real x) { _start = x; assert( start() <= end() ); }
    void control::end(real x) { _end = x; assert( start() <= end() ); }
    void control::speed(real x) { _speed = x; }
    
    void control::interval(real x) { _qtimer.setInterval( x / phys::unit::ms ); }
    void control::loop(bool b) { _loop = b; }
    
    void control::connect() {
      
      QObject::connect(&_qtimer, SIGNAL(timeout()),
		       this, SLOT(poll())); 

    }

    real control::length() const { return end() - start(); }

    void control::go(real t) { 
      _qtime.restart();
      time()->set( clamp(t) );
    }

    math::T<math::real> control::dt() const {
      return math::body(time()->get(), speed() );
    }

    void control::step(math::real dt) {
      go( time()->get() + dt );
    }
    
    math::real control::clamp(math::real t) const {
      return std::min(end(), std::max(start(), t));
    }

    void control::poll()  {
      std::lock_guard<std::mutex> lock(mutex);

      // log( macro_here, "poll" );
      const real elapsed = _qtime.elapsed() * phys::unit::ms;

      real value = time()->get() + speed() * elapsed;
      
      if( loop() && time()->get() >= value ) value = start();

      time()->set( clamp( value ) );
    }
    
    void control::play() {
      // log( macro_here, "play" );
      go( time()->get() );
      
      emit playback( true );
      _qtimer.start();
    }
    
    void control::pause() {
    
      // log( macro_here, "pause" );
      emit playback( false );
      _qtimer.stop();
    }

    bool control::playing() const { return _qtimer.isActive(); }

    
    void control::restart() {
      // log( macro_here, "restart" );
      go( start() );
    }

    void control::stop() {
      // log( macro_here, "stop" );
      pause();
      restart();
    }
    
    
  }
}
