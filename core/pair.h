#ifndef CORE_PAIR_H
#define CORE_PAIR_H

#include <core/edit.h>

namespace core {


  template<class First, class Second>
  struct editor< std::pair<First, Second>& > {
    std::pair<First, Second>& ref;
    
    template<class F>
    typename std::result_of<F(First&, Second&)>::type operator()(const F& f) const {
      return f(ref.first, ref.second);
    }
    
  };

  
  template<class First, class Second>
  struct editor< const std::pair<First, Second>& > {
    const std::pair<First, Second>& ref;
    
    template<class F>
    typename std::result_of<F(const First&, const Second&)>::type operator()(const F& f) const {
      return f(ref.first, ref.second);
    }
    
  };
  

}


#endif
