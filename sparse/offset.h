#ifndef SPARSE_OFFSET_H
#define SPARSE_OFFSET_H

#include <core/pair.h>
#include <sparse/dim.h>
#include <sparse/storage.h>
#include <core/noop.h>

// TODO
#include <core/error.h>
#include <core/log.h> 

#include <unordered_map>
#include <core/stl.h>

#include <core/range/iterator.h>
#include <core/pair.h>

namespace sparse {
  
  template<class K>
  class offset {
    typedef math::natural value_type;
    typedef K key_type;

    typedef typename storage<key_type, value_type>::type data_type;
    data_type data;
    
    typedef typename data_type::iterator iterator;
    typedef typename data_type::const_iterator const_iterator;
    
    value_type dimension;
    
    typedef std::pair< iterator, iterator > range_type;
    range_type range() { return {data.begin(), data.end()}; }
      
    typedef std::pair< const_iterator, const_iterator > const_range_type;
    const_range_type range() const { return {data.begin(), data.end()}; }

  public:

    offset(offset&&) = default;
    offset(const offset& ) = default;

    offset& operator=(const offset& ) = default;
    offset& operator=(offset&& ) = default;

    offset(value_type size = 0) 
      : data(),
	dimension(0) { 
      rehash( size );
    }

    void rehash( value_type size ) {
      if( size ) data.rehash( size );
    }
    
    value_type size() const { return data.size();  }
    
    value_type dim() const { 
      return dimension;
    }
    
    value_type operator()(key_type k) {
      assert( sparse::dim( k ) );
      auto it = data.insert( std::make_pair(k, dim()) );
      
      if(it.second) {
	dimension += sparse::dim(k);
      }

      return it.first->second;
    }

    value_type operator()(key_type k) const {
      assert( sparse::dim( k ) );
      auto it = data.find(k);
      
      if( it == data.end() ) {
	throw core::error("key not found");
      } else {
	return it->second;
      }

    }
      
 
    
    template<class F>
    void each(const F& f) const {
      core::each(range(), f);
    }

    
    template<class F>
    bool find(key_type k, const F& f) const {
      auto it = data.find(k);
      if( it != data.end() ) { f(it->second); return true; } 
      return false;
    }
    
    bool find(key_type ) const { return find( core::noop() ); }

    bool empty() const { return data.empty(); }
  };


}


#endif
