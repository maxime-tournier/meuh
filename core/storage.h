#ifndef CORE_STORAGE_H
#define CORE_STORAGE_H

#include <vector>
#include <core/uint.h>

namespace core {

  template<class T>
  struct chunk {
    T* begin;
    T* end;
    
    chunk(T* begin, T* end)
      : begin(begin),
	end(end) {
      assert( end >= begin );
    }
    
    uint dim() const { return end - begin; }
  };
  

  template<class T>
  class storage {

    std::vector<uint> offset;
    std::vector<T> data;
    
  public:

    storage() = default;
    storage( const storage& ) = default;
    storage( storage&& ) = default;
    
    void fill(T value) { std::fill(data.begin(), data.end(), value); }

    uint size() const { return offset.size(); }
    uint dim() const { return data.size(); }

    uint off( uint handle ) const { 
      assert(handle < size() );
      return offset[handle];
    }

    T* ptr( uint handle ) {
      return &data[ off(handle) ];
    }

    const T* ptr( uint handle ) const {
      return &data[ off(handle) ];
    }
    
    bool empty() const { return size() == 0; }


    uint add(uint dim) { 
      assert( dim );
      
      uint res = offset.size();
      offset.push_back( data.size() );
      if( dim ) data.resize( data.size() + dim );
      
      return res;
    }

    
    uint dim( uint handle ) const {
      assert( !empty() );
      
      if( handle < (size() - 1) ){
	return off( handle + 1 ) - off( handle );
      } else {
	return dim() - off( handle );
      }
    }


    void reserve(uint size, uint dim) { 
      offset.reserve( size );
      data.reserve( dim ); 
    }
    
    
    chunk<T> operator()(uint handle) { 
      auto p = ptr(handle);

      return { p, p + dim(handle) };
    }
    
    chunk<const T> operator()(uint handle) const { 
      auto p = ptr(handle);
      
      return { p, p + dim(handle) };
    }
    
  };



}


#endif
