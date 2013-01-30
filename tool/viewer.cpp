#include "viewer.h"

#include <QKeyEvent>
#include <core/share.h>

#include <gui/viewer/text.h>

#include <gl/init.h>

#include <QApplication>

#include <script/meuh.h>

#include <core/stl.h>
#include <phys/unit.h>

#include <gui/convert.h>

#include <core/log.h>
#include <script/call.h>

#include <core/macro/derp.h>

#include <gui/fix.h>
#include <core/debug.h>

// #include <gl/clear.h>

namespace tool {
  
  void viewer::saveAnimation() {
    animation_state = animationIsStarted();
  }

  void viewer::restoreAnimation() {
    if(animation_state) this->startAnimation();
    else this->stopAnimation();
  }

  viewer::~viewer() { }

  qglviewer::Vec convert(const math::vec3& v) { return qglviewer::Vec(v.x(), v.y(), v.z()); }
  math::vec3 convert(const qglviewer::Vec& v) { return math::vec3(v.x, v.y, v.z); }

  math::real viewer::time() const { 
    throw std::logic_error("viewer::time not implemented");
  }
  

  void viewer::initializeGL() {
    ::gl::init();

    setMouseTracking( true );
    QGLViewer::initializeGL();
    script::exec("hook(viewer.init)");

  }

  static QWidget* toplevel(QWidget* wid) {
    QWidget* res = wid;
    
    while(res->parentWidget()) {
      res = res->parentWidget();
    }
    
    return res;
  }
  

  void viewer::push(const core::callback& f) {
    std::unique_lock<std::mutex> lock(mutex);
    todo.push_back( f );
  }

  void viewer::flush(bool) {
    std::unique_lock<std::mutex> lock(mutex);
    core::each(todo, [&](const core::callback& f) { f(); } );
    todo.clear();
  }
  

  
  void viewer::resize(unsigned int w, unsigned int h) {
    
    size_hint = QSize(w, h);
    updateGeometry();
    toplevel(this)->adjustSize();

  }
  
  QSize viewer::sizeHint() const { return size_hint; }
  

  GLuint viewer::texture(const std::string& relative, GLenum target) {
    QPixmap pixmap(core::share("/texture/" + relative).c_str());
    
    return bindTexture( pixmap, target );
  }
  

  viewer::viewer(QWidget* parent)
    : QGLViewer(parent),
      api("viewer"),
      size_hint(640, 400)
  {
    api
      .fun("animate", [&](bool value) { 
	  if( value ) { 
	    startAnimation();
	  } else {
	    stopAnimation();
	  }
	})
      .fun("snapshot", [&](const std::string& where) { snapshot(where); } )
      .fun("snapshot_next", [&] { snapshot_next(); } )
      .fun("text", [&](const std::string& v) { text(v); } )
      .fun("time", [&]{ return time(); } )
      .fun("resize", [&](math::natural w, math::natural h) { 
	  push([this, w, h]{ resize(w, h); } );
	})
      .fun("fps", [&](math::natural n) {
	  saveAnimation();
	  stopAnimation();
	  setAnimationPeriod( (phys::unit::s / math::real(n) ) / phys::unit::ms );
	  
	  restoreAnimation();
	})
      .fun("title", [&](const std::string& x) {
	  setWindowTitle(x.c_str());
	})
      .fun("axis", [&](bool b) {
	  setAxisIsDrawn( b );
	})
      .fun("update", [&] { this->updateGL(); } )
      .fun("straighten", [&] {
	  camera()->setUpVector( gui::convert(math::vec3::UnitY()));
	})
      .fun("restore", [&] {
	  restoreStateFromFile();
	})
      .fun("radius", [&](math::real r) {
	  setSceneRadius( r );
	});
    ;
      
      // .fun("ambient", [&](math::real x) { gl::light::model::ambient( 0.4 * gl::white()); }
      ;

    // connect(this, SIGNAL( resize_needed(unsigned int, unsigned int)),
    // 	    this, SLOT( resize(unsigned int, unsigned int) ) );

    connect(this, SIGNAL( drawFinished(bool)),
	    this, SLOT( flush(bool)));
    
    // adds lua-side functionalities
    script::require("viewer");  
    script::require("key");
  }

