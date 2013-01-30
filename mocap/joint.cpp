#include "joint.h"

namespace mocap {

  std::ostream& operator<<(std::ostream& o, const joint& j ) {
    o << j.id << " " << j.name << " " << j.offset.transpose();
    return o;
  }


}

