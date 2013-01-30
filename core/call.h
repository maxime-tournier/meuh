#ifndef CORE_CALL_H
#define CORE_CALL_H

#include <utility>

namespace core {

  template<class F, class ... Args>
  auto call( const F& f, Args&&... args) -> decltype( f( std::forward<Args>(args)... )) {
    return f( std::forward<Args>(args)... );
  }
  
}


#endif
