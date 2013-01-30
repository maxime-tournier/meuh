#include "convert.h"

#include <math/vec.h>

namespace gui {

  qglviewer::Vec convert(const math::vec3& v) { 
    return { float(v.x()), float(v.y()), float(v.z())};
  }

  math::vec3 convert(const qglviewer::Vec& v) { 
    return {v.x, v.y, v.z};
  }


}
