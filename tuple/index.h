#ifndef TUPLE_INDEX_H
#define TUPLE_INDEX_H

namespace tuple {
  
  // finds the first position of Type inside Tuple
  template<class Type, class Tuple>
  struct index;

  template<class Type, class Head, class ... Args>
  struct index< Type, std::tuple<Head, Args...> >{
    static const int value = 1 + index<Type, std::tuple<Args...> >::value;
  };
  
  template<class Type, class ... Args>
  struct index< Type, std::tuple<Type, Args...> > {
    static const int value = 0;
  };
  
}


#endif
