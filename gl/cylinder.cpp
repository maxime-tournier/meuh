#include "cylinder.h"

#include <gl/error.h>
#include <gl/vbo.h>
#include <math/pi.h>

#include <math/vec.h>


namespace gl {
  using namespace math;

  // includes last point
  vector< vec3 > y_circle(math::natural circ) {
    assert( circ );
    
    vector< vec3 > res;
    res.resize(circ + 1);
    
    res.each([&](natural i ) {
	const real theta = (2 * pi * i) / circ;
	res(i) = 0.5 * vec3(  std::sin(theta), 0, std::cos(theta) );
      });
    
    return res;
  } 

  // vert = subdivisions verticales
  vec heights(math::natural vert) {
    vec res;
    res.resize(2 + vert);
    const real min = -0.5, max = 0.5;
    const real stack_size = (max - min) / (res.rows() - 1);
    
    natural i = 0;
    for(real h = -0.5; h <= 0.5; h += stack_size ) {
      assert( int(i) < res.rows() );
      res(i++) = h;      
    }

    return res;
  }

  
  // vbo* make_cap(natural circ, const vec3& up) {
  //   // TODO
    
  // }


  std::unique_ptr<vbo::data_type> side_vertices(const vec& h, const vector<vec3>& points) {
    
    const natural m = h.size();
    const natural n = points.size();
    
    typedef array<math::vec3, math::vec3> vbo_type;
    vbo_type* res = new vbo_type(m * n);
    res->layout().vertex<0>().normal<1>();
    
    auto vertex = res->access<0>();
    auto normal = res->access<1>();
    
    for(natural i = 0; i < m; ++i) {
      for(natural j = 0; j < n; ++j) {
	const natural k = i * n + j;
	vertex(k) = points(j) + h(i) * vec3::UnitY();
	normal(k) = points(j).normalized();
      }
    }
    
    return std::unique_ptr<vbo::data_type>(res);
  }
  
  // m = stacks, n = circle subdiv
  std::unique_ptr<vbo::data_type> side_indices(natural m, natural n ) {
    
    const natural p = 2 * (m - 1) * (n - 1);
    
    typedef vector<unsigned short, 3> triangle;
    typedef array< triangle > vbo_type;

    vbo_type* res = new vbo_type(p);
    res->layout().element<0>();
    
    auto element = res->access<0>();

    natural k = 0;
    for(natural i = 0; i < m - 1; ++i) { // stacks
      for(natural j = 0; j < n - 1; ++j) { // sides
	const natural low_first = i * n;
	const natural up_first = low_first + n;

	const natural low_start = low_first + j;
	const natural up_start = up_first + j;
	
	element(k++) = triangle(low_start, low_start + 1,  up_start );
	element(k++) = triangle(low_start + 1, up_start + 1, up_start);
	
      }
    }
    assert( k == p );
    
    return std::unique_ptr<vbo::data_type>(res);
  }
  

  cylinder::cylinder(natural circ, natural vert ){
    auto points = y_circle(circ);
    auto heights = gl::heights(vert);
    
    side.data.vertices = side_vertices(heights, points);
    side.data.indices = side_indices(heights.rows(), points.rows()) ;
    
    side.vbo.indices = vbo( GL_ELEMENT_ARRAY_BUFFER );
  }

  void cylinder::init() {
    
    side.vbo.vertices.send( std::move(side.data.vertices) ); 
    side.vbo.indices.send( std::move(side.data.indices) );

  }
  
  void cylinder::draw() const {

    side.vbo.vertices.bind().data().layout().enable();

    side.vbo.indices.draw(GL_TRIANGLES);
 
    side.vbo.vertices.data().layout().disable();
    side.vbo.vertices.unbind();
      
  }

}
