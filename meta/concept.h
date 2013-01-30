#ifndef META_CONCEPT_H
#define META_CONCEPT_H

#include <meta/sfinae.h>
#include <meta/function.h>
#include <meta/list.h>

#include <core/unused.h>
#include <core/macro/concat.h>

#include <type_traits>

namespace meta {

  template<class T, class = enable>
  struct requires;

  template<class A>
  void concept_assert() {
    A* a = 0;
    if( a ) a->~A();
  }
    

  template<class >
  struct Concept;

  template<class C>
  struct Concept< Concept<C> > {
    
    ~Concept() {
      concept_assert< Concept<C> >();
    }
    
  };
  

  template<class A, class B>
  struct Equal;
  
  template<class A, class B>
  struct Concept< Equal<A, B> > {
    ~Concept() {
      static_assert( std::is_same<A, B>::value, "A must equal B" );
    }
  };
  
  template<class A>
  struct Equal<A, A> { };




  template<class T, class = enable>
  struct Defined;

  template<class T>
  struct Concept<Defined<T> > {
    ~Concept() {
      sizeof(T);

      // we need this to implement requires in a non-recursive way
      requires< Equal<typename Defined<T>::type, enable> > x;
    }
  };

  template<class T>
  struct Defined<T, typename sfinae< int_list< sizeof(T) > >::type > : trigger<> { };


    
  // requirements cant use requires, otherwise it loops forever during
  // the template resolution
  template<class C>
  struct requires<C, typename sfinae< typename Defined< Concept<C> >::type, 
				      typename Defined< C >::type>::type > 
  : trigger<>
  {
   
    typedef function<void(&)(), concept_assert< Concept<Concept<C> > > > interface_check;
    typedef function<void(&)(), concept_assert< Concept<C> > > concept_check;
    
  };

  template<class ... C>
  struct requires<type_list<C...>, typename sfinae< typename requires<C>::type... >::type > : trigger<> { };
  
#define requires(...)							\
  typename ::meta::requires< ::meta::type_list<__VA_ARGS__> >::type	\


  
 

}


#endif
