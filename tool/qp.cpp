#include "qp.h"

#include <math/qp.h>
#include <core/stl.h>

namespace tool {

  qp::qp() : eps(0) { }
  
  void qp::clear() { (*this) = qp(); }
  
  
  void qp::add_var( math::natural dim ) { 
    _c.conservativeResize( _c.size() + dim );
  }
  
  void qp::add_con( math::natural dim ) { 
    _b.conservativeResize( _b.size() + dim );
  }
  
  math::vec::lchunk qp::b() { return _b.segment(0, _b.size()); }
  math::vec::rchunk qp::b() const { return _b.full(); }
  
  math::vec::lchunk qp::c() { return _c.segment(0, _c.size()); }
  math::vec::rchunk qp::c() const { return _c.full(); }
  

  math::natural qp::vars() const { return _c.size(); }
  math::natural qp::cons() const { return _b.size(); }
  
  void qp::solve(math::vec& x) const {
    
    assert( x.empty() || x.size() == vars() );
    
    using namespace math;
    
    vec qout;
    
    auto Q = [&](vec::rchunk x) -> const vec& {
      qout = vec::Zero( vars() );
      
      core::each(metric, [&](const linear_type& q) {
	  q(qout, x);
	});

      return qout;
    };
    
    vec aout;
    auto A = [&](vec::rchunk x) -> const vec& {
      
      // TODO this is useless
      aout = vec::Zero( cons() );

      core::each(constraint, [&](const linear_type& a) {
	  a(aout, x);
	});
      
      return aout;
    };
    
    vec aTout;
    auto AT = [&](vec::rchunk x) -> const vec& {
       
      aTout = vec::Zero( vars() );
      
      core::each(constraintT, [&](const linear_type& aT) {
	  aT(aTout, x);
	});
      
      return aTout;
    };
    
    x = math::qp(Q, A, AT).solve(c(), b(), iter, x);
  }

}
