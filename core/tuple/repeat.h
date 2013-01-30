#ifndef TUPLE_REPEAT_H
#define TUPLE_REPEAT_H

#include <tuple>
#include <core/tuple/push.h>
#include <core/tuple/index.h>

namespace core {
  namespace tuple {

    template<class G, int N>
    struct repeat {

      typedef typename push_back< typename repeat<G, N-1 >::type, G>::type type;

    };

    namespace impl {
      
      template<int I, class G>
      const G& forward(const G& g) { return g; }

      template<int N, class G, int ... I>
      typename repeat<G, N>::type make_repeat(const G& g, index<I...> ) {
	return std::make_tuple( forward<I>(g)... );
      }

    }

    template<int N, class G>
    typename repeat<G, N>::type make_repeat(const G& g) {
      return impl::make_repeat<N>(g, typename upto<N-1>::type() );
    }
    
    


    template<class G>
    struct repeat<G, 0> { typedef std::tuple<> type; };
  
  }
}

#endif
