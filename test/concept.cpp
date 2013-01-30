
#include <concept.h>


template<class> constexpr bool Foo() { return false; }
template<> constexpr bool Foo<int>() { return true; }


template<class, class = bool> struct for_foo;

template<class T> 
struct for_foo<T, requires< Foo<T> > > {
  void foo() { };
};

template<class, class = bool> struct for_defined;

template<class T>
struct for_defined<T, requires< Defined<T> > > {
  
};

struct undefined;

struct function {

  int operator()(const char&) const;
  
};

struct muchel {

};


namespace impl {

  template<class F, class Domain, class Range>
  static bool func(Range (F::*)(const Domain& ) const );

}


template<class F>
constexpr decltype(impl::func(&F::operator()))  Func() { return true; }



template<class, class = bool> struct for_func;

template<class F>
struct for_func<F, requires< Func<F> > > { 

};



int main(int, char** ) {

  for_foo<int> x;
  for_foo<char> y;


  for_defined<int> u;
  for_defined<undefined> v;

 
  for_func<int> a;
  for_func<function> b;

  // static_assert( Not< Defined< undefined > >(), "" );
  // static_assert( Defined< int >(), "" );

  return 0;
}
