#ifndef CONST_VECTOR_H
#define CONST_VECTOR_H

#include <cassert>

namespace core {

  // a vector holding const members
  // TODO customize allocator
  template<class U>
  class const_vector {
  public:
    typedef unsigned int size_t;

    const_vector(size_t n) 
      : n(n), 
	data( n * sizeof(U) ) { 
      assert( n );
    }
    
    const_vector(const const_vector& other) 
      : n(other.n), 
    	data( n * sizeof(U) ) {
      for(size_t i = 0; i < n; ++i) {
    	set(i, other(i));
      }
    }
    
    // const_vector(const const_vector& ) = default;
    const_vector(const_vector&& other) 
      : n(other.n),
	data( std::move(other.data) ) {
      other.n = 0;
    }

    // const_vector(const_vector&& other) 
    //   : n(0),
    // 	data(0) {
    //   swap(other);
    // }

    ~const_vector() { 
      
      if( !n ) return;
      
      for(U* ptr = at(0); ptr < at(n); ++ptr) {
	ptr->~U();
      }

      // delete [] data;
    }
    
    size_t size() const { return n; }
    
    const U& operator()(size_t i) const { 
      assert( i < size() );
      return *at(i); 
    }

    U& operator()(size_t i) { 
      assert( i < size() );
      return *at(i); 
    }

    template<class ... Args>
    const U& set(size_t i, Args&& ... args) {
      U* bob = new( at(i) ) U( std::forward<Args>(args)... );
      return *bob;
    };
    

    void swap( const_vector& other ) {
      assert( this != &other );
      
      size_t other_n = other.n;
      char* other_data = other.data;
      
      other.n = n;
      other.data = data;

      n = other_n;
      data = other_data;
    }

    template<class F>
    void each(const F& f) {
      for(unsigned int i = 0, n = size(); i < n; ++i) f(i); 
    }
    

    template<class F>
    static const_vector map(const F& f, size_t n) {
      const_vector res(n);
      // TODO should we call dtor first lol ?
      res.each([&](size_t i) { res.set(i, f(i)); } );
      return res;
    }
    
    
  private:
    size_t n;
    std::vector<char> data;
    
    U* at(size_t i) const { return ((U*)&data.front()) + i; }
  };
  

  // convenience
  template<class F>
  const_vector< typename std::result_of< F(unsigned in) >::type > make_const_vector(unsigned int n, const F& f) {
    return const_vector< typename std::result_of< F(unsigned in) >::type >::map(f, n);
  }
  


}


#endif
