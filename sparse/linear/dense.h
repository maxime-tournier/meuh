#ifndef SPARSE_LINEAR_DENSE_H
#define SPARSE_LINEAR_DENSE_H

#include <sparse/insert.h>
#include <sparse/iter.h>
#include <sparse/zip.h>
#include <sparse/map.h>

#include <sparse/linear/dimension.h>
#include <sparse/linear/numbering.h>
#include <sparse/linear/element.h>
#include <sparse/linear/consistent.h>

#include <math/vec.h>
#include <core/error.h>

#include <set>

namespace sparse {
  namespace linear {

    // dense conversions
  
    // TODO make exception instead of asserts ?
    template<class Key>
    std::map<const Key*, math::vec> vector(const numbering<Key>& num, const math::vec& data) {
      assert(data.rows() == int(num.dim()) && "wrong vector size" );
      std::map<const Key*, math::vec> res;
      
    
      for(auto off = num.offsets(); !off.empty(); off.pop()) {
	sparse::insert<const Key*, math::vec>(res, off.front().first, 
					      data.segment(off.front().second, 
							   dim(off.front().first) ), res.end() );
      }
      
      return res;
    }

    // quadratic !
    template<class K1, class K2>
    std::map<const K1*, std::map<const K2*, math::mat> > matrix(const numbering<K1>& num1,
								const numbering<K2>& num2,
								const math::mat& data) {

      std::map<const K1*, std::map<const K2*, math::mat> > res;
      
      num1.each( [&](const K1* k1, math::natural row) {
	  num2.each( [&](const K2* k2, math::natural col) {
	      auto block = data.block(row, col, dim(k1), dim(k2));
	      if( (block.array() != 0.0).any() ) {
		// TODO optimize insertion
		res[ k1 ] [ k2 ] = block;
	      }
	    });
	});

      return res;
    }

    



    namespace helper {

      template<class K, class Data>
      struct update_sparse {

	Data&& data;
	
	update_sparse(Data&& data) : data( std::forward<Data>(data)) { }
	
	void operator()(const K* k, math::vec& res, math::natural off) const {
	  res = data.segment(off, dim(k) );
	} 

	void operator()(const K* , sparse::zero, math::natural  ) const {
	  throw core::error("element insertion not implemented");
	}
	
	
	void operator()(const K* ,  math::vec&, sparse::zero ) const {
	  // TODO is this an error ? this is a block that isn't numbered
	  throw core::error("element removal not implemented");
	}

	


      };


      template<class K>
      struct update_dense {

	math::vec& res;
	
	update_dense(math::vec& res) : res(res) { }
	
	void operator()(const K* k, math::natural off, const math::vec& block) const {
	  res.segment(off, dim(k)).noalias() = block;
	} 

	void operator()(const K* k, math::natural off, sparse::zero  ) const {
	  res.segment(off, dim(k) ).setZero();
	}
	
	
	void operator()(const K* ,  sparse::zero , const math::vec&) const { }

	


      };


    }


    // in place sparse/dense conversion. use for allocation critical
    // tasks.
    template<class Key, class Data>
    void update(std::map<const Key*, math::vec>& res, const numbering<Key>& num, Data&& data) {
      assert(data.rows() == int(num.dim()) && "wrong vector size" );
      
      sparse::iter( core::range::all(res), num.offsets(), 
		    helper::update_sparse<Key, Data>( std::forward<Data>(data)) );
      
    }



    template<class Key>
    void update(math::vec& res, const numbering<Key>& num, const std::map<const Key*, math::vec>& data) {
      assert(res.rows() == int(num.dim()) && "wrong vector size" );
      
      sparse::iter( num.offsets(), core::range::all(data), 
		    helper::update_dense<Key>(res));
      
    }






    // TODO move
    namespace helper  {
      struct vector_filler {
	math::vec& result;

	vector_filler(math::vec& result) : result(result) { }

	template<class Key>
	void operator()(const Key* k, unsigned int offset, const math::vec& segment) const {
	  assert(segment.rows() == int(dim(k))  );
	  result.segment(offset, dim(k) ).noalias() = segment;
	}

	// don't do anything when either one is zero
	template<class K, class A>
	void operator()(const K*, const A&, const sparse::zero& ) const {  }
	
	template<class K, class A>
	void operator()(const K*, const sparse::zero&, const A& ) const {  }

      };
    
  
    
      template<class Key>
      math::vec safe_dense(const numbering<Key>& num, const std::map<const Key*, math::vec>& data) {
	assert(num.dim()  && "wont work if num is empty");
	assert( consistent(data) );
	// if( !sparse::consistent(data) ) throw core::exception( macro_here + "~caca !" );

	math::vec res = math::vec::Zero(num.dim());
	
	using namespace core;
	for(auto offsets = num.offsets(); !offsets.empty(); offsets.pop() ) {
	  Key* key = offsets.front().first;
	  math::natural off = offsets.front().second;
	  res.segment( off, dim(key) ) = element(data, key);
	}
	
	return res;
      }

