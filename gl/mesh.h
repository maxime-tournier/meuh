#ifndef GL_MESH_H
#define GL_MESH_H

#include <gl/vbo.h>
#include <memory>

namespace geo {
  struct mesh;
}


namespace gl {


  class mesh {
    
    struct chunk {
      std::unique_ptr<gl::vbo::data_type> data;
      gl::vbo vbo;

      chunk& operator=(chunk&& );
      chunk( chunk&& );
      chunk();
      
    } tris, quads;
    
  public:
    
    mesh( const geo::mesh& m);
    mesh(const mesh& ) = delete;
    mesh(mesh && ) = default;
    
    mesh() = default;
    mesh& operator=(mesh&& );
    
    void init();
    void draw();
    
  };
  

}


#endif
