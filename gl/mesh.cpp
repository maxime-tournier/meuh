#include "mesh.h"

#include <geo/mesh.h>

#include <gl/vbo.h>

#include <core/stl.h>


// delicious copypasta. TODO fix
namespace gl {

  namespace tris {
    
    // vertex texcoord normal
    std::unique_ptr<gl::vbo::data_type> vertex_texcoord_normal(const geo::mesh& m) {
      if(m.triangles.empty()) return 0;
    
      typedef gl::array< geo::vertex, geo::normal, geo::texcoord > data_type;
      
      data_type* data = new data_type(3 * m.triangles.size()) ;
      data->layout().vertex<0>().normal<1>().texcoord<2>();
    
      auto vertex = data->access<0>();
      auto normal = data->access<1>();
      auto texcoord = data->access<2>();
    
      unsigned int t = 0;
      core::each( m.triangles, [&](const geo::mesh::face<3>& tri ) {
	
	  for(unsigned int i = 0; i < 3; ++i) {
	  
	    vertex(3*t + i) = m.vertices( tri.vertex[i] );
	    texcoord(3*t + i) = m.texcoords( tri.texcoord[i] );
	    normal(3*t + i) = m.normals( tri.normal[i] );
	  
	  }
	
	  ++t;
	});

      return std::unique_ptr<vbo::data_type>(data);
    }
  
    
    // vertex normal
    std::unique_ptr<gl::vbo::data_type> vertex_normal(const geo::mesh& m) {
      if(m.triangles.empty() ) return 0;
    
      typedef gl::array< geo::vertex, geo::normal > data_type;
    
      data_type* data = new data_type(3 * m.triangles.size()) ;
      data->layout().vertex<0>().normal<1>();
    
      auto vertex = data->access<0>();
      auto normal = data->access<1>();
    
      unsigned int t = 0;
      core::each( m.triangles, [&](const geo::mesh::face<3>& tri ) {
	
	  for(unsigned int i = 0; i < 3; ++i) {
	  
	    vertex(3*t + i) = m.vertices( tri.vertex[i] );
	    normal(3*t + i) = m.normals( tri.normal[i] );
	  
	  }
	
	  ++t;
	});

      return std::unique_ptr<vbo::data_type>(data);
    }
  
    // vertex texcoord
    std::unique_ptr<vbo::data_type> vertex_texcoord(const geo::mesh& m) {
      if(m.triangles.empty() ) return 0;
    
      typedef gl::array< geo::vertex, geo::texcoord > data_type;
    
      data_type* data = new data_type(3 * m.triangles.size()) ;
      data->layout().vertex<0>().texcoord<1>();
    
      auto vertex = data->access<0>();
      auto texcoord = data->access<1>();
    
      unsigned int t = 0;
      core::each( m.triangles, [&](const geo::mesh::face<3>& tri ) {
	
	  for(unsigned int i = 0; i < 3; ++i) {
	  
	    vertex(3*t + i) = m.vertices( tri.vertex[i] );
	    texcoord(3*t + i) = m.texcoords( tri.texcoord[i] );
	    
	  }
	  
	  ++t;
	});
      
      return std::unique_ptr<vbo::data_type>(data);
    }

  }


  namespace quads { 

    std::unique_ptr<vbo::data_type> vertex_texcoord_normal(const geo::mesh& m) {
      if(m.quads.empty()) return 0;
    
      typedef gl::array<  geo::vertex, geo::normal, geo::texcoord  > data_type;
    
      data_type* data = new data_type(4 * m.quads.size()) ;
      data->layout().vertex<0>().normal<1>().texcoord<2>();
    
      auto vertex = data->access<0>();
      auto normal = data->access<1>();
      auto texcoord = data->access<2>();

      unsigned int q = 0;
      core::each( m.quads, [&](const geo::mesh::face<4>& quad ) {
	
	  for(unsigned int i = 0; i < 4; ++i) {
	  
	    vertex(4*q + i) = m.vertices( quad.vertex[i] );
	    texcoord(4*q + i) = m.texcoords( quad.texcoord[i] );
	    normal(4*q + i) = m.normals( quad.normal[i] );
	  }
	
	  ++q;
	});
    
      return std::unique_ptr<vbo::data_type>(data);
    }
  




