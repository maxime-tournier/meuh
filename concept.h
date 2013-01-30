#ifndef MEUH_CONCEPT_H
#define MEUH_CONCEPT_H

#include <type_traits>

// a concept is a constexpr predicate. 
typedef bool (concept)();
  
namespace impl {

  template<bool> struct requires;
  template<> struct requires<true> { 
    typedef bool type;
  };

  template<bool ... > struct And;
  template<> struct And<> { static constexpr bool value = true; };
    
  template<bool a, bool ... b> struct And<a, b...> { 
    static constexpr bool value = a && And<b...>::value; 
  };

  template<bool ... > struct Or;
  template<> struct Or<> { static constexpr bool value = false; };
    
  template<bool a, bool ... b> struct Or<a, b...> { 
    static constexpr bool value = a || Or<b...>::value; 
  };
    
    
  template<int> constexpr bool Defined() { return true; }
    
}

// logical connectors
template<concept ... c>
constexpr bool And() { return impl::And<c()...>::value; }

template<concept c>
constexpr bool Not() { return !c(); }

template<concept ... c>
constexpr bool Or() { return impl::Or<c()...>::value; }
  
constexpr bool True() { return true; }
constexpr bool False() { return false; }
  
  
// requires is aliased to bool whenever each argument is defined and
// true
template<concept ... c >
using requires = typename impl::requires< And<c...>() >::type;
  
 
// Type always returns true whenever its argument is a declared
// type. e.g. Type< typename foo::bar >()
template<class T>
constexpr bool Type() { return true; }
  
  
// Value always returns true provided its argument is a declared
// (constexpr) value. It can be used to check member functions:
// Value( &F::operator() )
template<class T>
constexpr bool Value(T) { return true; }

constexpr bool Value(...) { return false; }

// Defined is valid whenever its argument is a defined type (i.e. on
// which sizeof can be applied)
template<class T> 
constexpr requires< impl::Defined< sizeof(T) > > Defined() { return true; }


template<class A, class B>
constexpr bool Same() { return std::is_same<A, B>::value; }



#endif
