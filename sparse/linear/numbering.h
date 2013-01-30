#ifndef SPARSE_LINEAR_NUMBERING_H
#define SPARSE_LINEAR_NUMBERING_H

#include <math/types.h>
#include <map>

#include <boost/iterator/transform_iterator.hpp>
#include <core/stl.h>
#include <sparse/keys.h>

#include <sparse/linear/dimension.h>
#include <core/strip.h>

namespace sparse{ 
  namespace linear {
    
    // les offset sont tries en fonction des key*
    template<class Key>
    class numbering { 
      typedef typename Key::key key_type;
      typedef std::map<key_type, math::natural> offsets_type;
      offsets_type offsets_;
    
      math::natural dim_;
      
    public:
      
      template<class Range>
      void number(Range keys ){
	clear();
	
	// first insert all keys
	core::each(keys, [&](key_type k) {
	    this->offsets_.insert( this->offsets_.end(), std::make_pair(k, 0) );
	  });
	
	// then assign offsets
	core::each(offsets_, [&](key_type k, math::natural& o) {
	    o = this->dim();
	    const math::natural d =  linear::dim(k);
	    assert(d && "zero dimension key !");

	    this->dim_ += d;
	  });
	
      };
      
      void clear() { dim_ = 0; offsets_.clear(); }
      numbering() : dim_(0) { };
      
      template<class Range>
      numbering( Range keys ) { number( keys ); }
      
      math::natural dim() const { return dim_; }
      math::natural size() const { return offsets_.size(); }

      typedef typename offsets_type::const_iterator const_offsets_iterator;
      typedef core::range::iterator< const_offsets_iterator > offsets_range;
      
      offsets_range offsets() const { return {offsets_.begin(), offsets_.end() }; }
      
      core::range::first< offsets_range > keys() const { 
	return core::range::take_first(offsets());
      }

      typedef std::map<key_type, math::vec> sparse_vector;
      typedef math::vec dense_vector;
      

      // impl is in dense.h
      sparse_vector sparse(const dense_vector& v) const;
      dense_vector dense(const sparse_vector& s) const;

      sparse_vector zero() const;

      
      math::natural off(key_type k) const { 
	auto r = offsets_.find(k); 
	assert(r != offsets_.end() );
	
	return r->second;
      }

      
      // convenience
      template<class F>
      void each(const F& f) const {
	core::each(offsets_, f);
      }

    };

    namespace impl {

      template<class > struct help {};
      template<class A> struct help<const A*> { typedef A type; };
    }


    template<class Range>
    numbering< typename impl::help<typename core::strip<typename Range::value_type>::type>::type>  number(const Range& range) { return {range}; }
    
  }
}

#endif

// TODO FIXME
#include <sparse/linear/dense.h>

