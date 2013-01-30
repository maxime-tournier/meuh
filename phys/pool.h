#ifndef PHYS_POOL_H
#define PHYS_POOL_H

#include <core/type_map.h>
#include <tuple/pow.h>
#include <tuple/index.h>
#include <tuple/traits.h>

#include <core/callback.h>

#include <phys/object.h>

#include <core/stl.h>


namespace phys {


  template<class ... Operations>
  class pool {

    typedef std::tuple< std::function< void(const Operations& ) >...> callback_type;
    callback_type callback;
    
    typedef typename ::tuple::traits< callback_type >::range_type each;

    // objects storage
    core::type_map storage;
    
    // helper types
    template<class G>
    struct traits {
      typedef object<G> object_type;
      typedef object_type* key_type;
      
      typedef std::vector< key_type > container_type;
    };


    template<class G>
    struct add_operations {
      
      pool<Operations...>* p;
      
      template<int I>
      void operator()() const {
	
	// for each operation, we add the application of the operation
	// on each G element
	typedef typename std::tuple_element<I, std::tuple<Operations...> >::type op_type;
	
	// this is to keep gcc 4.7 happy
	auto p = this->p;
	
	core::push_back( std::get<I>(p->callback), [p](const op_type& op) {
	    core::each( p->template obj<G>(), [&](typename traits<G>::key_type key) {
		op( key );
	      });
	  });

      };

    }; 

    // access container for objects of type G
    template<class G>
    typename traits<G>::container_type& obj() { 
      bool inserted;
      auto& res = this->storage.template get<typename traits<G>::container_type>(&inserted);
      
      if( inserted ) {
	// add corresponding operations to callbacks
	each::apply( add_operations<G>{this} );
      }
      
      return res;
    }
    
    template<class G>
    const typename traits<G>::container_type& obj() const { 
      return this->storage.template get<typename traits<G>::container_type>();
    }

  public:
    

    // add a given object 
    template<class G>
    void add( object<G>* o ) {
      obj<G>().push_back(o);
    }

    // apply the given operation
    template<class Op>
    void apply(const Op& op) const {
      auto& cb = std::get< tuple::index<Op, std::tuple<Operations...> >::value >(callback);
      if( cb ) cb(op);
    }


    void clear() {
      storage.clear();
      callback = callback_type{};
    };
    
  };


  
}


#endif
