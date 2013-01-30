#ifndef TOOL_CONTROL_H
#define TOOL_CONTROL_H

#include <math/mat.h>
#include <math/vec.h>

#include <map>

#include <phys/object.h>
#include <math/func/jacobian.h>

namespace tool {


  // TODO rename to "features" ?
  
  class control {
  public:
    
    typedef Eigen::Block<math::mat::base> mat_block;
    typedef Eigen::VectorBlock<math::vec::base> vec_block;
    
    typedef std::function< void( mat_block, vec_block ) > block_type;
    
  private:    
    
    struct feature_type {
      block_type block;
      math::natural dim;
      math::real weight;
    };


    const math::natural dofs;
    
    std::map< std::string, feature_type > feature;
    
    math::natural dimension;
    
  public:
    
    control(math::natural dofs);
    
    void add(const std::string& name,
	     math::natural dim,
	     const block_type& block,
	     math::real w = 1);

    struct gains {
      const math::real p, d;

      gains(math::real p = 1, math::real d = 0) : p(p), d(d) { }
      
    };

    template<class F>
    void add(const std::string& name,
	     math::real w,
	     const F& fun,	// error function
	     const phys::object< typename math::func::traits<F>::domain >* obj,
	     gains g = gains(),
	     const math::lie::group< typename math::func::traits<F>::range >& dmn = 
	     math::lie::group< typename math::func::traits<F>::range >() ) {
      add( name, dmn.alg().dim(), [&](mat_block M, vec_block v) {
	  auto adapted = dmn.log() << fun;
	  
	  using namespace math;
	  
	  M = func::J(adapted, obj->lie, dmn)(obj->g());
	  
	  // TODO vectorize function here for complicated spaces ?
	  
	  vec v_k; v_k.resize(obj->lie.alg().dim());
	  euclid::get(v_k, obj->v());
	  
	  v = -g.p * adapted( obj->g() ) -g.d * (M * v_k);
	  
	}, w);
      
    }
    
    void weight(const std::string& name,
		math::real);
    
    math::natural dim() const;


    void update();
    void clear();
    
    math::mat matrix;
    math::vec vector;

  };

}



#endif
