#include "obj.h"

#include <gl/mesh.h>
#include <gl/obj/file.h>
#include <math/vec.h>

namespace gl {
  namespace obj {

    void debug(const file& data) {
      using namespace std;
      cout << "vertices: " << data.vertices.size() << endl
	   << "texcoords: " << data.texcoords.size() << endl
	   << "normals: " << data.normals.size() << endl
	   << "faces: " << data.faces.size() << endl;

    }


    vbo::data* vbo(const file& data) {
      
      typedef array<math::vec3, math::vec3> vbo_type;
      vbo_type* res = new vbo_type( data.vertices.size() );
      res->layout().vertex<0>().normal<1>();
      
      auto vertex = res->access<0>();
      auto normal = res->access<1>();

      // vertices
      for(unsigned int i = 0; i < data.vertices.size(); ++i) {
	vertex(i) = math::vec3(data.vertices[i].x, 
			       data.vertices[i].y, 
			       data.vertices[i].z);
	normal(i) = math::vec3::Zero();
      }

      // normals: average normals to each face sharing vertex
      using namespace core;
      foreach(all(data.faces), [&](const obj::face& f) {
	  if( f.normals.empty() ) return;
	  assert(f.normals.size() == f.vertices.size() );

	  for(unsigned int i = 0; i < f.vertices.size(); ++i) {
	    unsigned int v = f.vertices[i] - 1; // vertex index
	    unsigned int n = f.normals[i] - 1; // normal index
	    
	    normal( v ) += math::vec3( data.normals[n].x, 
				       data.normals[n].y, 
				       data.normals[n].z);
	  }
	  
	});
      
      // normalize normals ^_^
      for(unsigned int i = 0; i < data.vertices.size(); ++i) {
	normal( i ) = normal( i ).normalized();
      }

      
      return res;
    }

    gl::ibo* faces(const file& data, unsigned int size) {
      typedef gl::element_array<> ibo_type;
      
      
      std::vector< obj::face > faces;

      using namespace core;
      foreach( all(data.faces),
	       [&](const obj::face& f) {
		 if( f.vertices.size() == size) faces.push_back(f);
	       } );
      
      if( faces.empty() ) {
	std::cout << "no " << size << "-faces!" << std::endl;
	return 0;
      }
      
      ibo_type* res = new ibo_type;
      res->data.resize( size * faces.size() );
      
      unsigned int off = 0;
      foreach(all(faces), [&](const obj::face& f) {
	  std::cout << "face: ";
	  for(unsigned int i = 0; i < size; ++i){
	    std::cout << f.vertices[i] << " ";
	    res->data[ off++ ] = f.vertices[i] - 1;
	  }
	  std::cout << std::endl;
	});
      
      assert( off == res->data.size());
      
      return res;
    }



    geo::mesh mesh(const file& data) {
      
      debug(data);

      geo::mesh res;
      
      // res.vertices.reset( obj::vbo(data) );
      // res.triangles.reset( obj::faces(data, 3) );
      // res.quads.reset( obj::faces(data, 4) );
      
      return res;
    }






  }
}
