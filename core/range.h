#ifndef CORE_RANGE_H
#define CORE_RANGE_H

namespace core {

  // TODO give its own module ?
  namespace range {
    
    template<class Range>
    struct traits;

    // template<class Range>
    // struct traits {
    //   typedef typename Range::value_type value_type;
    // };

    // template<class Range>
    // bool empty(const Range& range) { return range.empty(); }

    // template<class Range>
    // void pop(Range& range) { range.pop(); }
    
    // template<class Range>
    // typename traits<Range>::value_type get(const Range& range) { return range.front(); }
    
  }
    
}

#endif
