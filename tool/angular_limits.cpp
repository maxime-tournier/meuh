#include "angular_limits.h"


#include <math/svd.h>

#include <math/se3.h>
#include <math/so3.h>
#include <math/mat.h>

#include <math/lie/dist.h>
#include <math/lie/slerp.h>

#include <core/log.h>

#include <math/func/vector.h>

#include <gl/common.h>
#include <gl/raii.h>

namespace tool {

  angular_limits::result_type angular_limits::operator()(const data_type& data, math::natural root)  const {
    assert(!data.empty());
    
    result_type res;
    
    core::logger log;
    
    using namespace math;

    math::vector< SO3 > orients;
    orients.resize(data.rows());

    const math::real eps = 0.5;
    
    data(0).each([&](math::natural j) {
	if( j == root ) return;
	
	// log("computing limits for joint", j);

	orients.each( [&](natural i ) {
	    orients(i) = data(i)(j);
	  });
   
	orients.each( [&](natural i ) {
	    // glitches removal
	    if(i > 0 && i < orients.rows() - 1) {
	      const math::real dist = lie::dist(orients(i - 1), orients(i));
	      if( dist > eps) { 
		log("removing outlier at frame:", i, dist);
		orients(i) = lie::slerp(orients(i-1), orients(i+1), 0.5); 
	      }
	    }
	    
	  });
	
	try {
	  res[j] = lie::fit_ellipsoid(orients, Lie<SO3>(),
				      outer, inner,
				      fuzz);
	}
	catch( const svd::exception::zero& e ) {
	  // degenerate ellipsoid for this dof: 
	  ellipsoid blocked;
	  blocked.center = orients(0);
	  blocked.metric = 1e14 * mat33::Identity();

	  res[ j ] = blocked;
	}
	

      });

    
    return res;
  }
  






  limits_viewer::limits_viewer() 
    : index(0)
  {
    
  }


  void limits_viewer::init() {
    startAnimation();
  }

  void limits_viewer::draw() {
    using namespace gl;
    auto it = limits.find(index);
    if( it != limits.end() ){

      raii::disable light(GL_LIGHTING);

      {
	point_size(1.0);
	raii::begin points(GL_POINTS);
	
	color( white() );
	data.each([&](math::natural i) {
	    vertex( math::lie::log(it->second.center.inverse() * (data(i)(index))) );
	    
	  });
      }
      {
	point_size(10.0);
	raii::begin points(GL_POINTS);
	if(!current.empty()){
	  color( red() );
	  vertex( math::lie::log(it->second.center.inverse() * current(index)));
	}
	
      }

    }
    
  }



}
