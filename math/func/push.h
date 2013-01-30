#ifndef MATH_FUNC_PUSH_H
#define MATH_FUNC_PUSH_H

#include <math/func.h>

namespace math {
	namespace func {

		template<class F>
		struct pushforward {
			F of;

			pushforward( const pushforward& ) = default;
			pushforward(pushforward&& ) = default;

			typename traits<F>::push operator()(const typename traits<F>::domain& x) const {
				return {of, x};
			}
      
		};

		template<class F>
		pushforward< typename std::decay<F>::type > d( F&& f ) { return {std::forward<F>(f)}; }
    
	}
}



#endif
