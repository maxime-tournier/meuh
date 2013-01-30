#ifndef INSTANCIATOR_H
#define INSTANCIATOR_H




struct types;

template< template<class> class T, class Args1 > struct arg1;
template< template<class, class> class T, class Args1, class Args2 > struct arg2;

// and so on
template<class> struct instanciator;


template< template<class> class T, class A >
struct instanciator< arg1<T, types(A) > > : T<A>  { 
  
};

template< template<class> class T, class A, class B>
struct instanciator< arg1<T, types(A, B) > > : T<A>, T<B>  { };

template< template<class> class T, class A, class B, class C>
struct instanciator< arg1<T, types(A, B, C) > > : T<A>, T<B>, T<C>  { };

// and so on

template< template<class, class> class T,
	  class A, class B>
struct instanciator< arg2<T, types(A), types(B) > > : T<A, B> {
  
};

template< template<class, class> class T,
	  class A, class B, class C, class D>
struct instanciator< arg2<T, types(A, B), types(C, D) > >: T<A, C>, T<A, D>, T<B, C>, T<B, D>  {
  
};
  
// and so on








#endif