  void viewer::arrow(const math::vec3& from, 
		     const math::vec3& to,
		     math::real radius ,
		     unsigned int subdiv ) {
    drawArrow(convert(from), convert(to),
	      radius, subdiv);
  }


  void viewer::keyPressEvent(QKeyEvent* e) {
    const std::string s = e->text().toStdString();
    macro_debug("key:", s);
    try{
      if( ! script::call<bool>("key_press", s) )  QGLViewer::keyPressEvent(e);
    } catch( const luabind::error& e ) {
      core::log( script::the()->pop() );
    }

  }
  



  void viewer::postDraw() {
    // need direct lua access to perform drawing in the same thread
    script::exec("hook(viewer.draw)");
    QGLViewer::postDraw();
  }
  
  void viewer::snapshot(const std::string& where) {
    setSnapshotCounter( 1 ); // for ffmpeg to produce movie from jpegs
    setSnapshotQuality( 100 );
    setSnapshotFileName(where.c_str()); 
  }

  void viewer::snapshot_next() {
    connect(this, SIGNAL(drawFinished(bool)), this, SLOT(snapshot_frame(bool)));
  }
  
  void viewer::snapshot_frame(bool value) {
    saveSnapshot( value );
    disconnect(this, SIGNAL(drawFinished(bool)), this, SLOT(snapshot_frame(bool)));
  }

  void viewer::drawWithNames() { 
    ::gl::clear(GL_DEPTH_BUFFER_BIT);
    glInitNames();


    picker.draw(); 
  }
  
  // manipulation
  struct grabber : qglviewer::MouseGrabber {
    tool::pickable* p;
    math::real z;

    void wake(tool::pickable* pp, math::real zz) {
      p = pp;
      z = zz;
      addInMouseGrabberPool();
    }
    
    void checkIfGrabsMouse(int , int , const qglviewer::Camera* const) { 
      setGrabsMouse( p ); 
    }
    
    void mouseMoveEvent(QMouseEvent* const e, qglviewer::Camera* const camera ) {
      // core::log("got mousemove event");

      if(!p) return;
      
      qglviewer::Vec point(e->x(), e->y(), z);
      math::vec3 world = gui::convert( camera->unprojectedCoordinatesOf( point ) );
      p->move( world );
    }
    
    void mouseReleaseEvent(QMouseEvent* const, qglviewer::Camera* const) {
      p->end();
      p = 0;
      // core::log("got mouserelease event");
      
    }
    
  };

  std::unique_ptr< grabber > grabber_instance;

  static grabber* grab() { 
    if( ! grabber_instance ) grabber_instance.reset( new grabber );
    return grabber_instance.get();
  }

  void viewer::postSelection(const QPoint& point) {
    bool found;
    qglviewer::Vec selectedPoint = camera()->pointUnderPixel(point, found);
    
    if(!found) return;
    
    if( selectedName() <  0 ) {
      // this should not happen ? 
      core::log( DERP );
      return;
    }

    // core::log("picked", selectedName() );

    // local selection coords
    math::real dist = camera()->projectedCoordinatesOf(selectedPoint).z;
	    
    // z-buffer entry
    const qglviewer::Vec loc(point.x(), point.y(), dist);
	    
    // unproject
    const qglviewer::Vec target = camera()->unprojectedCoordinatesOf(loc);
    const math::vec3 abs = gui::convert( target );

    // checks picking 
    auto& p = picker.at( selectedName() );

    // pick callback
    const math::vec3 corrected = p.pick( abs );

    // updates projected point
    dist = camera()->projectedCoordinatesOf( gui::convert(corrected) ).z;
    
    // wake up our trusty grabber to deal with mouse moves
    grab()->wake(&p, dist);
  }


  void viewer::text(const std::string& string) {
    const QFont font("Ubuntu", height() / 15);
    gui::viewer::text(string.c_str(), font).draw( this );
    
  };
  

  // int console_app(int argc, char** argv, std::function< viewer*() > make_viewer) {
  //   QApplication app(argc, argv);

  //   viewer* wid = make_viewer();
  //   wid->show();
    

  //   std::thread t( script::console(wid->script()) );
    
  //   return app.exec();
  // }
}
