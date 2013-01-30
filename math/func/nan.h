#ifndef MATH_FUNC_NAN_H
#define MATH_FUNC_NAN_H

#include <math/types.h>
#include <math/nan.h>
#include <core/type.h>
#include <math/error.h>

namespace math {

  namespace func {

    template<class F>
    struct nan_check {
      const F f;

      typedef typename F::domain domain;
      typedef typename F::range range;

      template<class A>
      static void check(const A& a, const std::string& message ) {
	if( math::nan(a) ) throw math::error( core::type<F>::name() + " " + message );
      }



      range operator()(const domain& x) const {
	check(x, "has nan input!");
	
	range res = f(x);
	
	check(res, "returned a nan!");
	
	return res;
      }

      T<range> operator()(const T<domain>& dx) const {

	check(dx.at(), "has nan input! (at)");
	check(dx.body(), "has nan input! (body)");

	T<range> res = math::push(f, dx);
	
	check(res.at(), "returned a nan! (at)");
	check(res.body(), "returned a nan! (body)");
	return res;
      }

      coT<domain> operator()(const pullT<nan_check>& fx) const {

	
	check(fx.at(), "has nan input! (at)");
	check(fx.body(), "has nan input! (body)");


      	coT<domain> res = math::pull(f, fx.at(), fx.body());

	check(res.at(), "returned a nan! (at)" );
	check(res.body(), "returned a nan! (body)"  );
	 
      	return res;
      } 

    };


    template<class F>
    nan_check<F> nan(const F& f) { return {f}; }


  }

}

#endif
