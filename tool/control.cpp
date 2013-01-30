#include "control.h"

#include <core/stl.h>

namespace tool {

  control::control(math::natural dofs) 

    : dofs(dofs),
      dimension(0) { 
  
  }

  

  void control::add(const std::string& name,
		    math::natural dim, 
		    const block_type& chunk,
		    math::real w) {

    if(! w ) return; 
    
    feature[ name ] = { chunk, dim, w };
    dimension += dim;

  }


  void control::weight(const std::string& name,
		       math::real w ) {
    feature[name].weight = w;
  }


  void control::clear() {

    dimension = 0;
    feature.clear();
    
  }

  math::natural control::dim() const {
    return dimension;
  }


  void control::update() {

    matrix = math::mat::Zero( dim(), dofs );
    vector = math::vec::Zero( dim() );
    
    math::natural off = 0;
    core::each( feature, [&](const std::string& ,
			     const feature_type& f) {
		  auto M = matrix.block(off, 0, f.dim, matrix.cols());
		  auto v = vector.segment(off, f.dim );

		  f.block(M, v);
		  
		  M = f.weight * M;
		  v = f.weight * v;
		  
		  off += f.dim;
		  
		});
    
  }



}
