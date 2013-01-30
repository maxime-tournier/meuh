#ifndef SPARSE_LINEAR_TRANSPOSE_H
#define SPARSE_LINEAR_TRANSPOSE_H

#include <map>
#include <cassert>

#include <core/each.h>
#include <core/range/stl.h>
#include <sparse/zero.h>
#include <sparse/iter.h>

namespace sparse {
  namespace linear {

    template<class K1, class K2>
    struct transpose {
      typedef std::map<K2, math::mat> row_type;
      typedef std::map<K1, row_type> data_type;

      const data_type& data;

      typedef std::map<K1, math::vec> col_vec;
      typedef std::map<K2, math::vec> row_vec;
      
      transpose(const data_type& data) : data(data) {
	assert( consistent(data) );
      }


      struct helper {

	row_vec& res;
	
	helper(row_vec& res):res(res) { }

	void operator()(K1, const row_type& row, const math::vec& x) const {

	  auto it = res.begin();
	  
	  core::each(row, [&]( K2 i, const math::mat& M) {
	      
	      // inserts if not already here
	      it = this->res.insert(it, std::pair<K2, math::vec>(i, math::vec() ));
		
	      if( it->second.empty() )  it->second = M.transpose() * x;
	      else it->second += M.transpose() * x;
	    });
	  
	}

	void operator()(K1, const zero&, const math::vec& ) const { }
	void operator()(K1, const row_type&, const zero& ) const { }
	

      };


      // matrix vector product
      row_vec operator*(const col_vec& x) const {
	
	row_vec res;
	
	// sparse::iter( core::range::all(data), core::range::all(x), helper(res) );
	sparse::iter_nonzero( data, x, helper(res) );
	return res;
      }

      

      std::map<K2, std::map<K1, math::mat> > full() const {
	std::map<K2, std::map<K1, math::mat> > res;

	core::each(data, [&](K1 k1, const std::map<K2, math::mat>& row) {
	    core::each(row, [&](K2 k2, const math::mat& chunk) {
		res[k2][k1] = chunk.transpose();
	      });
	  });
	
	return res;
      }
      
    };

    
    template<class K1, class K2>
    transpose<K1, K2> transp(const std::map<K1, std::map<K2, math::mat> >& data) {
      return {data};
    }


    template<class K1, class K2>
    std::map<K1, std::map<K1, math::mat> > operator*(const std::map<K1, std::map<K2, math::mat> >& lhs,
						     const transpose<K1, K2>& rhs) {
      std::map<K1, std::map<K1, math::mat> > res;
      
      core::each(lhs, [&](K1 lk1, const std::map<K2, math::mat>& lrow) {

	  core::each(rhs.data, [&](K1 rk1, const std::map<K2, math::mat>& rrow) {
	      

	      struct dot { 
		
		math::mat& total;
		
		void operator()(K2, const math::mat& left, const math::mat& right) const {
		  total += left * right.transpose();
		}
		
		void operator()(K2, const math::mat& , sparse::zero ) const {} 
		void operator()(K2, sparse::zero, const math::mat&  ) const {} 

	      };

	       
	      
	      math::mat& chunk = res[ lk1 ][ rk1 ];
	      chunk = math::mat::Zero( dim(lk1), dim(rk1));
	      
	      sparse::iter(core::range::all(lrow), core::range::all(rrow),
			   dot{ chunk } );
	       
	    });
	  

	});


      return res;
    }


   
  }
}


#endif
