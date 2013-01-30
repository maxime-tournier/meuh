#include <plugin/kinect.h>

#include <core/log.h>
#include <core/error.h>

#include <image/format.h>

#include <map>

#include <libfreenect.h>
#include <libfreenect-registration.h>

#include <thread>

struct kinect : plugin::kinect {

// raii for init/shutdown libfreenect
  struct libfreenect {
    freenect_context* context;

    std::thread thread;
    bool quit;
    
    // // maps 11bits in ts to distance
    // double gamma[2048];
    
    libfreenect() : context(0), quit( false ) {
      if( freenect_init(&context, NULL) < 0) throw core::error("freenect_init() failed");
    
      init();

      thread = std::thread([&] { loop(); } );
    }
    
    
    void init() {
      freenect_set_log_level(ctx(), FREENECT_LOG_DEBUG);
      freenect_select_subdevices(ctx(), (freenect_device_flags)(FREENECT_DEVICE_MOTOR | FREENECT_DEVICE_CAMERA));

      // for(int i = 0; i < 2048; ++i) {
      // 	const double k1 = 1.1863; 
      //   const double k2 = 2842.5; 
      //   const double k3 = 0.1236; 
      //   const double depth = k3 * std::tan(i/k2 + k1); 
      //   gamma[i] = depth; 
      // }
     
    }
    
    
    void loop() {
      // 60 msec timeout
      timeval timeout = {0, 60};
      
      while( !quit ) {
	while( freenect_process_events_timeout( ctx(), &timeout ) >= 0 ) {
	  // core::log("processing events");
	}
      }
      
    };

    freenect_context* ctx() { 
      assert( context );
      return context; 
    }
    
    ~libfreenect() { 
      
      quit = true;
      thread.join();
      
      freenect_shutdown( ctx() );
    }
    
  };

  static libfreenect lib;
  
  freenect_device *device;

  static std::map<freenect_device*, depth_callback> depth_cb;
  static std::map<freenect_device*, rgb_callback> rgb_cb;
  static std::map<freenect_device*, ir_callback> ir_cb;
  
  kinect() 
    : device( 0 )
  { 
    
  }

  ~kinect() { };
  
  
  static void sub_devices( freenect_device_flags flags ) {
    freenect_select_subdevices(lib.ctx(), flags);
  }

  virtual dev_type dev_count() const {
    return freenect_num_devices( lib.ctx() );
  }
  
  virtual void open(dev_type id) {
    if( device ) throw core::error("dont open twice lol");
    if (freenect_open_device(lib.ctx(), &device, id) < 0) throw core::error("could not open device");
  }

  virtual void close() {
    freenect_close_device( dev() );
  }
  
  freenect_device* dev() const { 
    if( !device ) throw core::error("no device opened");
    return device; 
  }
  
  virtual void tilt(math::real deg) {
    freenect_set_tilt_degs(dev(), deg);
  }

  
  virtual void start_depth(const depth_callback& cb) {
    depth_cb[ dev() ] = cb;
    freenect_set_depth_callback(dev(), c_depth_cb);
    freenect_set_depth_mode( dev(), freenect_find_depth_mode(FREENECT_RESOLUTION_MEDIUM, FREENECT_DEPTH_REGISTERED));
    freenect_start_depth( dev() );
  }

  virtual void stop_depth() {
    freenect_stop_depth( dev() );
    freenect_set_depth_callback(dev(), 0);
    depth_cb.erase( dev() );
  }

  virtual void start_rgb(const rgb_callback& cb) {
    // disable IR if needed
    if(  ir_cb.find( dev() ) != ir_cb.end() ) stop_ir();
    
    rgb_cb[ dev() ] = cb;

    freenect_set_video_callback(dev(), c_rgb_cb);
    freenect_set_video_mode(dev(), freenect_find_video_mode(FREENECT_RESOLUTION_MEDIUM, FREENECT_VIDEO_RGB));
    freenect_start_video( dev() );
  }

  virtual void stop_rgb() {
    freenect_stop_video( dev() );
    freenect_set_video_callback(dev(), 0);
    rgb_cb.erase( dev() );
  }


  virtual void start_ir(const ir_callback& cb) {
    // disable RGB if needed
    if(  rgb_cb.find( dev() ) != rgb_cb.end() ) stop_rgb();
    
    ir_cb[ dev() ] = cb;
    
    freenect_set_video_callback(dev(), c_ir_cb);
    freenect_set_video_mode(dev(), freenect_find_video_mode(FREENECT_RESOLUTION_MEDIUM, FREENECT_VIDEO_IR_8BIT));
    freenect_start_video( dev() );
  }

  virtual void stop_ir() {
    freenect_stop_video( dev() );
    freenect_set_video_callback(dev(), 0);
    ir_cb.erase( dev() );
  }
  
  
  static void c_depth_cb(freenect_device *dev, void *v_depth, uint32_t timestamp) {
    // core::log("depth!");
    auto it = kinect::depth_cb.find( dev );
    if(it != kinect::depth_cb.end() ) {
      
      uint16_t *depth = (uint16_t*)v_depth;

      it->second( depth_view(depth), timestamp );
    }
  }

  static void c_rgb_cb(freenect_device *dev, void *rgb, uint32_t timestamp) {
    // core::log("rgb!");
    auto it = kinect::rgb_cb.find( dev );
    if(it != kinect::rgb_cb.end() ) {
      
      image::format::rgb* color = (image::format::rgb*)rgb;

      it->second( rgb_view(color), timestamp );
    }
  }

  static void c_ir_cb(freenect_device *dev, void *ir, uint32_t timestamp) {
    auto it = kinect::ir_cb.find( dev );
    if(it != kinect::ir_cb.end() ) {
      
      unsigned char* color = (unsigned char*)ir;
      ir_map map;
      
      // TODO parallel ?
      map.each([&](math::natural i, math::natural j) {
	  const auto& c = color[ 640 * j + i ];
	  map(i, j) = c / 256.0;
	});
      
      it->second( map, timestamp );
    }
  }
  

  void start_pcl(const pcl_callback& pcl_cb) {
    
    start_depth([&,pcl_cb](const depth_view& depth, timestamp t) {
	pcl_map map;
	
	map.each( [&](math::natural i, math::natural j) {
	    double x, y;
	    freenect_camera_to_world(dev(), i, j, depth(i, j), &x, &y);
	    
	    map(i, j) = math::vec3(x, y, depth(i, j) ) / 1000.0;
	    // core::log(map(i, j).transpose() );
	  });

	pcl_cb( map, t );
	
      });
    
  }


  virtual void init()  { } 
  
};


kinect::libfreenect kinect::lib;

std::map<freenect_device*, kinect::depth_callback> kinect::depth_cb;
std::map<freenect_device*, kinect::rgb_callback> kinect::rgb_cb;
std::map<freenect_device*, kinect::ir_callback> kinect::ir_cb;


static const plugin::loader<plugin::kinect> loader( [] { return new kinect; } );
