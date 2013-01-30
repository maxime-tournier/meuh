#ifndef CORE_LOG_H
#define CORE_LOG_H

#include <utility>
#include <iostream>		// TODO remove ?

namespace core {

  struct logger {

    bool enabled;
    std::ostream& stream;

    logger(std::ostream& stream = std::cout);

    template<class Head, class ... Tail>
    std::ostream& operator()(Head&& head, Tail&&... tail) const {
      if(!enabled) return stream;
      
      stream << head << '\t';
      return (*this)( std::forward<Tail>(tail)... );
    }
    
    std::ostream& operator()() const;
    
  };

  
  logger& log();

  template<class H, class... T>
  std::ostream& log(H&& h, T&&...t) {
    return log()(std::forward<H>(h), std::forward<T>(t)...);
  }
  
}



#endif
