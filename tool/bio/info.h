#ifndef BIO_INFO_H
#define BIO_INFO_H

#include <math/types.h>
#include <string>

namespace tool {
  namespace bio {

    struct info {
      virtual ~info();

      virtual math::vec3 dim(const std::string& name) const = 0;
      virtual math::real mass(const std::string& name) const = 0;
      virtual math::vec3 inertia(const std::string& name) const = 0;
    
    };

  }
}


#endif
