#include "pose.h"

#include <math/func/lie.h>
#include <math/func/coord.h>
#include <math/func/id.h>
#include <math/func/tuple.h>
#include <math/tuple.h>

#include <math/func/vector.h>


namespace mocap {

  local::range local::operator()(const domain& p) const {
    const math::natural n = p.rows();
    range res;
    res.resize(n);

    using core::tree::prefix;
    skeleton->exec( prefix( [&](mocap::skeleton* j) {
	joint::index i = j->get().id;
	assert( i < n );
	
	if( j->root() ) res( i ) = p( i );
	else {
	  res( i ) = p( j->parent()->get().id ).inverse() * p(i);
	}
	})
      );
    
    return res;
  }

  using namespace math;

  // T<local::range> local::diff(const T<domain>& dp) const {
  //   const natural n = dp.at().rows();

  //   range at;
  //   Lie<range>::algebra body;
    
  //   at.resize( n );
  //   body.resize( n );
    
  //   const Lie<domain> dmn(dp.at());

  //   using core::tree::prefix;
  //   skeleton->each([&](mocap::skeleton* j) {
  // 	const joint::index i = j->get().id;
  // 	assert( i < n );

  // 	if( j->root() ) {
  // 	  at( i ) = dp.at()( i );
  // 	  body(i) = dp.body()( i );
  // 	}
  // 	else {
  // 	  const joint::index p = j->parent()->get().id;
	  
  // 	  using namespace func;
  // 	  auto f =  
  // 	    func::lie::prod( func::lie::inverse<SE3>(), id<SE3>() ) 
  // 	    << join( coord<SE3>(p), coord<SE3>(i) );
	  
  // 	  auto dres_i = d(f)( dp );
	  
  // 	  at( i ) = dres_i.at();
  // 	  body( i ) = dres_i.body();
	  
  // 	}
  //     });
    
  //   return math::body(at, body);
  // }
  
  
  global::range global::operator()(const domain& p) const {
    const natural n = p.rows();
     
    range res;
    res.resize(n);
    

    using core::tree::prefix;
    
    skeleton->exec( prefix( [&](mocap::skeleton* j) {
	  joint::index i = j->get().id;
	  assert( i < n );
	  
	  
	  if( j->root() ) res( i ) = p( i );
	  else {
	    joint::index parent = j->parent()->get().id;

	    res( i ) = res( parent  ) * p(i);
	    // TODO normalize ?
	  }

	}));
    
    return res;
  }
    
  // math::T<global::range> global::diff(const math::T<domain>& dp) const {
  //   using namespace math;
  //   const natural n = dp.at().rows();

  //   range at;
  //   Lie<range>::algebra body;

  //   at.resize(n);
  //   body.resize(n);

  //   const Lie<domain> dmn(dp.at());
    
  //   skeleton->exec( core::tree::prefix( [&]( mocap::skeleton* j ) {
  // 	  const joint::index i = j->get().id;
  // 	  assert( i < n );
	  
  // 	  if( j->root() ) { 
  // 	    at( i ) = dp.at()( i );
  // 	    body( i ) = dp.body()( i );
  // 	  } else {
  // 	    const joint::index p = j->parent()->get().id;
	    
  // 	    auto dres_p = math::body(at(p), body(p));
  // 	    auto dp_i = func::coord<SE3>(i).diff(dp);
	    
  // 	    auto dres_i = func::lie::product<SE3>().diff( math::tuple::merge( std::make_tuple(dres_p, dp_i) ) );
	    
  // 	    at(i) = dres_i.at();
  // 	    body(i) = dres_i.body();
  // 	  }
	  
  // 	}));
    
  //   return math::body(at, body);
    
  // }


  
  pose tee_pose(skeleton* s) {
    pose res;
    res.resize( core::tree::size(s) );
    
    s->each( [&res](skeleton* j) {
	res( j->get().id ) = math::SE3::translation( j->get().offset );
      });
    
    return res;
  };



}
