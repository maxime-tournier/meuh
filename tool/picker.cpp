#include "picker.h"

#include <core/stl.h>
#include <gl/name.h>

#include <stdexcept>

namespace tool {

  math::natural pickable::count = 0;


  pickable::pickable() : _id(count++) { }

  math::natural pickable::id() const { return _id; }

  void picker::draw() const {
    
    core::each(pickables, [&](math::natural i, const tool::pickable& p) {
	using namespace gl::name;
	push(i);
	p.draw();
	pop();
      });
    
  }


  void picker::add(const pickable& p) {
    pickables[p.id()] = p;
  }

  void picker::remove(math::natural id) {
    pickables.erase( id );
  }

  const pickable& picker::at(math::natural id) const {
    auto it = pickables.find(id);
    if( it == pickables.end() ) throw std::logic_error("unknown pickable");
    return it->second;
  };

   pickable& picker::at(math::natural id) {
     auto it = pickables.find(id);
     if( it == pickables.end() ) throw std::logic_error("unknown pickable");
     return it->second;
   };
  
  void picker::clear() {
    *this = picker();
  }
  
  math::natural picker::size() const { return pickables.size(); }
}
