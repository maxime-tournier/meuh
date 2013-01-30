#ifndef CORE_RANGE_PAIR_H
#define CORE_RANGE_PAIR_H

#include <core/make.h>

namespace core {
  namespace range {
    
    // TODO deal with references...
    template<class Range>
    struct first {

      Range pair;
      
      typedef decltype( core::make<Range>().front().first ) value_type;
      
      bool empty() const { return pair.empty();} 
      void pop() { pair.pop();} 
      value_type front() const { return pair.front().first;} 
      
    };
    
    template<class Range>
    first<Range> take_first(const Range& r) { return {r}; }

    // template<class Range>
    // struct second {
    //   Range pair;
      
    //   typedef typename Range::value_type::second_type value_type;

    //   bool empty() const { return pair.empty();} 
    //   void pop() { pair.pop();} 
    //   value_type& front() const { return pair.front().second;} 
      
    // };

    // template<class Range>
    // second<Range> take_second(const Range& r) { return {r}; }

  }
}



#endif
