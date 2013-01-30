#ifndef SPARSE_WEIGHT_H
#define SPARSE_WEIGHT_H

#include <sparse/storage.h>
#include <core/stl.h>

namespace sparse {

  template<class Key>
  class weight {
    typedef math::real value_type;
    typedef Key key_type;
    
    typedef typename storage<key_type, value_type>::type data_type;
    data_type data;
    
  public:
    weight(math::natural size = 0) : data(size) { }
    
    value_type& operator()(key_type k) { 
      auto it = data.find(k);

      if(it == data.end()) it = data.insert(it, std::make_pair(k, 0.0) );

      return it->second;
    }

    value_type operator()(key_type k) const { 
      auto it = data.find(k);
      if(it == data.end() ) return 0.0;
      else return it->second;
    }

    
    template<class F>
    void each(const F& f) const {
      core::each( core::all(data), [&](key_type k, value_type v) {
	  f(k, v);
	});
    }
    
  };

}


#endif
