#ifndef SPARSE_MATRIX_H
#define SPARSE_MATRIX_H

#include <sparse/vector.h>
#include <math/mat.h>

#include <boost/functional/hash.hpp>
#include <core/tuple/iter.h>

namespace sparse {
  
  typedef Eigen::Map<math::mat> mat_chunk;
  typedef Eigen::Map<const math::mat> const_mat_chunk;
  

  template<class K1, class K2>
  struct chunk< matrix<K1, K2> > {
    typedef mat_chunk type;
  };

  template<class K1, class K2>
  struct chunk< const matrix<K1, K2> > {
    typedef const_mat_chunk type;
  };
  

  template<class Row, class Col>
  class matrix {
  public:  
    typedef Row row_type;
    typedef Col col_type;

  private:
    typedef std::tuple<row_type, col_type> key_type;

    typedef vector<key_type> data_type;
    data_type data;
    
    offset<row_type> _rows;
    offset<col_type> _cols;
    
    static mat_chunk chunk(key_type k, vec_chunk v) {
      return mat_chunk(v.data(), dim( std::get<0>(k)), dim( std::get<1>(k)) );
    }

    static const_mat_chunk const_chunk(key_type k, const_vec_chunk v) {
      return const_mat_chunk(v.data(), dim( std::get<0>(k)), dim( std::get<1>(k)) );
    }
    
  public:
    matrix(const matrix& ) = default;
    matrix& operator=(const matrix& ) = default;

    matrix(matrix&& ) = default;
    matrix& operator=(matrix&& ) = default;
    
    matrix(math::natural size = 0) 
      : data(size), _rows(size), _cols(size) { }
    
    

    mat_chunk operator()(row_type r, col_type c) {
      return operator()( std::make_tuple(r, c) ); 
    }

    const_mat_chunk operator()(row_type r, col_type c) const {
      return operator()( std::make_tuple(r, c) ); 
    }
      
    mat_chunk operator()(key_type k) {
      _rows( std::get<0>(k) );
      _cols( std::get<1>(k) );
      return chunk(k, data(k));
    }


    const_mat_chunk operator()(key_type k) const {
      return const_chunk(k, data(k));
    }
    

    // convenience for diagonal matrices:
    mat_chunk operator()(row_type r) {
      return operator()(r, r);
    }

    const_mat_chunk operator()(row_type r) const {
      return operator()(r, r);
    }
    
    // iterator
    template<class F>
    void each(const F& f) {
      
      data.each([&](key_type k, vec_chunk v) {
	  f(std::get<0>(k), std::get<1>(k), this->chunk(k, v));
	});
    }

    template<class F>
    void each(const F& f) const {
      
      data.each([&](key_type k, const_vec_chunk v) {
	  f(std::get<0>(k), std::get<1>(k), this->const_chunk(k, v));
	});
      
    }
    

    const offset<key_type>& keys() const { return data.keys(); } 

    const offset<row_type>& rows() const { return _rows; }
    const offset<col_type>& cols() const { return _cols; }
    
    bool empty() const { return data.empty(); }


    // TODO work this a bit
    typedef typename storage<Row, std::vector<Col> >::type index_type;

    index_type index() const {

      index_type res( keys().size() );
      each([&](Row r, Col c, const_mat_chunk) {
	  res[r].push_back(c);
	});

      return res;
    }
    

    void reserve(math::natural dim) { data.reserve( dim ); }
  };




  // free functions    
  template<class Row, class Col>
  struct traits< std::tuple<Row, Col> > {
    static math::natural dimension(const std::tuple<Row, Col>& p) {
      return dim( std::get<0>(p) ) * dim( std::get<1>(p)); 
    }
  };
    

  


}

namespace std {

  template<class ... Args>
  struct hash< std::tuple<Args...> > {
    
    struct helper {
      size_t& seed;

      template<class A>
      void operator()(const A& a) const {
	boost::hash_combine(seed, a);
      }
      
    };

    size_t operator()(const std::tuple<Args...> &t) const {
      size_t seed = 0;
      core::tuple::iter( helper{seed}, t );
      return seed;
    }
  };

}

#endif

