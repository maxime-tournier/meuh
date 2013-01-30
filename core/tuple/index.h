#ifndef INDICES_H
#define INDICES_H

#include <tuple>

namespace core {
  namespace tuple {


    template<int ... I> struct index {
      template<int N> struct push_back {
	typedef index<I..., N> type;
      };

      template<int N> struct push_front {
	typedef index<N, I...> type;
      };
    };

    template<int N, int ... I>
    index< (N + I)...> shift( index<I...> ) {
      return index< (N+I)... >();
    }
    
    template<int N> struct upto {
      typedef typename 
      upto<N-1>::type::template push_back<N>::type type;
    };
    
    template<> struct upto<0> { typedef index<0> type; };
    
    template<class ... Args>
    typename upto< sizeof...(Args) - 1 >::type
    make_index(const std::tuple<Args...>& ) {
      return typename upto< sizeof...(Args) - 1 >::type();
    }

  }
}


#endif
