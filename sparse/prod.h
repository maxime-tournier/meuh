#ifndef SPARSE_PROD_H
#define SPARSE_PROD_H

#include <sparse/vector.h>
#include <sparse/matrix.h>
#include <sparse/transp.h>

namespace sparse {



  template<class Row, class Col> 
  void prod(vector<Row>& out, 
	    const matrix<Row, Col>& m,
	    const vector<Col>& v) {
    assert( out.keys().dim() == m.rows().dim() );
    
    out.zero();
    
    m.each([&](Row r, Col c, const_mat_chunk m) {
	v.find(c, [&](const_vec_chunk v) {
	    out(r).noalias() += m * v;
	  });
      });
    
  }

  
  template<class Row, class Col>
  vector<Row> operator*(const matrix<Row, Col>& m,
			const vector<Col>& v) {
    vector<Row> res(m.rows(), math::vec() );
    
    prod(res, m, v);    
    
    return res;
  }
  

  template<class Row, class Col>
  void prod(vector<Col>& out, 
	    const transpose<Row, Col>& m,
	    const vector<Row>& v) {
    out.zero();
    
    m.each([&](Col c, Row r, const_transp_mat_chunk m) {
	v.find(r, [&](const_vec_chunk v) {
	    out(c).noalias() += m * v;
	  });
      });
  }


  template<class Row, class Col>
  vector<Col> operator*(const transpose<Row, Col>& m,
			const vector<Row>& v) {
    vector<Col> res(m.rows(), math::vec());
    
    prod(res, m, v);
    
    return res;
  }

}


#endif
