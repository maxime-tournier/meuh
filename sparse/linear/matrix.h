#ifndef SPARSE_LINEAR_MATRIX_H
#define SPARSE_LINEAR_MATRIX_H

#include <math/mat.h>
#include <sparse/linear/dimension.h>

namespace sparse {
  namespace linear {

    template<class Row, class Col>
    class matrix {
      
      typedef std::pair<Row, Col> key_type;
      vector<key_type> data;
      
      typedef Eigen::Map<math::mat> value_type;
    public:
      
      matrix(math::natural size = 0) : data(size) { }

      value_type operator()(Row r, Col c) {
	return operator()( std::make_pair(r, c) ); 
      }
      
      value_type operator()(key_type k) {
	auto tmp = data(k);
	return value_type(tmp.data(), dim(k.first), dim(k.second) );
      }
      
      void clear() { data.clear(); }
      void size() { return data.size(); }
    };
      
    
    
    template<class Row, class Col>
    struct traits< std::pair<Row, Col> > {
      static math::natural dimension(const std::pair<Row, Col>& p) {
	return dim(p.first) * dim(p.second); 
      }
    };
    
  }
}

#endif

