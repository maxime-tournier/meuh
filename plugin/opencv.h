#ifndef PLUGIN_OPENCV_H
#define PLUGIN_OPENCV_H

#include <plugin/base.h>

#include <math/mat.h>
#include <math/vec.h>

#include <image/rgb.h>
#include <image/gray.h>

namespace plugin {

  struct opencv : base  {
    
    virtual void load(image::rgb&, const std::string& name) = 0;
    virtual void load(image::gray&, const std::string& name) = 0;
    
    virtual void save(const std::string& name, const image::rgb& ) = 0;
    virtual void save(const std::string& name, const image::gray& ) = 0;
    
  };
  
}


#endif
