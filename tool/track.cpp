#include <sparse/matrix.h>

#include "track.h"

namespace tool {
 
  namespace track {

    // template<> 
    // math::vec3 desired<math::vec3>(gui::frame* f){ return f->transform().translation(); }
    
    // template<> 
    // void update<math::vec3>(gui::frame* f, const math::vec3& pos) {
    //   f->transform(math::SE3(pos, f->transform().rotation()));
    // }

    template<>
    math::T<math::vec3> handle<math::vec3>::desired( math::real t ) const
    {
      return math::body(frame->transform().translation(),
			math::vec3::Zero() );
    }
    
  }
}
