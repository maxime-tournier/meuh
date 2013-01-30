#ifndef SPARSE_LINEAR_H
#define SPARSE_LINEAR_H

#include <map>
#include <math/vec.h>
#include <math/matrix.h>

#include <core/stl.h>

#include <sparse/map.h>
#include <sparse/zip.h>

#include <sparse/linear/dimension.h>
#include <sparse/linear/func.h>

#include <Eigen/LU>
#include <math/nan.h>

#include <sparse/linear/consistent.h>
#include <core/stl.h>

#include <core/log.h>
#include <sparse/iter.h>
#include <sparse/foldl.h>

#include <sparse/linear/lazy.h>

// TODO cleanup, 
namespace sparse {
  namespace linear {
    
  

    // matrix vector product
    template<class K1, class K2>
    std::map<K1, math::vec> operator*(const std::map<K1, std::map<K2, math::mat> >& m,
				      const std::map<K2, math::vec>& v) {
      assert( consistent(m) );
      assert( consistent(v) );
      using namespace math;

      typedef typename std::map<K1, std::map<K2, mat> >::const_iterator row_it;
      typedef typename std::map<K2, mat>::const_iterator col_it;
      typedef typename std::map<K2, vec>::const_iterator vec_it;

      std::map<K1, vec> res;
      auto it = res.begin();
      
      vec chunk;
      for(row_it row = m.begin(), erow = m.end(); row != erow; ++row) {
	
	chunk = vec::Zero(dim(row->first) );
	
	// on calcule le bloc
	// sparse::iter( core::range::all(row->second), core::range::all(v),
	// 	      func::product_iter(chunk) );

	sparse::iter_nonzero( row->second, v, func::product_iter<math::vec>(chunk) );
      
	
	// puis on l'insere
	it = ++sparse::insert(res, row->first, chunk, it );
      }
    
      return res;
    }

    // sum
    template<class Key, class Content>
    std::map<Key, Content> operator+(const std::map<Key, Content>& x,
    				     const std::map<Key, Content>& y) {
      assert( consistent(x) && consistent(y) );
      
      std::map<Key, Content> res = x;
      res += y;
      
      return res;
    }

    template<class Key, class Content>
    std::map<Key, Content>& operator+=(std::map<Key, Content>& lhs, const std::map<Key, Content>& rhs) {
      assert( consistent(lhs) && consistent(rhs) );
      sparse::iter(core::range::all(lhs), core::range::all(rhs), func::plus_equal<Key>(lhs) );
      return lhs;
    }

    template<class Key, class Content>
    std::map<Key, Content>& operator-=(std::map<Key, Content>& lhs, const std::map<Key, Content>& rhs) {
      assert( consistent(lhs) && consistent(rhs) );
      sparse::iter(core::range::all(lhs), core::range::all(rhs), func::less_equal<Key>(lhs) );
      return lhs;
    }


    template<class Key, class Content>
    std::map<Key, Content>&& operator+(const std::map<Key, Content>& x,
				       std::map<Key, Content>&& y) {
      y += x;
      return std::move(y);
    }

    template<class Key, class Content>
    std::map<Key, Content>&& operator+(std::map<Key, Content>&& x,
				       const std::map<Key, Content>& y) {
      x += y;
      return std::move(x);
    }

    template<class Key, class Content>
    std::map<Key, Content>&& operator+(std::map<Key, Content>&& x,
				       std::map<Key, Content>&& y) {
      x += y;
      return std::move(x);
    }



    // unary minus
    template<class Key, class Content>
    std::map<Key, Content> operator-(const std::map<Key, Content>& x) {
      assert( consistent(x) );
      return sparse::map<Content>(x, func::minus() );
    }
  
    // binary minus
    template<class Key, class Content>
    std::map<Key, Content> operator-(const std::map<Key, Content>& x,
				     const std::map<Key, Content>& y) {
      assert( consistent(x) && consistent(y) );
      std::map<Key, Content> res;
      sparse::zip(res, x, y, func::diff() );
      return res;
    }

    // binary minus
    template<class Key, class Content>
    std::map<Key, Content>&& operator-(std::map<Key, Content>&& x,
				       const std::map<Key, Content>& y) {
      assert( consistent(x) && consistent(y) );
      x -= y;
      return std::move(x);
    }

    


    // scalar multiplication
    template<class Key, class Content>
    std::map<Key, Content> operator*(math::real lambda, const std::map<Key, Content>& x) {
      return sparse::map<Content>(x, func::scalar{lambda} );
    }


    // vector dot product
    template<class Key>
    math::real fast_dot(const std::map<Key, math::vec >& x,
		   const std::map<Key, math::vec >& y) {
      
      math::real res = 0;

      sparse::iter_nonzero(x, y, func::dot{res} );
       
      return res;
    }

    // vector dot product
    template<class Key>
    math::real dot(const std::map<Key, math::vec >& x,
		   const std::map<Key, math::vec >& y) {
      
      math::real res = 0;

      sparse::iter(core::range::all(x), core::range::all(y), func::dot{res} );
      
      return res;
    }
    

    // squared norm
    template<class Key>
    math::real norm2(const std::map<Key, math::vec >& x) {
      return sparse::foldl(x, 0.0, [](const math::real& sum, const math::vec& v) { return sum + v.squaredNorm(); } );
    }

    // norm
    template<class Key>
    math::real norm(const std::map<Key, math::vec >& x) {
      return std::sqrt( norm2(x));
    }
    

    

  }
}

#endif
