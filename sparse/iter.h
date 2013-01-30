#ifndef SPARSE_ITER_H
#define SPARSE_ITER_H

#include <sparse/zero.h>
#include <core/range.h>

#include <core/stl.h>

// hardcore rvalue action going on in here

namespace sparse {

  namespace helper {

    template<class A, class B>
    const B& second(const std::pair<A, B>& pair ) {
      return pair.second;
    }

    template<class A, class B>
    B& second(std::pair<A, B>& pair ) {
      return pair.second;
    }

    template<class A, class B>
    B&& second(std::pair<A, B>&& pair ) {
      return std::move(pair.second);
    }

  }
    

  template<class Range1, class Range2, class F>
  void iter(Range1 r1, Range2 r2, const F& f = F() ) {
      
    while (!r1.empty() && !r2.empty()) {
	
      auto k1 = r1.front().first;
      auto k2 = r2.front().first;
	
      if( k1 == k2) {  
	f(k1, helper::second(r1.front()), helper::second(r2.front()));
	r1.pop();
	r2.pop();
      } else if( k2 < k1 ) {
	f( k2, zero(), helper::second(r2.front()) );
	r2.pop();
      } else {
	f( k1, helper::second(r1.front()), zero());
	r1.pop();
      }
    }

    assert( r1.empty() || r2.empty() );
      
    for(; !r1.empty(); r1.pop() ) {
      f( r1.front().first, helper::second(r1.front()), zero() );
    }
      
    for(; !r2.empty(); r2.pop() ) {
      f( r2.front().first, zero(), helper::second(r2.front()) );
    }
      
  }

  
  // only iterates on both non-zero values, taking the smallest
  // container as a basis

  // TODO rename iter_smallest
  template<class Key, class LHS, class RHS, class F>
  void iter_nonzero(const std::map<Key, LHS>& lhs,
		    const std::map<Key, RHS>& rhs,
		    const F& f = F()) {
    if(lhs.size() < rhs.size() ) {
      
      core::each(lhs, [&](Key k, const LHS& v) {
	  
	  auto it = rhs.find(k);

	  if( it != rhs.end() ) {
	    f(k, v, it->second );
	  } else {
	    f(k, v, zero() );
	  }

	});
      

    } else {

       core::each(rhs, [&](Key k, const RHS& v) {
	  
	  auto it = lhs.find(k);

	  if( it != lhs.end() ) {
	    f(k, it->second, v);
	  } else {
	    f(k, zero(), v);
	  }
	  
	 });
    }
    
  }

  // FIXME copypasta
  template<class Key, class LHS, class RHS, class F>
  void iter_nonzero(std::map<Key, LHS>& lhs,
		    const std::map<Key, RHS>& rhs,
		    const F& f = F()) {
    if(lhs.size() < rhs.size() ) {
      
      core::each(lhs, [&](Key k, LHS& v) {
	  
	  auto it = rhs.find(k);

	  if( it != rhs.end() ) {
	    f(k, v, it->second );
	  } else {
	    f(k, v, zero() );
	  }

	});
      

    } else {

       core::each(rhs, [&](Key k, const RHS& v) {
	  
	  auto it = lhs.find(k);

	  if( it != lhs.end() ) {
	    f(k, it->second, v);
	  } else {
	    f(k, zero(), v);
	  }
	  
	 });
    }
    
  }

    
  // template<class Range1, class Range2, class F>
  // void iter_move(Range1 r1, Range2 r2, const F& f = F() ) {
  //   for(; (!r1.empty()) && (!r2.empty());  ) {

  // 	auto k1 = r1.front().first;
  // 	auto k2 = r2.front().first;
	
  // 	auto& c1 = r1.front().second;
  // 	auto& c2 = r2.front().second;
	
  // 	if( k1 == k2) {  
  // 	  f(k1, std::move(c1), std::move(c2));
  // 	  r1.pop();
  // 	  r2.pop();
  // 	} else if( k2 < k1 ) {
  // 	  f( k2, zero(), std::move(c2) );
  // 	  r2.pop();
  // 	} else if( k1 < k2 ) {
  // 	  f( k1, std::move(c1), zero());
  // 	  r1.pop();
  // 	}
  //   }

  //   assert( r1.empty() || r2.empty() );
      
  //   core::foreach(r1, [&](decltype(r1.front()) v) {
  // 	  f( v.first, std::move(v.second), zero() );
  // 	});
  //   core::foreach(r2, [&](decltype(r2.front()) v) {
  // 	  f( v.first, zero(), std::move(v.second) );
  // 	});
  // }
    


}


#endif
