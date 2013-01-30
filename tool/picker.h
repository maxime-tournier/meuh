#ifndef TOOL_PICKER_H
#define TOOL_PICKER_H

#include <core/callback.h>
#include <math/vec.h>
#include <map>

namespace tool {

  class pickable {
    static math::natural count;
    math::natural _id;
  public:

    math::natural id() const;
    
    pickable();

    core::callback draw;
    
    // returns corrected hit pos if needed
    std::function< math::vec3 (const math::vec3& )> pick;
    std::function< void(const math::vec3& )> move; 
    std::function< void()> end; 
    
  };
  

  class picker {
    
    std::map< math::natural, tool::pickable > pickables;

  public:
    
    void draw() const;
    
    void add(const tool::pickable& );
    void remove(math::natural );
    
    // should be operator()
    const pickable& at(math::natural) const;
    pickable& at(math::natural);

    math::natural size() const;

    void clear();

  }; 

}

#endif
