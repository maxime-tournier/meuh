#ifndef SPARSE_ZIP_H
#define SPARSE_ZIP_H

#include <sparse/zero.h>
#include <sparse/insert.h>
#include <sparse/iter.h>

#include <core/make.h>

#include <core/range/move.h>
#include <core/stl.h>

namespace sparse {

  // algorithms
  namespace helper {

    // perfect forward to f
    template<class Key, class Value, class F>
    struct inserter {
      std::map<Key, Value>& result;
      const F& f;
	
      mutable typename std::map<Key, Value>::iterator it;

      template<class A, class B>
      void operator()(const Key& k, A&& a, B&& b ) const {
	it = ++sparse::insert(result, k, f(std::forward<A>(a), std::forward<B>(b)), it );
      }
      
    };

      
    template<class Key, class Value, class F>
    inserter<Key, Value, F> make_inserter(std::map<Key, Value>& res,
					  const F& f = F() ) {
      res.clear();
      auto it = res.begin();
      
      return { res, f, it };
    }


  }


  template<class Key, class Res, class C1, class C2, class F>
  void zip(std::map<Key, Res>& res, 
	   const std::map<Key, C1>& m1, 
	   const std::map<Key, C2>& m2,
	   const F& f = F()) {
      
    using namespace core::range;
    sparse::iter(all(m1), all(m2), 
		 helper::make_inserter(res, f) );
     
  };
    
  // 
  template<class Key, class Res, class C1, class C2, class F>
  void zip_nonzero(std::map<Key, Res>& res, 
		   const std::map<Key, C1>& m1, 
		   const std::map<Key, C2>& m2,
		   const F& f = F()) {
    sparse::iter_nonzero(m1, m2, helper::make_inserter(res, f) );
  };


  template<class Key, class Res, class C1, class C2, class F>
  void zip(std::map<Key, Res>& res, 
	   std::map<Key, C1>&& m1, 
	   std::map<Key, C2>&& m2,
	   const F& f = F()) {
      
    using namespace core::range;
    sparse::iter(move(all(m1)), move(all(m2)), 
		 helper::make_inserter(res, f) );
  };
    


}


#endif
