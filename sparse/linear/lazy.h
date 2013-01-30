#ifndef SPARSE_LINEAR_LAZY_H
#define SPARSE_LINEAR_LAZY_H

#include <type_traits>
#include <core/strip.h>


namespace sparse {
  namespace linear {
    namespace range {

      template<class Key, class Value, class LHS, class RHS>
      struct sum {
	
	LHS lhs;
	RHS rhs;

	typedef std::pair<Key, Value> value_type;
	
	bool empty() const { return lhs.empty() && rhs.empty(); }

	void pop() const {
	  assert(!empty());
	  
	  if( lhs.empty() || (rhs.front().first < lhs.front.first)  ) {
	    rhs.pop(); return;
	  }

	  if( rhs.empty() || (lhs.front().first < rhs.front.first ) ) {
	    lhs.pop(); return;
	  }
	  
	  lhs.pop(); rhs.pop();
	}

	
	value_type front() const { 
	  if( lhs.empty() || (rhs.front().first < lhs.front.first)  ) {
	    return rhs.front();
	  }
	  
	  if( rhs.empty() || (lhs.front().first < rhs.front.first ) ) {
	    return lhs.front(); 
	  }

	  return std::make_pair(lhs.front().first, lhs.front() + rhs.front());
	}
	
      };


      

    }



    template<class Key, class Value>
    struct lvalue {
      typedef std::map<Key, Value> data_type;

      data_type& data;

      template<class Range>
      data_type& operator=(const Range& range) {
        data_type res;
	  
	auto it = res.begin();
	core::each( range, [&](const std::pair<Key, Value>& pair) {
	    it = res.insert(it, pair);
	  });
	  
	data = res;
	return data;
      }
    };

    
    template<class Key, class Value>
    lvalue<Key, Value> lazy( std::map<Key, Value>& data) { return {data}; } 
        
  }
}



#endif
