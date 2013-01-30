#ifndef SPARSE_TRANSP_H
#define SPARSE_TRANSP_H

#include <sparse/matrix.h>

namespace sparse {

  typedef Eigen::Transpose< mat_chunk > transp_mat_chunk;
  typedef Eigen::Transpose< const_mat_chunk > const_transp_mat_chunk;
  
  template<class Row, class Col>
  struct transpose {
    const matrix<Row, Col>& data;

    typedef Row row_type;
    typedef Col col_type;
    
    transpose(const matrix<Row, Col>& data) 
      : data(data) { }

    typedef const_transp_mat_chunk value_type;
    
    value_type operator()(col_type c, row_type r) const {
      return data(r, c).transpose();
    }

    
    template<class F>
    void each(const F& fun) const {
      
      data.each([&](row_type r, col_type c, const_mat_chunk m) {
	  fun(c, r, m.transpose() );
	});
      
    }

    // transposed :)
    const offset<col_type>& rows() const { return data.cols(); }
    const offset<row_type>& cols() const { return data.rows(); }
  };


  template<class Row, class Col>
  transpose<Row, Col> transp(const matrix<Row, Col>& mat) { return {mat}; }
  
}


#endif
