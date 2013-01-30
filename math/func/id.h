#ifndef MATH_FUNC_ID_H
#define MATH_FUNC_ID_H


namespace math {

  namespace func {
    
    template<class A>
    struct id {
      
      typedef A domain;
      typedef A range;
      
      A operator()(const A& a) const { return a; }
      A operator()(A&& a) const { return std::move(a); }
      
      struct push : base< id< typename math::lie::group<A>::algebra > > {
	push(const id&, const domain& ) { }
      };

      struct pull : base< id< typename math::lie::group<A>::coalgebra > >{
	pull(const id&, const domain& ) { }
      };

    };

  }


}
  
#endif
