#ifndef GL_SKELETON_H
#define GL_SKELETON_H

#include <core/tree/node.h>
#include <math/se3.h>

#include <gl/error.h>
#include <math/func/coord.h>
#include <gl/gl.h>
#include <gl/common.h>

#include <core/graph.h>
#include <gl/frame.h>


namespace phys {
  namespace rigid {
    struct skeleton;
  }
}

namespace gl {
  

  namespace skel {

    void draw(const phys::rigid::skeleton& skeleton,
	      const math::vector< math::SE3 >& absolute);
    
    void draw_box(const phys::rigid::skeleton& skeleton,
		  const math::vector< math::SE3 >& absolute);

    void draw_box(const phys::rigid::skeleton& skeleton,
		  const math::vector< math::SE3 >& absolute,
		  math::natural i);
    
  }
}
  
// struct skeleton {
//   const enum mode{
  //     wire,
  //     frames,
  //     full
  //   } how;
    
  //   // TODO bone width ?
  //   skeleton(mode how) : how(how) { }
    

  

    // template<class A>
    // void draw(core::tree::node<A>* topology, 
    // 	      const math::vector< math::SE3 >& positions) const {
    //   topology->each( [&](core::tree::node<A>* j) {
    // 	  if( j->root() ) { root( positions(j->get().id) ); }
    // 	  else {
    // 	    bone( positions(j->parent()->get().id),
    // 		  positions(j->get().id) );			    
	    
    // 	  }
    // 	});
    // }

    // template<class A>
    // void draw_center(core::tree::node<A>* topology, 
    // 		     const math::vector< math::SE3 >& positions) const {
    //   topology->each( [&](core::tree::node<A>* j) {
    // 	  const math::natural i = j->get().id;
    // 	  glPushName(i);
    // 	  phys_bone( positions(i), j->get().length );
    // 	  glPopName();
    // 	});
    // }

    // template<class A>
    // void draw_center(core::tree::node<A>* topology, 
    // 		     const math::T<math::vector< math::SE3 > >& dpositions) const {
    //   using namespace math;

    //   const Lie< vector<SE3> > dmn( dpositions.at() );
      
    //   topology->each( [&](core::tree::node<A>* j) {
	  
    // 	  const natural i = j->get().id;
    // 	  glPushName(i);
	  
    // 	  gl::color( (j->root() || j->leaf()) ? gl::white() : gl::red() );
    // 	  phys_bone( dpositions.at()(i), j->get().length );
    // 	  reverse_velocity( func::coord<SE3>(i).diff(dpositions) );
    // 	  glPopName();
    // 	});
      
      
    // }
    
    

    
    // template<class A>
    // void draw(core::tree::node<A>* topology, 
    // 	      const math::T<math::vector< math::SE3 > >& dpositions) const {
    //   topology->each( [&](core::tree::node<A>* j) {
	  
    // 	  const math::natural i = j->get().id;
    // 	  if( j->root() ) { root( dpositions.at()(i) ); }
    // 	  else {
    // 	    bone( dpositions.at()(j->parent()->get().id),
    // 		  dpositions.at()(i) );			    
	    
    // 	  }
    // 	  velocity( math::body( dpositions.at()(i), dpositions.body()(i) ) );
    // 	});
    // }
    
    // void bone(const math::SE3& start, const math::SE3& end) const;
    // void root(const math::SE3& start) const;
    // void velocity( const math::T< math::SE3 >& ) const;
    // void reverse_velocity( const math::T< math::SE3 >& ) const;
    
    // void phys_bone(const math::SE3& frame, const math::real& length) const;

    // static void init();
  // };





#endif
