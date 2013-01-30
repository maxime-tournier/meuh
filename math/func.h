#ifndef MATH_FUNC_H
#define MATH_FUNC_H

#include <math/types.h>

#include <core/make.h>
#include <core/use.h>

#include <meta/name.h>
#include <math/error.h>

#include <math/func/error.h>
#include <math/func/base.h>


namespace math {

  namespace func {

    template<class F> struct default_push;
    template<class F> struct default_pull;

    template<class F> struct default_hess;
    
    namespace impl {
      
      template<class Range, class G, class Domain>
      typename std::decay<Range>::type range(Range (G::*)(const Domain&) const );
    
      template<class Range, class G, class Domain>
      typename std::decay<Range>::type range(Range (G::*)(Domain&&) const );
      
      template<class Range, class G, class Domain>
      Domain domain(Range (G::*)(const Domain&) const );

      template<class Range, class G, class Domain>
      Domain domain(Range (G::*)(Domain&&) const );

      template<class A, class B> struct different { typedef A type; };
      template<class A> struct different<A, A>;
      
      // push::push refers to the first push (injected name), so we
      // exclude these cases from matches
      template<class F>
      typename different<typename F::push, F>::type* push(F*);
      
      template<class F>
      default_push<F>* push( ... );
      
      template<class F> 
      typename different<typename F::pull, F>::type*  pull(F*);

      template<class F>
      default_pull<F>* pull( ... ); 


      template<class F> 
      typename different<typename F::hess, F>::type* hess(F*);

      template<class F>
      default_hess<F>* hess( ... ); 
      
      template<class Domain, class Range, class F>
      bool requires(Range (F::*)(const Domain& ) const);
      
      template<class Domain, class Range, class F>
      bool requires(Range (F::*)(Domain&& ) const);
    }
    
    template<class ... F>
    auto requires() -> decltype( core::use( impl::requires( &F::operator() )... ) );
    
    template<class F>
    struct traits<F, decltype( requires<F>() ) >  {
      
      typedef decltype( impl::range(&F::operator()) ) range;
      typedef decltype( impl::domain(&F::operator()) ) domain;
      
      // we use pointers to prevent the compiler from instanciating
      // these types
      typedef typename std::remove_pointer< decltype( impl::push<F>( 0 )) >::type push;
      typedef typename std::remove_pointer< decltype( impl::pull<F>( 0 )) >::type pull;
      typedef typename std::remove_pointer< decltype( impl::hess<F>( 0 )) >::type hess;
      
      // TODO trigger this
      static void constraints() {

	// TODO use std::declvar
	domain x = core::make<domain>();
	F f = core::make<F>();
	
	range y = f(x);

	push df = {f, x};
	pull dTf = {f, x};
	hess Hf = {f, x};
	
	core::use(x, f, y, df, dTf);
      }

    };

 

    template<class F>
    struct default_push : base< typename error< typename traits<F>::domain,
						typename traits<F>::range >::push > {
      
      default_push(const F& , const typename traits<F>::domain& )
	: default_push::base( meta::name<F>() + " has no pushforward" ) {

      };
		   

    };


    
    template<class F>
    struct default_pull : base< typename error< typename traits<F>::domain,
						typename traits<F>::range >::pull > {
      
      default_pull(const F& , const typename traits<F>::domain& )
	: default_pull::base( meta::name<F>() + " has no pullback" ) {

      };
		   

    };


  }
  
}


#endif
