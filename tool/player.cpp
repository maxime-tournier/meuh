#include "player.h"

#include <core/debug.h>

namespace tool {

  player::player()  : t(0), dt(1e-2) { }

  void player::pause() {
    
    core::semaphore s;
    bool ok = true;
    go( [&,ok] () mutable { 
	if(ok) { s.notify(); ok = false; }
	sema.reset(); 
	sema.wait(); } );

    s.wait();
  }

  void player::play() {
    
    go( [&] { step(); } );
    
  }

  void player::once(core::callback cb) {
    state_type copy = state;
    go( [copy, cb,this] {
	cb();
	go( copy );
      } );
  }

  void player::step() {
    timer.start();
    if( this->animator ) {
      auto lock = this->lock();
      this->animator(this->t, this->dt);
    }
    else core::log("no animator lol");

    this->t += this->dt;
    this->fps = 1.0 / timer.start();
  }

  void player::start() {
       
    thread = std::thread{ [&] { 
	auto copy = [&]() -> state_type {
	  auto lock = this->lock();
	  return state;
	};
	
	macro_debug("player started");
	while( state_type c = copy() ) {
	  macro_debug("player state");
	  c();
	}
	macro_debug("player exited");
      }};
    pause();

   
  }


  void player::go(const state_type& s) {
    auto lock = this->lock();
    state = s;
    sema.notify();
  }

  void player::exit() {
    go( state_type{} );
    thread.join();
  }


  player::lock_type player::lock() {
    return lock_type{mutex};
  }

}
