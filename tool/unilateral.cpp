#include "unilateral.h"

#include <core/stl.h>
#include <core/debug.h>

#include <coll/hit.h>
#include <coll/primitive.h>

#include <math/covector.h>



namespace tool {

  void unilateral::update(const std::list< coll::hit >& hits ) {
    
    constraints.resize( hits.size(), constraint_type(1) );
    
    const math::natural dim = geometry.keys().dim();
    
    geometry = constraint_type::matrix();
    geometry.reserve( dim );
    
    corrections = constraint_type::vector();
    corrections.reserve( dim );
    
    math::natural i = 0;
    core::each( hits, [&](const coll::hit& h) {
	
	// dofs
	const auto d1 = phys::dof::key( h.first->data );
	const auto d2 = phys::dof::key( h.second->data );
	
	if(!d1 && !d2) { macro_debug("no dofs in contact lol"); }
	
	// constraint
	const auto c = &constraints[i];

	// get pullbacks and fill geometry rows
	if( d1 ) {
	  const pullback_type& dT1 = mapper.find(d1)->second( h.pos, h.first );
	  dT1( geometry( c, d1 ).row(0), h.dir( h.first ) );
	}

	if( d2 ) {
	  const pullback_type& dT2 = mapper.find(d2)->second( h.pos, h.second );
	  dT2( geometry( c, d2 ).row(0), h.dir( h.second ) );
	}

	corrections(c).setConstant( h.error );
	
	++i;
      });
    
  }


  void unilateral::clear() {
    (*this) = unilateral();
  }



}
