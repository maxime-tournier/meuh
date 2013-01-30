
#include <meta/concept.h>

template<class> struct Foo;

namespace meta {
 
  template<class X>
  struct Concept<Foo<X> > {
    
    ~Concept() {
      typedef typename X::domain domain;
      typedef typename Foo<X>::range range;
    }
    
  };
}



struct foo {
  typedef int domain;
};
  
struct bar {
  // typedef char domain;
};

template<>
struct Foo<foo> {
  typedef double range;
};

template<class X, class = meta::enable>
struct ForFoo;


template<class X>
struct ForFoo<X, requires( Foo<X> ) > {
  
};

void bob () {

  // requires< Foo<foo> > t;
  // requires< Foo<bar> > u;

  ForFoo<foo> t;
  // ForFoo<bar> u;

  
  // concept_assert( Foo<foo> );
  // concept_assert( Foo<bar> );
  // assert<Foo<bar> > y;

  // taiste< Foo<foo> > t;
  // taiste< Foo<bar> > u;

}
