#ifndef OPENCV_H
#define OPENCV_H

#include <plugin/opencv.h>

struct opencv : plugin::opencv {

  virtual void load(image::rgb&, const std::string& name);
  virtual void load(image::gray&, const std::string& name);
    
  virtual void save(const std::string& name, const image::rgb& );
  virtual void save(const std::string& name, const image::gray& );
  
};

#endif
