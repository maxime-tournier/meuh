#ifndef PHYS_SPARSE_STORAGE_H
#define PHYS_SPARSE_STORAGE_H

#include <core/find.h>
#include <core/foreach.h>
#include <phys/sparse/chunk.h>

namespace phys {
  namespace sparse {


    template<class Key, class Value>
    class storage {
    protected:
      typedef storage base;
      
      typedef std::map<Key, math::natural> offsets_type;
      offsets_type offsets;
      std::vector<math::real> data; // TODO math::vec ?
      
    public:
      typedef sparse::chunk<Value> chunk;

      void reserve(math::natural n ) { data.reserve(n); }
      
      typename chunk::type operator()(Key k) { 
	auto res = offsets.insert( std::make_pair(k, data.size() ) );
	
	if(res.second) {	// we did insert
	  data.resize( data.size() + chunk::size(k), 0 );
	}
	
	return chunk::get(k, &data[ res.first->second ]);
      }


      const typename chunk::type operator()(Key k) const { 
	// TODO avoid creating exception at each call ?
	return chunk::get(k, &data[ core::find(offsets, k, "key not found" )->second ]);
      }

      
      template<class Action>
      void each(const Action& action) {
	using namespace core;
	foreach(all(offsets), [&](const typename offsets_type::value_type& i ) {
	    action(i.first, chunk::get(i.first, &this->data[i.second] ) );
	  });
      };
      
    };


  }
}


#endif
