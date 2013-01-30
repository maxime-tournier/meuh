#ifndef SPARSE_LINEAR_CONSISTENT_H
#define SPARSE_LINEAR_CONSISTENT_H

#include <sparse/linear/dimension.h>
#include <math/vec.h>
#include <map>

#include <core/stl.h>

namespace sparse {
  namespace linear {

    
    template<class K>
    bool consistent(const std::map<K*, math::vec>& v ) {
      using namespace core;
      
      for( auto chunk = core::range::all(v); !chunk.empty(); chunk.pop() ) {
	if( ! dim(chunk.front().first) ) return false;
	if( int( dim(chunk.front().first) ) != chunk.front().second.rows() ) return false;
      }   
      
      return true;
    }

    template<class K>
    bool consistent(const std::map<K*, math::mat>& v ) {
      using namespace core;
      
      for( auto chunk = core::range::all(v); !chunk.empty(); chunk.pop() ) {
	if( ! dim(chunk.front().first) ) return false;
	if( chunk.front().second.rows() != chunk.front().second.cols()) return false;
	if( int( dim(chunk.front().first) ) != chunk.front().second.rows() ) return false;
      }   
      
      return true;
    }
    
    
    template<class K1, class K2>
    bool consistent(const std::map<K1*, std::map<K2*, math::mat> >& m ) {
      // TODO foreach
      
      for( auto rows = core::range::all(m); !rows.empty(); rows.pop() ) {
	K1* row = rows.front().first;
	
	if( !dim(row) ) return false;
	
	for( auto cols = core::range::all( rows.front().second); !cols.empty(); cols.pop() ) {
	  const math::mat& chunk = cols.front().second;
	  K2* col = cols.front().first;
	
	  if( !dim(col) ) return false;
	  if( chunk.rows() != int( dim(row)) ) return false;
	  if( chunk.cols() != int( dim(col)) ) return false;
	}
	
      }
      
      return true;
    }
    

  }
}

#endif
