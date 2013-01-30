#include "raii.h"

#include <gl/enable.h>
#include <gl/begin.h>
#include <gl/matrix.h>


namespace gl {
  namespace raii {


    matrix::matrix() { gl::matrix::push(); }
    matrix::~matrix() {gl::matrix::pop(); }
    
    enable::enable(GLenum what) 
    : what(what) {
      gl::enable(what);
    }

    enable::~enable() {
      gl::disable(what);
    }

    disable::disable(GLenum what) 
      : what(what) {
      gl::disable(what);
    }

    disable::~disable() {
      gl::enable(what);
    }

    begin::begin(GLenum mode) {
      glBegin( mode );
    }

    begin::~begin() { glEnd(); }

  }
}
