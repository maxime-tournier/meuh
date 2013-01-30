#ifndef CORE_RANGE_ITERATOR_H
#define CORE_RANGE_ITERATOR_H

#include <iterator>
#include <core/range.h>

namespace core {
  namespace range {

    // adaptor for iterator pairs
    template<class Iterator>
    struct traits< std::pair<Iterator, Iterator> > {
      typedef typename std::iterator_traits<Iterator>::reference value_type;
    };

    template<class Iterator>
    bool empty(const std::pair<Iterator, Iterator>& range) { return range.first == range.second; }

    template<class Iterator>
    void pop(std::pair<Iterator, Iterator>& range) { ++range.first; }

    template<class Iterator>
    typename traits<std::pair<Iterator, Iterator> >::value_type 
    get(const std::pair<Iterator, Iterator>& range) { return *range.first; }
    
   
    

  }
}




#endif
