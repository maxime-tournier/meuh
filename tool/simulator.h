#ifndef TOOL_SIMULATOR_H
#define TOOL_SIMULATOR_H

#include <tool/viewer.h>
#include <phys/system.h>
#include <phys/engine.h>
#include <phys/solver/any.h>
#include <phys/dof.h>

#include <script/api.h>

// transition
#include <phys/step/animator.h>
#include <phys/step/data.h>

#include <tool/player.h>

namespace tool {

  struct simulator : viewer {

    virtual void reset();
    void animate();
    
    // returns external force
    virtual phys::dof::momentum momentum(math::real t, math::real dt);
    
    // setup 
    virtual void setup();
    virtual void constraints();
    
    // compute system response
    virtual void factor(bool fast = false);
    
    virtual phys::dof::velocity solve(const phys::dof::momentum& net);
    virtual phys::dof::velocity correction(const phys::dof::velocity& delta = phys::dof::velocity() );
    
    virtual void integrate(const phys::dof::velocity& , math::real dt);
    virtual void correct(const phys::dof::velocity& );
    
    simulator(QWidget* = 0);
    ~simulator();
    
    virtual math::real time() const;
    
    // fix velocities based on unconstrained velocities and constraint
    // deltas
    virtual void fix(const phys::dof::velocity& velocity,
		     const phys::dof::velocity& delta);
    
    tool::player player;
    
    void startAnimation();
    void stopAnimation();

    // stops, waits til animate end if needed
    void stop();

  protected:
    math::real t, dt;
    math::natural post_steps;
    
    script::api api;

    phys::step::animator_type animator;
    phys::step::data data;
    
    bool background;

    struct {
      bool correction;
    } flags;

  private:
    typedef std::recursive_mutex mutex_type;
    typedef std::unique_lock<mutex_type> lock_type;
    
    mutex_type mutex;

  protected:
    
    lock_type lock();
    
  };

}



#endif
