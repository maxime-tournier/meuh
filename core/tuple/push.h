#ifndef TUPLE_PUSH_H
#define TUPLE_PUSH_H

#include <tuple>

namespace core {
  namespace tuple {

    template<class, class > struct push_back;

    template<class ... Args, class A>
    struct push_back< std::tuple<Args...>, A> {
      typedef std::tuple<Args..., A> type;
    };

    template<class, class > struct push_front;

    template<class A, class ... Args >
    struct push_front< A, std::tuple<Args...> > {
      typedef std::tuple<A, Args...> type;
    };
  

  }
}


#endif
