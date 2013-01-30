#ifndef CORE_UNIQUE_H
#define CORE_UNIQUE_H

#include <memory>

namespace core {

  template<class Class, class ...Args>
  std::unique_ptr<Class> unique(Args&&... args) { 
    return std::unique_ptr<Class>( new Class(std::forward<Args>(args)... ) );
  }

  template<class Class>
  std::unique_ptr<Class> unique(Class* ptr) { 
    return std::unique_ptr<Class>(ptr); 
  }
  
}

#endif
