#include "ground.h"

namespace tool {


  static gl::texture chess;
  
  void ground::init(GLuint texture) {
    chess.gen( texture ).nearest().unbind();
  }
  
  void ground::draw() const {
    using namespace gl;
    matrix::safe([&]{
	color( grey() );
	translate(model.geo.origin());
	math::quat q;
	q.setFromTwoVectors(math::vec3::UnitZ(), model.geo.normal() );
	rotate( q );
	
	math::real tile_size = 200000;
	scale( tile_size );

	
	chess.bind();
	matrix::mode(GL_TEXTURE);
	matrix::push();
	scale( int(tile_size) >> 4);
	quad();
	matrix::pop();
	matrix::mode(GL_MODELVIEW);
	
	
	quad(GL_QUADS);
	chess.unbind();
      });
    
  }



}


