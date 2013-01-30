#ifndef CORE_EDIT_H
#define CORE_EDIT_H

#include <functional>

namespace core {

  // TODO rename access !
  // edit a value with a lambda. used for iteration.

  template<class T> 
  struct editor {
    
    T&& ref;
    
    template<class F>
    typename std::result_of< F(T&&) >::type operator()(const F& f) const {
      return f( std::forward<T>(ref) );
    }
    
  };
  
  template<class T>
  editor<T> edit(T&& ref) { return {std::forward<T>(ref)}; }
  
}


#endif
