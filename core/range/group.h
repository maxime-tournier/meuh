#ifndef CORE_RANGE_GROUP_H
#define CORE_RANGE_GROUP_H

#include <core/tuple/fold.h>
#include <core/tuple/iter.h>
#include <core/tuple/map.h>

#include <core/type.h>

namespace core {

  namespace range {

    template<class ... Range>
    struct group {
      
      std::tuple<Range...> range;
      
      typedef std::tuple< typename Range::value_type... > value_type;
      
      struct impl {
	struct empty {
	
	  template<class R>
	  bool operator()(bool b, const R& r) const { return b || r.empty(); }
	
	};

	struct pop {
	  template<class R>
	  void operator()(R& r) const { r.pop(); }
	};

	struct front {

	  template<class R>
	  typename R::value_type operator()(const R& r) const {
	    return r.front();
	  }
	  
	};

      };
      
      
      bool empty() const { 
	return tuple::foldl(typename impl::empty(), false, range);
      };
      
      
      void pop() { 
	tuple::iter(typename impl::pop(), range);
      }
      
      value_type front() const { 
	return tuple::map(typename impl::front(), range);
      }
    };

    
    template<class ... Range>
    group<Range...> make_group(const Range&... range) { 
      return { std::make_tuple(range ... )}; 
    }
    
        
  }

}



#endif
