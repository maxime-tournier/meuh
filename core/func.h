#ifndef CORE_FUNC_H
#define CORE_FUNC_H

namespace core {

  namespace impl {

    template<class A> struct help {
      typedef A type;
    };

    // template<class Ret, class C, class ... Args>
    // typename help< Ret(Args...) >::type  type( Ret (C::*)( Args ... )  );
    
    template<class Ret, class C, class ... Args>
    help< Ret(Args...) > type( Ret (C::*)( Args ... ) const  );
    
    
  }
  

  template<class F> 
  struct func {
    typedef decltype( impl::type(&F::operator()) ) tmp;
    typedef typename tmp::type type;
  };


}




#endif
