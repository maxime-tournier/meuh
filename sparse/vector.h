#ifndef SPARSE_VECTOR_H
#define SPARSE_VECTOR_H

#include <sparse/offset.h>
#include <math/vec.h>

namespace sparse {
  
  template<class K>
  struct chunk< vector<K> > {
    typedef Eigen::Map< math::vec > type;
  };

  template<class K>
  struct chunk< const vector<K> > {
    typedef Eigen::Map< const math::vec > type;
  };
  
  typedef Eigen::Map<math::vec> vec_chunk;
  typedef Eigen::Map<const math::vec> const_vec_chunk;
  
  
  template<class K>
  class vector {
  public:
    typedef K key_type;

  private:
    std::vector<math::real> data;

    typedef sparse::offset<K> offset_type;
    offset_type offset;

    vec_chunk chunk(math::natural off, math::natural dim) {
      assert( dim );
      assert( (off + dim) <= data.size() );
      return {&data[off], dim};
    }

    const_vec_chunk chunk(math::natural off, math::natural dim) const {
      assert( dim );
      assert( (off + dim) <= data.size() );
      return {&data[off], dim};
    }


  public:
    
    vector(vector&& ) = default;
    vector(const vector& ) = default;
    
    vector& operator=(vector&& ) = default;
    vector& operator=(const vector& ) = default;
    
    const offset_type& keys() const { return offset; }

    vector(math::natural size = 0, math::natural dim = 0) 
      : offset( size ) {
      if( dim ) data.reserve( dim ); 
      else if( size ) data.reserve( size );
    }

    template<class Data>
    vector(const offset_type& offset, const Data& d )
      : data(),
	offset(offset)
    {
      if( offset.dim() ) {
	if( d.rows() ) {
	  data.resize( offset.dim() );
	  assert( d.rows() >= offset.dim() );
	  
	  for(math::natural i = 0, n = offset.dim(); i < n; ++i) {
	    data[ i ] = d(i);
	  }
	  
	} else data.resize(offset.dim(), 0.0);
      }
    }
    
    vec_chunk operator()(key_type k) {
      math::natural off = offset( k );
      if( off == data.size() ) {
	data.resize(offset.dim(), 0.0);
      }
	
      return this->chunk(off, dim(k));
    }

    const_vec_chunk operator()(key_type k) const {
      math::natural off = offset(k);
      return this->chunk(off, dim(k));
    }
    
    template<class F>
    void each(const F& fun) {
      offset.each([&](key_type k, math::natural off) {
	  fun(k, this->chunk(off, dim(k)));
	});
    }

    template<class F>
    void each(const F& fun) const {
      offset.each([&](key_type k, math::natural off) {
	  fun(k, this->chunk(off, dim(k)));
	});
    }
    

    template<class F>
    bool find(key_type k, const F& f) const {
      return offset.find(k, [&](math::natural off) {
	  f( this->chunk(off, dim(k)));
	});
    }
    
    template<class F>
    bool find(key_type k, const F& f) {
      return offset.find(k, [&](math::natural off) {
	  f( this->chunk(off, dim(k)));
	});
    }

    bool empty() const { return keys().empty(); }
        
    // direct dense access
    const_vec_chunk dense() const {
      return const_vec_chunk(&data[0], keys().dim());
    }

    vec_chunk dense() {
      return vec_chunk(&data[0], keys().dim());
    }

    
    void reserve( math::natural dim) {
      data.reserve( dim );
    }
    

    void clear() {
      *this = vector();
    }

    void zero() {
      dense().setZero();
    }

  }; 
}



#endif
