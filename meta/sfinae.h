#ifndef META_SFINAE_H
#define META_SFINAE_H

namespace meta {

  struct enable {};

  template<class T=enable>
  struct trigger {
    typedef T type;
  };
  
  template<class ... A> struct sfinae : trigger<> { };
  
}


#endif
