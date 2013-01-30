#ifndef IMAGE_CONV_H
#define IMAGE_CONV_H

#include <image/rgb.h>
#include <image/rgba.h>
#include <image/gray.h>


namespace image {

  void convert(rgb&, const gray& );
  void convert(gray&, const rgb& );
  
}


#endif
