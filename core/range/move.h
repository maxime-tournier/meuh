#ifndef CORE_RANGE_MOVE_H
#define CORE_RANGE_MOVE_H

namespace core {
  namespace range {

    template<class Range>
    struct move_values {
      
      Range range;

      typedef typename std::remove_reference< typename Range::value_type >::type&& value_type;
      
      bool empty() const { return range.empty(); }
      void pop() { range.pop(); }
      value_type front() const { return std::move( range.front() ); }
      
    };


    template<class Range>
    move_values<Range> move(const Range& range ) { return  {range}; }


  }
}

#endif
