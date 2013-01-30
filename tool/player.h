#ifndef TOOL_PLAYER_H
#define TOOL_PLAYER_H

#include <math/types.h>
#include <core/semaphore.h>
#include <phys/step/animator.h>

#include <functional>


#include <core/log.h>
#include <core/timer.h>
#include <thread>

#include <core/callback.h>
#include <core/noop.h>


namespace tool {

  class player {
    
    typedef core::callback state_type;
    state_type state;
    
    core::semaphore sema;
    std::thread thread;

    core::timer timer;

    typedef std::recursive_mutex mutex_type;
    mutex_type mutex;
    
  protected:
    void go(const state_type& s);

  public:
    math::real t, dt;
    math::real fps;
    phys::step::animator_type animator;

    player(player&& ) = default;
    player();
    
    // performs just one step, updating t and fps
    void step();

    // actions
    void pause();
    void play();

    // critical section on animator
    typedef std::unique_lock<mutex_type> lock_type;
    lock_type lock();
    
    // thread start
    void start();
    void exit();

    void once(core::callback c);
  };



}

#endif
