#include "vbo_viewer.h"

#include <math/vec.h>
#include <gl/init.h>
#include <gl/common.h>

#include <gl/obj/file.h>
#include <gl/obj.h>

#include <gl/sphere.h>
#include <gl/cube.h>


namespace gui {


  vbo_viewer::vbo_viewer(QWidget*  parent ) 
    : QGLViewer( parent ) {
  
  }
  

  struct vec {
    double x, y, z;
  };

  void vbo_viewer::init() {

    gl::init();
    gl::point_size(10);

    setSceneRadius(10);
    
    
     
  }


  void vbo_viewer::load(const QString& file) {
    gl::obj::file obj(file.toStdString());
    
    mesh = gl::obj::mesh(obj);
    
  }

  void vbo_viewer::draw() {

    // gl::disable(GL_LIGHTING);
    gl::color(1, 1, 1);
    
    mesh.draw();
    
    // gl::enable(GL_LIGHTING);
    
  }


}
