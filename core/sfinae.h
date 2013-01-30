#ifndef CORE_SFINAE_H
#define CORE_SFINAE_H

namespace core {
  
  template<class ... A> struct sfinae;
  
  struct enabler {};

  typedef enabler enable;

  struct trigger {
    typedef core::enabler type;
  };
  
  template<class ... A>
  struct sfinae : trigger { };  
  
  template<bool B, class T = enabler>
  struct enable_if_bool { };
  
  template<class T>
  struct enable_if_bool<true, T> { 
    typedef T type;
  };
    

  template<class Cond, class T = enabler >
  struct enable_if : enable_if_bool< Cond::value, T > { };

  template<class Cond, class T = enabler >
  struct enable_unless : enable_if_bool< !Cond::value, T >  { };

  template<class A, A> struct has;
  
  struct yes { char x; };
  struct no { char x[2]; };
  

  template<class ...> struct first;

  // predicates
  template<class ... > struct and_;
  
  template<> struct and_<> { static const bool value = true; };
  template<class H, class ... T> struct and_<H, T...> { static const bool value = H::value && and_<T...>::value; };
  
  template<class ... > struct or_;
  template<> struct or_<> { static const bool value = false; };
  template<class H, class ... T> struct or_<H, T...> { static const bool value = H::value || or_<T...>::value; };
    

}

#endif
