#ifndef CORE_CALLBACK_H
#define CORE_CALLBACK_H

#include <functional>

namespace core {

  // default void -> void callback type
  typedef std::function< void() > callback;


  // push front/back actions on a std::function
  template<class F, class ... Args>
  void push_back(std::function< void(Args...) >& on, const F& f) {
    if(!on) on = f;
    else {
      auto copy = on;
      on = [copy, f] (Args... args) {
	copy(args... );
	f( args... );
      };
    }
  }

  template<class F, class ... Args>
  void push_front(std::function<void(Args...) >& on, const F& f) {
    if(!on) on = f;
    else {
      auto copy = on;
      on = [copy, f] (Args ... args) {
	f( args... );
	copy( args... );
      };
    }
  }
}


#endif