    std::unique_ptr<vbo::data_type> vertex_normal(const geo::mesh& m) {
      if( m.quads.empty() ) return 0;
    
      typedef gl::array< geo::vertex, geo::normal > data_type;
    
      data_type* data = new data_type(4 * m.quads.size()) ;
      data->layout().vertex<0>().normal<1>();
    
      auto vertex = data->access<0>();
      auto normal = data->access<1>();
    
      unsigned int q = 0;
      core::each( m.quads, [&](const geo::mesh::face<4>& quad ) {
	
	  for(unsigned int i = 0; i < 4; ++i) {
	  
	    vertex(4*q + i) = m.vertices( quad.vertex[i] );
	    normal(4*q + i) = m.normals( quad.normal[i] );
	  
	  }
	
	  ++q;
	});
    
      return std::unique_ptr<vbo::data_type>(data);
    }
  

     // vertex texcoord
    std::unique_ptr<vbo::data_type> vertex_texcoord(const geo::mesh& m) {
      if(m.quads.empty() ) return 0;
    
      typedef gl::array< geo::vertex, geo::texcoord > data_type;
    
      data_type* data = new data_type(4 * m.quads.size()) ;
      data->layout().vertex<0>().texcoord<1>();
    
      auto vertex = data->access<0>();
      auto texcoord = data->access<1>();
    
      unsigned int q = 0;
      core::each( m.quads, [&](const geo::mesh::face<4>& quad ) {
	
	  for(unsigned int i = 0; i < 4; ++i) {
	  
	    vertex(4*q + i) = m.vertices( quad.vertex[i] );
	    texcoord(4*q + i) = m.texcoords( quad.texcoord[i] );
	    
	  }
	  
	  ++q;
	});
      
      return std::unique_ptr<vbo::data_type>(data);
    }






    
  }

  

  
 
  

  void mesh::init() {
    assert( !tris.vbo.id() );
    assert( !quads.vbo.id() );
    
    if( tris.data ) tris.vbo.send( std::move(tris.data) );
    if( quads.data) quads.vbo.send( std::move( quads.data ) );
    
    assert( tris.vbo.id() || quads.vbo.id() );
  }
  

  void mesh::draw() {
    if( !tris.vbo.id() && !quads.vbo.id() ) init();
    
    if( tris.vbo.id() ) tris.vbo.draw(GL_TRIANGLES);
    if( quads.vbo.id()) quads.vbo.draw(GL_QUADS);
    
  }
  

  mesh::mesh(const geo::mesh& m) {
    assert( m.ok() );

    if( m.texcoords.empty() ) {
      // std::cout << "no texcoords !" << std::endl;
      tris.data = gl::tris::vertex_normal(m);
      quads.data = gl::quads::vertex_normal(m);
      
    } else if( m.normals.empty() ) {
      // std::cout << "no normals !" << std::endl;
      tris.data =  gl::tris::vertex_texcoord(m);
      quads.data =  gl::quads::vertex_texcoord(m);
    } else {
      // std::cout << "texcoords & normals !" << std::endl;
      tris.data = gl::tris::vertex_texcoord_normal(m);
      quads.data = gl::quads::vertex_texcoord_normal(m);
      
    }

  }
   
  mesh::chunk::chunk() { }

  mesh::chunk::chunk(chunk&& other) 
    : data(std::move(other.data)),
      vbo(std::move(other.vbo)) {
  }

  mesh::chunk& mesh::chunk::operator=(chunk&& other) {
    if( this != &other ) {
      data = std::move(other.data);
      vbo = std::move(other.vbo);
    }
    return *this;
  }

  mesh& mesh::operator=(mesh&& other) {
    if( this != &other ) {
      tris = std::move(other.tris);
      quads = std::move(other.quads);
    }    
    return *this;
  }

  
}
