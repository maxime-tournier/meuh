#include "friction.h"

#include <core/stl.h>

#include <coll/hit.h>
#include <coll/primitive.h>

#include <math/covector.h>

#include <math/random.h>

#include <Eigen/Geometry>

#include <math/mat.h>
#include <math/vec.h>
#include <math/svd.h>
#include <math/pi.h>

#include <sparse/dense.h>
#include <sparse/prod.h>

namespace tool {

  using namespace math;

  
  mat33 friction::basis(const vec3& normal ) {
    
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
  
  
  mat22 friction::cone(real mu) {
    
    mat22 res;

    res << 
      1, -1,
      mu, mu;

    return res;
  }
    

  math::mat friction::discrete(natural n) {

    math::mat res;
    res.resize(3, n);
      
    for(natural i = 0; i < n; ++i) {
      real theta = (i * pi) / n;
	
      res.col(i) << 0, std::cos(theta), std::sin(theta);
    }

    return res;
  }
  

  void friction::update(const std::list< coll::hit >& hits) {
    using namespace math;
    
    const natural size = hits.size();
    
    normal.resize( size, normal_type( 1 ) );
    tangent.resize( size, tangent_type( 2 ) );
    
    natural i = 0;

    core::each( hits, [&](const coll::hit& h) {
	
    	// dofs
    	const auto d1 = phys::dof::key( h.first->data );
    	const auto d2 = phys::dof::key( h.second->data );
	
	assert( d1 || d2 );
	
    	// constraint handles
    	const auto cn = &normal[i];
    	const auto ct = &tangent[i];
	
	using namespace math;
	
	// world geometry
	auto b = basis(h.normal);
	
    	// get pullbacks and fill geometry rows
    	if( d1 ) {
	  pullback_type dT1 = mapper.find(d1)->second( h );
	 
	  real sign = h.sign( h.first );
	  
	  // normal
	  dT1( N( cn, d1 ).row(0), sign * b.col(0) );
	  
	  // tangent
	  for(natural j = 0; j < 2; ++j) {
	    dT1( T( ct, d1 ).row(j), sign * b.rightCols<2>().col(j) );
	  };
	  
    	}
	
    	if( d2 ) {
	  pullback_type dT2 = mapper.find(d2)->second( h );
	  using namespace math;
	  
	  real sign = h.sign( h.second );
	  
	  // normal
	  dT2( N( cn, d2 ).row(0),  sign * b.col(0) );
	  
	  // tangent
	  for(natural j = 0; j < 2; ++j) {
	    dT2( T( ct, d2 ).row(j), sign * b.rightCols<2>().col(j) );
	  };
    	}
	
    	n(cn).setConstant( h.error );
	
    	++i;
      });
    
  }


  void friction::clear() {
    (*this) = friction();
  }


  static math::mat make_K(math::natural n, math::real mu ) {
    math::mat res;
    res.resize(2 * n, 3);

    math::mat d = friction::discrete(n);

    for(math::natural i = 0; i < n; ++i) {
      res.row(i) = d.col(i).transpose();
      res.row(n + i) = -d.col(i).transpose();
    }

    res.col(0).setConstant(mu);

    return res;
  }
  

  friction::helper::helper(const friction& data,
			   math::natural n,
			   math::real mu)
    : data(data),
      n(n),					
      mu(mu),
      K( make_K(n, mu) ),
      D( discrete(n) ) {
    // TODO optimize a bit (D, then K)
  }

  
  math::natural friction::helper::vars() const {
    return n * data.normal.size();
  }

  math::natural friction::helper::cons() const {
    return ( K.rows() +  2 * D.rows() ) * data.normal.size();
  }
  
  
  void friction::helper::apply(math::vec& out, const math::vec& in) const {
    assert( out.size() == offset.cons + cons() );
    assert( in.size() == offset.vars + vars() );
    
    phys::dof::vector v = sparse::dense(dofs, in);
    
    // compute relative velocities
    normal_type::vector Nv = data.N * v;
    tangent_type::vector Tv = data.T * v;
    
    // constraint segment
    auto chunk = out.segment(offset.cons, cons());
    
    math::natural off = 0;
    
    for(math::natural i = 0; i < data.normal.size(); ++i) {
      math::vec3 u;
      
      u.row(0) = Nv( &data.normal[i] );
      u.tail<2>() = Tv( &data.tangent[i] );
      
      math::vec s = out.segment(offset.cons + i * n, n);
      
      math::vec ss; ss.resize( 2 * n );
      ss << s, s;
      
      chunk.segment(off, K.rows()) += K * u + (mu * mu) * ss;
      
      math::vec Du = D * u;

      off += K.rows();
      chunk.segment(off, D.rows()) += Du + s;

      off += D.rows();
      chunk.segment(off, D.rows()) += -Du + s;
      
    };
  };



  void friction::helper::applyT(math::vec& out, const math::vec& in) const {
    assert( in.size() == offset.cons + cons() );
    assert( out.size() == offset.vars + vars() );
    
    // constraint segment
    auto chunk = out.segment(offset.vars, vars());
    
    // math::natural off = 0;
    
    // for(math::natural i = 0; i < data.normal.size(); ++i) {
    //   math::vec3 u;
      
    //   u.row(0) = Nv( &data.normal[i] );
    //   u.tail<2>() = Tv( &data.tangent[i] );
      
    //   math::vec s = out.segment(offset.cons + i * n, n);
      
    //   math::vec ss; ss.resize( 2 * n );
    //   ss << s, s;
      
    //   chunk.segment(off, K.rows()) = K * u + (mu * mu) * ss;
      
    //   math::vec Du = D * u;

    //   off += K.rows();
    //   chunk.segment(off, D.rows()) = Du + s;

    //   off += D.rows();
    //   chunk.segment(off, D.rows()) = -Du + s;
      
    // };

  };








}
