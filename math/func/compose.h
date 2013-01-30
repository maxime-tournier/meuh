#ifndef MATH_FUNC_COMPOSE_H
#define MATH_FUNC_COMPOSE_H

#include <math/func.h>
#include <math/lie.h>

#include <math/func/push.h>
#include <math/func/pull.h>
#include <math/func/ref.h>

namespace math {
	namespace func {

		namespace impl {
			template<class Domain, class Range>
			struct compose_check {
				static_assert( std::is_same< Domain, Range >::value, "function composition domain/range" );
			};
		}
    
		template<class LHS, class RHS>
		struct compose {
      
			LHS lhs;
			RHS rhs;

			compose( const LHS& lhs, const RHS& rhs ) : lhs(lhs), rhs(rhs) { }
			compose( LHS&& lhs, RHS&& rhs ) : lhs(std::move(lhs)), rhs(std::move(rhs)) { }

			compose(const compose& ) = default;
			compose(compose&& ) = default;

			static typename impl::compose_check<typename traits<LHS>::domain, 
			                                    typename traits<RHS>::range> check;
      
		typedef typename traits<RHS>::domain domain;
		typedef typename traits<LHS>::range  range;

		auto operator()( const domain& x) const -> decltype( lhs(rhs(x) ) ) {
			return lhs( rhs( x ) );
		}
      
		// TODO should be conditional ?
		struct push : base< compose< typename traits<LHS>::push,
		                             typename traits<RHS>::push > > {
			push(const compose& of,
			     const domain& at) 
				: push::base( d( ref(of.lhs) )( of.rhs(at) ), d( ref(of.rhs) )(at) ) {
	  
			}



		};

struct pull : base< compose< typename traits<RHS>::pull,
                             typename traits<LHS>::pull > > {
	pull(const compose& of,
	     const domain& at) 
		: pull::base( dT( ref(of.rhs))( at ), dT( ref(of.lhs) )( of.rhs(at) ) ) {
	  
	}



};

 
		};

		namespace impl {


			template<class LHS, class RHS, class = void>
			struct compose;

			template<class LHS, class RHS > 
			struct compose<LHS, RHS, decltype( math::func::requires<LHS, RHS>() ) > {
				typedef func::compose<LHS, RHS> type;
			};

		}


		template<class LHS, class RHS>
		typename impl::compose<  typename std::decay<LHS>::type,
		                         typename std::decay<RHS>::type >::type
		operator<<(LHS&& lhs, RHS&& rhs) {
			return { std::forward<LHS>(lhs), std::forward<RHS>(rhs) };
		}

	template<class LHS, class RHS>
	typename impl::compose< typename std::decay<RHS>::type,
	                        typename std::decay<LHS>::type >::type
	operator>>(LHS&& lhs, RHS&& rhs) {
		return { std::forward<RHS>(rhs), std::forward<LHS>(lhs) };
	}



    

}
	}

#endif
