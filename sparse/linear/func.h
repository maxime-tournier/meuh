#ifndef SPARSE_LINEAR_FUNC_H
#define SPARSE_LINEAR_FUNC_H

#include <sparse/zero.h>

#include <math/vec.h>
#include <math/matrix.h>

#include <math/svd.h>

namespace sparse {

  namespace linear {
    namespace func {
      
      struct sum {
	
	math::vec operator()(const math::vec& a, const math::vec& b) const {
	  return a + b;
	}

	math::vec operator()(const zero& , const math::vec& b) const {
	  return b;
	}

	math::vec operator()(const math::vec& a, const zero& ) const {
	  return a;
	}
    

	math::mat operator()(const math::mat& a, const math::mat& b) const {
	  return a + b;
	}

	math::mat operator()(const zero& , const math::mat& b) const {
	  return b;
	}

	math::mat operator()(const math::mat& a, const zero& ) const {
	  return a;
	}
	
      };


      template<class K>
      struct plus_equal {
	
	plus_equal(std::map<K, math::vec>& lhs) : lhs(lhs) { }

	std::map<K, math::vec>& lhs;
	mutable std::map<K, math::vec> to_add;
	
	~plus_equal() { lhs.insert( to_add.begin(), to_add.end() ); }

	void operator()(K, math::vec& lhs, const math::vec& rhs) const {
	  lhs += rhs;
	}
	
	void operator()(K, math::vec&, zero ) const {	}
	
	void operator()(K k, zero, const math::vec& rhs) const {
	  to_add[ k ] = rhs;
	}

      };

       template<class K>
      struct less_equal {
	
	 less_equal(std::map<K, math::vec>& lhs) : lhs(lhs) { }
	 
	 std::map<K, math::vec>& lhs;
	 mutable std::map<K, math::vec> to_add;
	 
	 ~less_equal() { lhs.insert( to_add.begin(), to_add.end() ); }
	 
	 void operator()(K, math::vec& lhs, const math::vec& rhs) const {
	   lhs -= rhs;
	 }
	
	void operator()(K, math::vec&, zero ) const {	}
	
	void operator()(K k, zero, const math::vec& rhs) const {
	  to_add[ k ] = -rhs;
	}

      };


      struct scalar {
	const math::real lambda;
	
	// TODO optimize return type
	math::vec operator()(const math::vec& x) const {
	  return lambda * x;
	};
	
      };


      struct dot {
	math::real& res;

	template<class Key>
	void operator()(Key , const math::vec& x, const math::vec& y) const {
	  res += x.dot(y);
	}
	
	template<class Key>
	void operator()(Key , const math::vec& , const zero& ) const { }

	template<class Key>
	void operator()(Key , const zero& , const math::vec&  ) const { }
	
	
      };


      struct diff {
	
	math::vec operator()(const math::vec& a, const math::vec& b) const {
	  return a - b;
	}

	math::vec operator()(const zero& , const math::vec& b) const {
	  return -b;
	}

	math::vec operator()(const math::vec& a, const zero& ) const {
	  return a;
	}
    
      };


      struct product {

	math::vec operator()(const math::mat& m, const math::vec& v) const { return m * v; }
    
	template<class A>
	zero operator()(const A&, const zero& ) const { return zero(); }

	template<class A>
	zero operator()(const zero&, const A&) const { return zero(); }


	
	math::mat operator()(const math::mat& lhs, const math::mat& rhs) const { return lhs * rhs; }
	zero operator()(const math::mat& , zero ) const { return zero(); }
	zero operator()(zero, const math::mat& ) const { return zero(); }
	
    
      };

      template<class Res>
      struct product_iter {

	Res& res;
	
	product_iter(math::vec& res) : res(res) { assert(res.rows()); }
	
	template<class K>
	void operator()(K , const math::mat& m, const Res& v) const { 
	  assert( res.rows() == m.rows() );
	  assert( m.cols() == v.rows() );
	  assert( res.cols() == v.cols() );
	  
	  res.noalias() += m * v; 
	}
	
	template<class K, class A>
	void operator()(K , const A&, const zero& ) const { }

	template<class K, class A>
	void operator()(K , const zero&, const A&) const { }
    
      };

       
    



      




      struct invert {
      
	template<class A>
	A operator()(const A& a) const {
	  assert(a.rows() == a.cols());
	  return math::svd(a).inverse();
	  return a.inverse();
	}
      
      };

    
      struct minus {
      
	template<class A>
	A operator()(const A& a) const { return -a; }
      
      };
    
    }

  }

}

#endif
