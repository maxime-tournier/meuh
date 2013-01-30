#ifndef CORE_REQUIRES_H
#define CORE_REQUIRES_H

#include <core/concept.h>

namespace core {

  // namespace impl {

  //   // if you get undefined type here, you probably need to derive
  //   // your conceptmap from concept::map
  //   template<class Concept, class = concept_map::tag >
  //   struct requires_one;
      
  //   template<class Concept>
  //   struct requires_one<Concept, typename concept<Concept>::concept_map::tag > {
  //     typedef enabler type;

      
  //   };
    
  //   template< void(*)()  ... > 
  //   struct trigger {
  //     typedef enabler type;
  //   };

  //   template<class Concept>
  //   void check() {
  //     throw "this is not supposed to ever be called";
  //     Concept* tmp;
  //     delete tmp;		// put requirements in concept dtor
  //   }

    
  //   template<class Concepts, class = sfinae<> >
  //   struct requires { };

  //   template<class ... Concepts>
  //   struct requires< sfinae<Concepts...> , 
  // 		     typename sfinae< typename requires_one<Concepts>::type...>::type  > {
  //     typedef typename trigger<&check<Concepts>...>::type type;
	
  //     template<class Ret>
  //     struct ret { typedef Ret type; };
	
  //   };
      
  // }

  
  
  // template<class ... Concepts>
  // struct requires : impl::requires< sfinae<Concepts...> > { };
  


}


#endif
