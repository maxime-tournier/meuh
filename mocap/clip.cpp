#include "clip.h"

namespace mocap {

  clip& clip::operator=(clip&& other)  {
    skeleton = std::move(other.skeleton);
    data = std::move(other.data);
    return *this;
  }


}