      template<class Key>
      math::vec fast_dense(const numbering<Key>& num, const std::map<const Key*, math::vec>& data) {
	assert(num.dim()  && "wont work if num is empty");
	assert( consistent(data) );
	
	math::vec res = math::vec::Zero(num.dim());
	
	sparse::iter(num.offsets(), core::range::all(data),
		     vector_filler(res) );

	return res;
      }



    }

    
    
    template<class Key>
    math::vec dense(const numbering<Key>& num, const std::map<const Key*, math::vec>& data) {
      return helper::fast_dense(num, data);
    }

    // convenience
    template<class Key>
    math::vec dense( const std::map<const Key*, math::vec>& data ) {
      return dense( numbering<Key>( sparse::keys(data) ), data);
    }


    // TODO move outta the way
    namespace helper {


      template<class Row>
      struct row_filler {
	mutable Row row;
      
	template<class K2>
	void operator()(const K2* k2, const math::natural& offset, const math::mat& block) const {
	  // std::cout << HERE << " row filler got one block: " << std::endl
	  // 	    << block << std::endl;
	  row.block(0, offset, row.rows(),  dim(k2) ) = block;
	}
	
	template<class K, class A, class B>
	void operator()(const K*, const A&, const B& ) const { 
	  // std::cout << HERE << " ALARM !!! " << typeid(A).name() << " " << typeid(B).name() << std::endl;
	}
	
      };

      template<class Row>
      row_filler<Row> make_row_filler(const Row& row) { return {row}; }
    
      template<class K2>
      struct matrix_filler {
	math::mat& result;
	const numbering<K2>& n2;
	
	template<class K1>
	void operator()(const K1* k1, unsigned int offset,
			const std::map<const K2*, math::mat>& row) const {
	  // std::cout << HERE << " matrix filler processing row: " << std::endl
	  // 	     << row.begin()->second << std::endl;

	  // TODO improve by starting/finishing closer to n1 in case data
	  // is bigger than needed
	  sparse::iter(n2.offsets(), core::range::all(row), 
		       make_row_filler( result.block(offset, 0, dim(k1), result.cols() ) ) );
	
	}

	template<class K, class A, class B>
	void operator()(const K*, const A&, const B& ) const {  
	  // std::cout << HERE << " ALARM matrixfiller !" << std::endl;
	}

      };

      template<class K2>
      matrix_filler<K2> make_matrix_filler(math::mat& res, const numbering<K2>& n2) {
	return {res, n2};
      }

    }


    // converts a sparse matrix to a dense one, given numberings
    // K1 is row index, K2 is cols
    template<class K1, class K2>
    math::mat dense(const numbering<K1>& n1, const numbering<K2>& n2, 
		    const std::map<const K1*, std::map<const K2*, math::mat> >& data ) {
      assert( n1.dim() && n2.dim() ); // TODO this should be an exception
      math::mat res = math::mat::Zero(n1.dim(), n2.dim() );
      
      // TODO improve by starting/finishing closer to n1 in case data
      // is bigger than needed
      auto offsets = n1.offsets();
      sparse::iter(offsets, core::range::all(data),
		   helper::make_matrix_filler(res, n2) );
      return res;
    }


  
    // numbers dimensions of a sparse matrix
    template<class K1, class K2, class C>
    void number(numbering<K1>& n1,
		numbering<K2>& n2,
		const std::map<const K1*, std::map<const K2*, C> >& matrix) {
      n1 = numbering<K1>( sparse::keys( matrix ) );
      std::set<const K2*> k2;
      
      typename std::map<const K1*, std::map<const K2*, C> >::const_iterator row, erow;
      for(row = matrix.begin(), erow = matrix.end(); row != erow; ++row) {

	for( auto keys = sparse::keys(row->second); !keys.empty(); keys.pop()) {
	  k2.insert(keys.front());
	}
	
      }
      
      n2 = numbering<K2>( core::range::all(k2) );

    }
    
    
    // convenience
    template<class K1, class K2>
    math::mat dense(const std::map<const K1*, std::map<const K2*, math::mat> >& data ) {
      numbering<K1> n1;
      numbering<K2> n2;
      number(n1, n2, data);
      return dense(n1, n2, data);
    }


    
    template<class Key>
    typename numbering<Key>::sparse_vector numbering<Key>::sparse(const dense_vector& v) const {
      return linear::vector(*this, v);
    }


    template<class Key>
    typename numbering<Key>::dense_vector numbering<Key>::dense(const sparse_vector& s) const {
      return linear::dense(*this, s);
    }



    template<class Key>
    typename numbering<Key>::sparse_vector numbering<Key>::zero() const {
      sparse_vector res;
      auto it = res.begin();
      core::each(offsets_, [&](key_type k, math::natural) {
	    
	  it = res.insert(it, std::make_pair(k, math::vec::Zero(dim(k))));
	});
      
      return res;
    }

  }
}

#endif
