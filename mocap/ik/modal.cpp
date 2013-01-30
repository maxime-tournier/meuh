#include "modal.h"

#include <core/stl.h>

#include <map>
#include <set>

#include <math/func/index.h>
#include <math/func/compose.h>
#include <math/algo/iter.h>
#include <math/svd.h>

namespace mocap {
  namespace ik {

    modal::range modal::operator()(const domain& g, 
				   const math::algo::stop& s ) const {
      using namespace math;
      // 1 build objective function

      std::map<natural, handle> objs;
      
      core::each(handles, [&](const handle& h) {
	  objs[h.joint->get().id] = h;
	});
      
      std::set<natural> indices;
      
      vector< handle::value_type > obj;
      obj.resize(objs.size());
      
      natural i = 0;
      core::each( objs, [&]( natural i, const handle& h) {
	  indices.insert(indices.end(), i);
	  obj(i++) = h.value();
	});
      
      pose x = local(skeleton)(g);
      
      // 2 optimize with levmar
      using math::func::operator<<;
      auto f = func::index::select<SE3>( indices ) << global(skeleton) ;

      const Lie<pose> G(x);
      const Lie<pose> H(obj);

      algo::iter_eps(s, [&](real& eps) {
	  
	  // tangent obj
	  using lie::operator*;
	  vec target = G.alg().coords( G.log( G.inv(f(x)) * obj) );
	  eps = target.norm();	// TODO normalize ?
	  
	  // jacobian
	  mat J;
	  J.resize(H.alg().dim(), modes.cols());
	  
	  J.each_col([&](natural j) {
	      auto dmj = math::body(x, G.alg().elem(modes.col(j)) ); // TODO optimize body creation
	      J.col(j) = H.alg().coords( f( dmj ).body() );
	    });
	  
	  // pseudoinverse
	  vec descent = modes * svd(J).solve(target);
	  
	  using lie::operator*;
	  x = x * G.exp( G.alg().elem(descent) );
	  
	});
      
      // 3 profit
      return global(skeleton)(x);
    }

  }
}
