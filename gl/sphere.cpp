#include "sphere.h"

#include <gl/vbo.h>

#include <core/stl.h>
#include <math/vec.h>

namespace gl {

  

  struct isocaedron {
    
    static const math::real x;
    static const math::real z;
    
    typedef math::vec3 vertex;

    struct face {
      unsigned int a, b, c;
    };
  
     
    std::vector< vertex > vertices;
    std::vector< face > faces;
    
    isocaedron() 
    {
      vertices = { 
	{-x, 0.0, z}, 
	{x, 0.0, z },
	{-x, 0.0, -z}, 
	{x, 0.0, -z},
	{0.0, z, x}, 
	{0.0, z, -x}, 
	{0.0, -z, x},
	{0.0, -z, -x},
	{z, x, 0.0}, 
	{-z, x, 0.0},
	{z, -x, 0.0}, 
	{-z, -x, 0.0} };
      
      faces  = {
	{0,4,1}, {0,9,4}, {9,5,4}, {4,5,8}, {4,8,1},    
	{8,10,1}, {8,3,10}, {5,3,8}, {5,2,3}, {2,7,3},    
	{7,10,3}, {7,6,10}, {7,11,6}, {11,0,6}, {0,1,6}, 
	{6,1,10}, {9,0,11}, {9,11,2}, {9,2,5}, {7,2,11} 
      };
      
    }

  };

  const math::real isocaedron::x = .525731112119133606; 
  const math::real isocaedron::z = .850650808352039932;

  
  template<class Action>
  unsigned int subdivide(const isocaedron::vertex& a,
			 const isocaedron::vertex& b,
			 const isocaedron::vertex& c,
			 unsigned int div, 
			 unsigned int offset,
			 const Action& action) {
    
    if( div == 0 ) {
      action(a, b, c, offset);
      return offset + 3;
    } else {
      isocaedron::vertex 
	ab = ((a + b) /2 ).normalized(),
	ac = ((a + c) /2 ).normalized(),
	bc = ((b + c) /2 ).normalized();
      
      offset = subdivide(a, ab, ac, div - 1, offset, action);
      offset = subdivide(b, bc, ab, div - 1, offset, action);
      offset = subdivide(c, ac, bc, div - 1, offset, action);
      offset = subdivide(ab, bc, ac, div - 1, offset, action);

      return offset;
    }
  }

  
  std::unique_ptr<vbo::data_type> sphere(math::real radius, unsigned int div) {
    
    const unsigned int n_triangles = 20 * (unsigned int)(std::pow(4, div)) ;
    const unsigned int n_vertices = 3 * n_triangles;

    typedef array<math::vec3, math::vec3> res_type; 
    res_type* res = new res_type(n_vertices);
    res->layout().vertex<0>().normal<1>();
    
    auto vertex = res->access<0>();
    auto normal = res->access<1>();
    
    isocaedron iso;
    unsigned int offset = 0;
    
    core::each( iso.faces, [&](isocaedron::face f) {
	isocaedron::vertex 
	  a = iso.vertices[f.a],
	  b = iso.vertices[f.b],
	  c = iso.vertices[f.c];
	
	offset = subdivide( a, b, c, div, offset, 
			    [&]( const isocaedron::vertex& a,
				 const isocaedron::vertex& b,
				 const isocaedron::vertex& c,
				 unsigned int off) {
			      
			      vertex(off) = radius * a;
			      vertex(off + 1) = radius * c;
			      vertex(off + 2) = radius * b;

			      normal(off) = a;
			      normal(off + 1) = c;
			      normal(off + 2) = b;
			    } );
	
      } );
    
    return std::unique_ptr<vbo::data_type>(res);
  }



}
