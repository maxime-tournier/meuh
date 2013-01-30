#ifndef CORE_CHANNEL_H
#define CORE_CHANNEL_H

#include <future>

namespace core {

  template<class Type>
  struct channel {
    std::promise< Type > promise;
    std::future< Type > future;
    
    void refresh() { 
      promise = std::promise<Type>();
      future = promise.get_future();
    }
    
  };

}


#endif
