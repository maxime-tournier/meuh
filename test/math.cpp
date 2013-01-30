#include <math/func/compose.h>
#include <core/log.h>
#include <math/euclid.h>
#include <math/lie.h>
#include <math/real.h>

#include <tuple>

#include <meta/name.h>


#include <math/tuple.h>

#include <math/func.h>


#include <math/func/tuple.h>

#include <math/vec.h>
#include <math/form.h>

#include <math/func/constant.h>
#include <math/func/vector.h>
#include <math/func/euclid.h>

// #include <math/func/hermite.h>
#include <math/func/any.h>

#include <tool/rigid.h>
#include <math/so3.h>

#include <math/func/euclid.h>
#include <math/func/lie.h>
#include <math/func/spline.h>
#include <math/func/interp.h>

// #include <math/func/lambda.h>

struct test {
  
  double operator()(const double& x) const { return x; }
  double operator()(double&& x) const { 
    core::log("rvalue !");
    return x; 
  }

};



struct print_index {
  
  template<int I>
  void operator()() const {
    core::log(I);
  }

};


using namespace math;
using namespace func;


// #define macro_func_lambda(... )  \ 
// 									\
// typename lambda::range operator()(const typename lambda::domain& x) const {	\
//   constexpr auto impl = __VA_ARGS__ ;					\
//   return impl()(x);							\
// }									\
// 									\
// typename lambda::range operator()(typename lambda::domain&& x) const {	\
//   constexpr auto impl = __VA_ARGS__ ;					\
//   return impl()( std::move(x) );					\
// }
						\
// 									\
// struct push {								\
//   const typename lambda::domain at;					\
//   									\
//   push(const lambda& , const typename lambda::domain& at)		\
//     : at(at) { }							\
//   									\
//   typename math::lie::group<typename lambda::range>::algebra operator()	\
//   (const typename math::lie::group<typename lambda::domain>::algebra& v) const { \
//     constexpr auto lambda = __VA_ARGS__;				\
//     return math::func::d(lambda())(at)(v);				\
//   }									\
// 									\
// };									\
//   									\
  
// template<class U>
// struct michel : lambda<U, U> {
//   macro_func_lambda([]{ return id<U>(); } );
// };

template<class F>
void debug_type() {
  core::log( meta::name< F >() );
}

template<class F>
void debug_func() {
  core::log( meta::name< F >()," : ",  meta::name< typename traits<F>::domain >(), " -> ",  meta::name< typename traits<F>::range >() );
}







template<class> class T;

template<class A>
struct base {
  struct derived : base< T<A> > {

  };

};

template<class F>
void taiste( ... );

template<class F>
typename F::base::derived taiste(F * );

struct foo {

  vec3 operator()(const vec3& ) const {
    return {};
  }

};

int main(int, char** ){
  using namespace math;
  
  typedef std::tuple<real, real, real> test_type;
  test_type test;
  
  math::euclid::space< test_type > E;
  test = (*E).zero();

  ::tuple::each(test).apply( print_index{} );
  
  using namespace func;
  id< vec3 > a;

  auto c = a + (2.0 * a);
  
  func::any< real, real > f, g;
  typedef decltype(a + a) comp_type;

  debug_type< comp_type >();
  debug_type< traits<comp_type>::push>();
  debug_type< traits< traits<comp_type>::push >::pull >();

  // debug_type< traits< traits<comp_type>::pull >::push  >();

  typedef id<real> id_type;
  typedef math::func::euclid::sum<real> sum_type;
  typedef math::func::tuple_join< id_type, id_type > join_type;
  
  
  core::log("LHS");
 
  debug_func< traits< traits<sum_type>::push >::push  >();
  debug_func< traits< traits<sum_type>::pull >::push >();

  debug_func< traits< traits<sum_type>::push >::pull  >();
  debug_func< traits< traits<sum_type>::pull >::pull >();

  core::log("RHS");
  debug_func< traits< traits<join_type>::push >::push  >();
  debug_func< traits< traits<join_type>::pull >::push >();

  debug_func< traits< traits<join_type>::push >::pull  >();

  // this one is fucked up
  debug_func< traits<join_type>::pull >();
  debug_func< traits< traits<join_type>::pull >::pull >(); 

  core::log("");
  // debug_func< traits< traits<join_type>::push >::pull  >();

  
  core::log("");
  debug_func< traits< traits<comp_type>::push >::push >();
  // debug_func< traits< traits<comp_type>::push >::pull  >();

  // core::log( d(f)(1.0)(1.0) );
  
  g = std::move(f);
  // core::log( d(g)(1.0)(2.0) );

  auto zob = func::norm2( a );

  auto dzob = d(zob)( vec3::Zero() );
  auto Hzob = dT( dzob )( vec3::Zero() );
  
  foo b;
  any<vec3, vec3> h = b << b;
  
  f = id<real>() + id<real>();

  math::euclid::space<math::vec> taiste(2);

  // typedef ::base<int> base_type;

  // debug_type<base_type>();
  // debug_type<base_type::derived>();
  // debug_type<base_type::derived::base::derived>();
  // debug_type<base_type::derived::base::derived::base::derived>();

  // base_type::derived::base::derived x;

  // debug_type< decltype( taiste< decltype( taiste(&x) ) >(0) ) >();
  
  // lie::group<quat> H;
}
 
 
