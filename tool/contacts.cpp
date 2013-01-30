#include "contacts.h"

#include <core/stl.h>
#include <core/debug.h>

#include <coll/hit.h>
#include <coll/primitive.h>

#include <math/covector.h>

#include <math/svd.h>
#include <math/func/SE3.h>

namespace tool {

  // basis is n, tx, ty
  static math::mat make_cone(const math::mat33& basis, math::natural n) {
    math::mat res;
    res.resize(3, n);


    math::vec3 coords;
    res.each_col([&](math::natural j) {
	
	math::real alpha = (2 * j * math::pi) / n;

	// TODO add coulomb friction coeff
	coords << 1, std::cos(alpha), std::sin(alpha);

	res.col(j) = (basis * coords).normalized();
	
      });

    return res;
  }
  

  static math::mat33 make_basis(const math::vec3& normal) {
    using namespace math;
    mat33 orig = mat33::Zero();
    orig.col(0) = normal;

    mat33 res = svd(orig).u();
    
    // ensure we have a +1 determinant

    vec3 tmp;
    if( res.determinant() < 0 ) {
      // swap tangent vectors
      tmp = res.col(1);
      res.col(1) = res.col(2);
      res.col(2) = tmp;
    }

    return res;
  }




  void contacts::update(const std::list< coll::hit >& hits ) {
    
    normal.resize( hits.size(), normal_type(1) );
    tangent.resize( hits.size(), tangent_type(2) );
    
    const math::natural dim = N.geometry.keys().dim();
    
    N.geometry = normal_type::matrix();
    N.geometry.reserve( dim );
    
    N.corrections = normal_type::vector();
    N.corrections.reserve( dim );

    T.geometry = tangent_type::matrix();
    T.geometry.reserve( dim ); 
    
    T.corrections = tangent_type::vector();
    T.corrections.reserve( dim );
    
    points.resize(hits.size());
    basis.resize(hits.size());
    
    math::natural i = 0;
    core::each( hits, [&](const coll::hit& h) {
	
	// dofs
	const auto d1 = phys::dof::key( h.first->data );
	const auto d2 = phys::dof::key( h.second->data );
	
	if(!d1 && !d2) { macro_debug("no dofs in contact lol"); }
	
	// constraint
	const auto cn = &normal[i];
	const auto ct = &tangent[i];

	using namespace math;
	
	// world geometry
	basis[i] = make_basis(h.normal);
	points[i] = h.pos;
	
	// get pullbacks and fill geometry rows
	if( d1 ) {
	  pullback_type dT1 = mapper.find(d1)->second( h );
	 
	  real sign = h.sign( h.first );
	  
	  // normal
	  dT1( N.geometry( cn, d1 ).row(0), sign * basis[i].col(0) );
	  
	  // tangent
	  for(natural j = 0; j < 2; ++j) {
	    dT1( T.geometry( ct, d1 ).row(j), sign * basis[i].rightCols<2>().col(j) );
	  };
	  
	}

	if( d2 ) {
	  pullback_type dT2 = mapper.find(d2)->second( h );
	  using namespace math;
	   
	  real sign = h.sign( h.second );
	  
	  // normal
	  dT2( N.geometry( cn, d2 ).row(0),  sign * basis[i].col(0) );

	  // tangent
	  for(natural j = 0; j < 2; ++j) {
	    dT2( T.geometry( ct, d2 ).row(j), sign * basis[i].rightCols<2>().col(j) );
	  };
	  
	}

	N.corrections(cn).setConstant( h.error );
	
	++i;
      });
  }



  // delicious copypasta
  void contacts::update_cones(const std::list< coll::hit >& hits, math::natural faces ) {
    
    cone.resize( hits.size(), cone_type(faces) );
    
    // previous size
    const math::natural dim = K.geometry.keys().dim();
    
    K.geometry = cone_type::matrix();
    K.geometry.reserve( dim );
    
    points.resize(hits.size());
    basis.resize(hits.size());
    
    math::natural i = 0;
    core::each( hits, [&](const coll::hit& h) {
	
	// dofs
	const auto d1 = phys::dof::key( h.first->data );
	const auto d2 = phys::dof::key( h.second->data );
	
	if(!d1 && !d2) { macro_debug("no dofs in contact lol"); }
	
	// constraint
	const auto c = &cone[i];
	
	using namespace math;
	
	// world geometry
	basis[i] = make_cone( make_basis(h.normal), faces );
	points[i] = h.pos;
	
	// get pullbacks and fill geometry rows
	if( d1 ) {
	  const pullback_type& dT1 = mapper.find(d1)->second( h );
	 
	  real sign = h.sign( h.first );
	  
	  for(natural j = 0; j < faces; ++j) {
	    dT1( K.geometry( c, d1 ).row(j), sign * basis[i].col(j) );
	  }
	  
	}

	if( d2 ) {
	  const pullback_type& dT2 = mapper.find(d2)->second( h );
	  using namespace math;
	   
	  real sign = h.sign( h.second );
	  
	  for(natural j = 0; j < faces; ++j) {
	    dT2( K.geometry( c, d2 ).row(j), sign * basis[i].col(j) );
	  }

	}

	// K.corrections(c).setConstant( -1e-7 );
	
	++i;
      });
   }










  std::vector<math::vec3> contacts::forces(const tangent_type::vector& lambda,
					   const normal_type::vector& mu) const {
    std::vector<math::vec3> res(normal.size());

    math::vec x;
    
    for(math::natural i = 0; i < normal.size(); ++i) {
      auto n = mu( &normal[i] );
      auto t = lambda( &tangent[i] );
      
      x.resize(n.size() + t.size() );

      if( n.size() ) x.head( n.size() ) = n;
      if( t.size() ) x.tail( t.size() ) = t;
      
      res[i] = basis[i] * x;
    }
    
    return res;
  };


  std::vector<math::vec3> contacts::forces_cone(const cone_type::vector& lambda) const {
    std::vector<math::vec3> res(cone.size());

    for(math::natural i = 0; i < cone.size(); ++i) {
       
      res[i] = basis[i] * lambda(&cone[i]);
     
    }
    
    return res;
  };

  contacts::zmp_type contacts::zmp(const std::vector<math::vec3>& f) const {
    using namespace math;
    wrench total = wrench::Zero();
    SE3 at = SE3::identity();

    assert( f.size() == points.size() );
    
    for(natural i = 0; i < f.size(); ++i) {
      total += dT(func::rigid_map<>{points[i]})(at)(f[i].transpose());
    }
    
    // core::log("zmp sanity check:", angular(total).dot(linear(total)) );


    zmp_type res; 
    res.base = linear(total).transpose().cross(angular(total).transpose()) / linear(total).squaredNorm();
    res.dir = linear(total).transpose();
    
    return res;
  };
  

  void contacts::clear() {
    (*this) = contacts();
  }


 

}
