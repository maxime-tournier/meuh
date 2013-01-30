#include "obj.h"

#include <math/vec.h>
#include <geo/mesh.h>
#include <geo/obj/file.h>

#include <core/stl.h>

namespace geo {
  namespace obj {

    void debug(const file& data) {
      using namespace std;
      cout << "vertices: " << data.vertices.size() << endl
	   << "texcoords: " << data.texcoords.size() << endl
	   << "normals: " << data.normals.size() << endl
	   << "faces: " << data.faces.size() << endl;

    }


    // vbo::data* vbo(const file& data) {
      
    //   typedef array<math::vec3, math::vec3> vbo_type;
    //   vbo_type* res = new vbo_type( data.vertices.size() );
    //   res->layout().vertex<0>().normal<1>();
      
    //   auto vertex = res->access<0>();
    //   auto normal = res->access<1>();

    //   // vertices
    //   for(unsigned int i = 0; i < data.vertices.size(); ++i) {
    // 	vertex(i) = math::vec3(data.vertices[i].x, 
    // 			       data.vertices[i].y, 
    // 			       data.vertices[i].z);
    // 	normal(i) = math::vec3::Zero();
    //   }

    //   // normals: average normals to each face sharing vertex
    //   using namespace core;
    //   foreach(all(data.faces), [&](const obj::face& f) {
    // 	  if( f.normals.empty() ) return;
    // 	  assert(f.normals.size() == f.vertices.size() );

    // 	  for(unsigned int i = 0; i < f.vertices.size(); ++i) {
    // 	    unsigned int v = f.vertices[i] - 1; // vertex index
    // 	    unsigned int n = f.normals[i] - 1; // normal index
	    
    // 	    normal( v ) += math::vec3( data.normals[n].x, 
    // 				       data.normals[n].y, 
    // 				       data.normals[n].z);
    // 	  }
	  
    // 	});
      
    //   // normalize normals ^_^
    //   for(unsigned int i = 0; i < data.vertices.size(); ++i) {
    // 	normal( i ) = normal( i ).normalized();
    //   }

      
    //   return res;
    // }

    // gl::ibo* faces(const file& data, unsigned int size) {
    //   typedef gl::element_array<> ibo_type;
      
      
    //   std::vector< obj::face > faces;

    //   using namespace core;
    //   foreach( all(data.faces),
    // 	       [&](const obj::face& f) {
    // 		 if( f.vertices.size() == size) faces.push_back(f);
    // 	       } );
      
    //   if( faces.empty() ) {
    // 	std::cout << "no " << size << "-faces!" << std::endl;
    // 	return 0;
    //   }
      
    //   ibo_type* res = new ibo_type;
    //   res->data.resize( size * faces.size() );
      
    //   unsigned int off = 0;
    //   foreach(all(faces), [&](const obj::face& f) {
    // 	  std::cout << "face: ";
    // 	  for(unsigned int i = 0; i < size; ++i){
    // 	    std::cout << f.vertices[i] << " ";
    // 	    res->data[ off++ ] = f.vertices[i] - 1;
    // 	  }
    // 	  std::cout << std::endl;
    // 	});
      
    //   assert( off == res->data.size());
      
    //   return res;
    // }


    static math::vector< geo::vertex > vertices( const file& f) {
      math::vector< geo::vertex > res;
      res.resize( f.vertices.size() );
      
      unsigned int i = 0;
      core::each(f.vertices, [&](const obj::vertex& v) {
	  res(i++) = geo::vertex( v.x, v.y, v.z );
	});
      
      return std::move(res);
    }


    static math::vector< geo::normal > normals( const file& f) {
      math::vector< geo::normal > res;
      res.resize( f.normals.size() );
      
      unsigned int i = 0;
      core::each(f.normals, [&](const obj::normal& v) {
	  res(i++) = geo::normal( v.x, v.y, v.z );
	});
      
      return std::move(res);
    }

    static math::vector< geo::texcoord > texcoords( const file& f) {
      math::vector< geo::texcoord  > res;
      res.resize( f.texcoords.size() );
      
      unsigned int i = 0;
      core::each(f.texcoords, [&](const obj::texcoord& t) {
	  res(i++) = geo::texcoord( t.u, t.v );
	});
      
      return std::move(res);
    }

    
    template<int N>
    static std::vector< mesh::face<N> > faces(const file& f) {
      
      std::vector< mesh::face<N> > res;
      
      core::each(f.faces, [&](const obj::face& x) {
	  assert( x.ok() );

	  // only deal with N-faces
	  if( x.vertices.size() != N ) return;
	  
	  mesh::face<N> tmp;

	  // obj indices start at 1 !
	  for( int i = 0; i < N; ++i) {
	    tmp.vertex[i] = x.vertices[i] - 1;
	    
	    tmp.normal[i] = x.normals.empty() ? -1 : x.normals[i] - 1;
	    tmp.texcoord[i] = x.texcoords.empty() ? -1 : x.texcoords[i] - 1;
	  }

	  res.push_back( tmp );
	  
	});
      
      return res;
    }
    

    geo::mesh mesh(const file& data) {
      
      // debug(data);

      geo::mesh res;
      
      res.vertices = vertices( data );
      res.texcoords = texcoords( data );
      res.normals = normals( data );

      res.triangles = faces<3>( data );
      res.quads = faces<4>( data );
      
      // std::cout << "tris: " << res.triangles.size() << std::endl;
      // std::cout << "quads: " << res.quads.size() << std::endl;
      
      return res;
    }

    geo::mesh load(const std::string& filename) {
      return mesh( file( filename ) );
    }

  }
}
