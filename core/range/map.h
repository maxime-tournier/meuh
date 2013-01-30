#ifndef CORE_RANGE_MAP_H
#define CORE_RANGE_MAP_H

#include <functional>

namespace core {
  namespace range {

    template<class Range, class F>
    struct mapping {
      Range source;
      const F map;
    };

    template<class Range, class F>
    mapping<Range, F> map(const Range& range, const F& f) { return {range, f}; }

    
    template<class Range, class F>
    struct traits< mapping<Range, F> > {
      typedef typename std::result_of<F( typename traits<Range>::value_type )>::type value_type;
    };
    
    template<class Range, class F>
    bool empty(const mapping<Range, F>& r) { return empty(r.source); }

    template<class Range, class F>
    void pop(mapping<Range, F>& r) { pop(r.source); }
    
    template<class Range, class F>
    typename traits< mapping<Range, F> >::value_type get(const mapping<Range, F>& r) { 
      return r.map( get(r.source) ); 
    }
    
    
  }
}


#endif
