#ifndef CORE_RANGE_STL_H
#define CORE_RANGE_STL_H

#include <core/range.h>
#include <utility>
#include <iterator>

namespace core {

  namespace range {
    
    // template<class Iterator>
    // struct stl {
    //   // silly STL
    //   typedef typename std::iterator_traits<Iterator>::reference value_type;
      
    //   Iterator first;
    //   Iterator last;
      
    //   stl(Iterator first, Iterator last) : first(first), last(last) { }
    //   stl(const std::pair<Iterator, Iterator>& bounds) : first( bounds.first), last(bounds.second) { }
      
    //   bool empty() const { return first == last; }
    //   void pop() { ++first; }
    //   value_type front() const { return *first; }
      
    // };

    

    // // TODO refinements
    // template<class Container>
    // range::stl<typename Container::const_iterator>
    // all(const Container& cont) { return {cont.begin(), cont.end()}; }

    // template<class Container>
    // range::stl<typename Container::iterator>
    // all(Container& cont) { return {cont.begin(), cont.end()}; }
 
  }

  
}


#endif
