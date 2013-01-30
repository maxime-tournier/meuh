#ifndef MATH_FUNCTION_ANY_H
#define MATH_FUNCTION_ANY_H


#include <memory>
#include <cassert>

#include <core/safe_bool.h>

#include <math/func.h>
#include <math/func/base.h>
#include <math/func/push.h>
#include <math/func/pull.h>
#include <math/func/ref.h>

#include <math/func/constant.h>

#include <core/local.h>

namespace math { 

  namespace func { 

    template<class Domain, class Range>
    class any : public core::safe_bool< any<Domain, Range> >{

      friend class core::safe_bool< any >;
      
      typedef Domain domain;
      typedef Range range;
      
      typedef any< typename math::lie::group<domain>::algebra, typename math::lie::group<range>::algebra  > push_type;
      typedef any< typename math::lie::group<range>::coalgebra, typename math::lie::group<domain>::coalgebra > pull_type;
      
      struct base {
	virtual ~base() { };
      
	virtual range call(const domain& x) const = 0;
      
	virtual push_type push(const domain& at) const = 0;
	virtual pull_type pull(const domain& at) const = 0;
      
	virtual std::unique_ptr<base> copy() const = 0;
	
      };

      template<class F>
      struct derived : base {
	mutable core::local<F> value;
	
	derived(const F& value) : value(value) { }
	derived(F&& value) : value( std::move(value) ) { }
	
	virtual range call(const domain& x) const {
	  return (*value)(x);
	}

	virtual push_type push(const domain& at) const {
	  return typename traits<F>::push(*value, at);
	}
	
	virtual pull_type pull(const domain& at) const {
	  return typename traits<F>::pull(*value, at);
	}
	
	virtual std::unique_ptr<base> copy() const {
	  return std::unique_ptr<base>( new derived(*value) );
	}

      };


      bool boolean() const { return bool(impl); }

      typedef std::unique_ptr< base > impl_type;
      impl_type impl;
      
      template<class F>
      void set(F&& f) { 
	impl.reset( new derived< typename std::decay<F>::type >( std::forward<F>(f) ));
      }
      
   
    public:
      
      template<class F>
      any( F&& f) {
	set( std::forward<F>(f) );
      }
      
      
      template<class F>
      any& operator=(const F& f) {
	set(f);
	return *this;
      }
      
      template<class F>
      any& operator=(const F&& f) {
	set(std::move(f));
	return *this;
      }


      any() = default;
      any(any&& ) = default;
      any(const any& other)  : impl(other.impl->copy()) { 
      	// int warning = 0;
      }
      
      any& operator=(const any& other) { 
	impl = other.impl->copy();
	return *this;
      }
      
      any& operator=(any&&) = default;
      
      range operator()(const domain& x) const { 
	assert( impl );
	return impl->call(x); 
      }
      
      struct push : func::base< push_type > {
	
	push(const any& of, const domain& at) 
	  : push::base(of.impl->push(at)) {
	  
	}


	
      };

      struct pull : func::base< pull_type > {
	
	pull(const any& of, const domain& at) 
	  : pull::base(of.impl->pull(at)) {
	  
	}


	
       };

      
    };




  

  

  }

}

#endif
