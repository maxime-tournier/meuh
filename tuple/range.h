#ifndef TUPLE_RANGE_H
#define TUPLE_RANGE_H

#include <tuple>
#include <core/use.h>

namespace tuple {

  // static access index list for a tuple.  
  template<int ... I> 
  struct range {
    
    static constexpr int N = sizeof...(I);
    
    // apply a function term-wise and stores the result in a tuple
    // called in decreasing order !
    template<class F>
    static auto map(const F& fun) -> std::tuple< decltype( fun.template operator()<I>() )... > {
      return std::make_tuple( fun.template operator()<I>()... );
    }
    
    // apply a function on each term **in increasing order**
    template<class F>
    static void apply(const F& fun) {
      core::use( (fun.template operator()<N - 1 - I>(), 0)... );
    }
    
    typedef range< I..., N > next_type;    
    
  };
  

  template<int I>
  struct range_type {
    typedef typename range_type<I-1>::type::next_type type;
  };


  template<>
  struct range_type<0> {
    typedef range<> type;
  };

  
  // TODO tuple/each.h ?
  template<class ... Args>
  typename range_type<sizeof...(Args)>::type each(const std::tuple<Args...>&) { return {}; }

  
  
}


#endif
