#ifndef IMAGE_FORMAT_H
#define IMAGE_FORMAT_H

#include <math/vec.h>

namespace image {

  namespace format {

    struct rgb {
      
      unsigned char r, g, b;
      math::vec3 gl() const;
      
      rgb();
      rgb(const math::vec3& );

    };
    

    struct bgr {
      
      unsigned char b, g, r;
      math::vec3 gl() const;
      
      bgr();
      bgr(const math::vec3& );
      
    };
    

  }

}


#endif
