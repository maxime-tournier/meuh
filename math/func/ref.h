#ifndef MATH_FUNC_REF_H
#define MATH_FUNC_REF_H

#include <math/func.h>
#include <math/types.h>


namespace math {
	namespace func {
    
		template<class F>
		struct reference {
      
			const F& to;
			reference(const F& f) : to(f) { }
	
			typedef typename traits<F>::domain domain;
			typedef typename traits<F>::range range;
	
			range operator()(const domain& x) const { 
				return to(x); 
			}
      
			struct push : base<typename traits<F>::push > {
				push(const reference& of, const domain& at) : push::base(of.to, at) { }
			};

			struct pull : base< typename traits<F>::pull > {
				pull(const reference& of, const domain& at) : pull::base(of.to, at) { }
			};

      
		};


		template<class F>
		reference<F> ref(const F& f) { return {f}; }
    
    
    
	}
}

#endif
