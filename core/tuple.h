#ifndef CORE_TUPLE_H
#define CORE_TUPLE_H

#include <core/tuple/apply.h>
#include <core/tuple/zip.h>
#include <core/tuple/map.h>
#include <core/tuple/fold.h>
#include <core/tuple/repeat.h>

#include <core/edit.h>

namespace core {
  
  template<class ... Args>
  struct editor< std::tuple< Args... >& > {
    std::tuple<Args...>& ref;

    template<class F>
    typename std::result_of< F( Args&... ) >::type operator()(const F& f) const {
      return tuple::apply(f, ref);
    }
    
  };

  template<class ... Args>
  struct editor< const std::tuple< Args... >& > {
    const std::tuple<Args...>& ref;

    template<class F>
    typename std::result_of< F( const Args&... ) >::type operator()(const F& f) const {
      return tuple::apply(f, ref);
    }
    
  };

}

#endif
