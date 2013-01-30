#include "light.h"
#include <gl/enable.h>
#include <math/vec.h>


namespace gl {

  using namespace math;
  
  light::light(GLenum id) : id (id) { }

  const light& light::enable() const {
    gl::enable(id);
    return *this;
  }
  
  const light& light::disable() const {
    gl::disable(id);
    return *this;
  }
      
  const light& light::ambient(const vec3& v) const {
    vector<GLfloat, 4> tmp;
    tmp.head<3>() = v.cast<GLfloat>();
    tmp(3) = 1.0;
    
    glLightfv(id, GL_AMBIENT, tmp.data() );
    return *this;
  }
  
  const light& light::diffuse(const vec3& v) const {
    const vector<GLfloat, 3> tmp = v.cast<GLfloat>();
    glLightfv(id, GL_DIFFUSE, tmp.data() );
    return *this;
  }
  
  const light& light::specular(const vec3& v) const {
    const vector<GLfloat, 3> tmp = v.cast<GLfloat>();
    glLightfv(id, GL_SPECULAR, tmp.data() );
    return *this;
  }
  
  const light& light::position(const vec3& v) const {
    vector<GLfloat, 4> tmp;
    tmp.head<3>() = v.cast<GLfloat>();
    tmp(3) = 1;
    
    glLightfv(id, GL_POSITION, tmp.data() );
    return *this;
  }

  const light& light::direction(const vec3& v) const {
    vector<GLfloat, 4> tmp;
    tmp.head<3>() = v.cast<GLfloat>();
    tmp(3) = 0;
    
    glLightfv(id, GL_POSITION, tmp.data() );
    return *this;
  }

  light light::get(math::natural i ) {
    return { GL_LIGHT0 + i };
  }

  void light::model::ambient(const math::vec3& v) {
    vector<GLfloat, 4> tmp;
    tmp.head<3>() = v.cast<GLfloat>();
    tmp(3) = 1.0;

    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, tmp.data());
  }


}
