#ifndef PLUGIN_KINECT_H
#define PLUGIN_KINECT_H

#include <plugin/base.h>

#include <math/mat.h>
#include <math/vec.h>

#include <image/format.h>

namespace plugin {

  
  struct kinect : base {
    
    typedef math::natural dev_type;
    
    virtual void init() = 0;
    
    virtual dev_type dev_count() const = 0;
    virtual void open(dev_type ) = 0;
    virtual void close() = 0;
    
    // TODO leds ?
    
    // open a specific device

    virtual void tilt(math::real deg) = 0;


    typedef uint32_t timestamp;
    
    // TODO handle resolution ?
    typedef math::matrix<uint16_t, 640, 480> depth_map; // in milimmeters
    typedef Eigen::Map< depth_map > depth_view;
    
    typedef math::matrix<image::format::rgb, 640, 480> rgb_map;
    typedef Eigen::Map< rgb_map > rgb_view;
    
    typedef math::matrix< math::real, 640, 480 > ir_map;    // grayscale in 0..1
    
    typedef math::matrix< math::vec3, 640, 480 > pcl_map;  // point-cloud, in meters

    typedef std::function< void(const depth_view&, timestamp) > depth_callback;
    virtual void start_depth(const depth_callback& ) = 0;
    virtual void stop_depth() = 0;
    
    typedef std::function< void(const rgb_view&, timestamp) > rgb_callback;
    virtual void start_rgb(const rgb_callback& ) = 0;
    virtual void stop_rgb() = 0;
    
    typedef std::function< void(const ir_map&, timestamp) > ir_callback;
    virtual void start_ir(const ir_callback& ) = 0;
    virtual void stop_ir() = 0;
    
    typedef std::function< void(const pcl_map&, timestamp) > pcl_callback;
    
    // stop using stop_depth
    virtual void start_pcl(const pcl_callback& ) = 0;
  };

  


}


#endif
