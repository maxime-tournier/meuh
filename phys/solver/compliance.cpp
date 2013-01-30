// #include "compliance.h"

// #include <sparse/linear/diagonal.h>
// #include <sparse/linear/nan.h>
// #include <sparse/linear.h>
// #include <core/stl.h>

// #include <core/macro/here.h>
// #include <core/macro/debug.h>


// namespace phys {
//   namespace solver {

//     // TODO optimize symmetric terms ?
//     // compliance::matrix_type make_matrix(const compliance& comp) {
//     //   compliance::matrix_type res;
      
//     //   if(comp.p) {
//     // 	res(compliance::lambda)(compliance::lambda) = comp.J.compliance();
	
//     // 	if(comp.q) res(compliance::lambda)(compliance::normal) = comp.J.dense * comp.N.linear;
//     // 	if(comp.r) res(compliance::lambda)(compliance::tangent) = comp.J.dense * comp.T.linear;
//     // 	// if(comp.a) res(compliance::lambda)(compliance::act) = comp.J.dense * comp.A.linear;
//     //   }

//     //   if(comp.q) {
//     // 	const math::vec diag = comp.N.keys.dense(comp.system.constraint.unilateral.damping);
	
//     // 	res(compliance::normal)(compliance::normal) = comp.N.compliance();
//     // 	res(compliance::normal)(compliance::normal).diagonal() += diag;
	
//     // 	if(comp.p) res(compliance::normal)(compliance::lambda) = comp.N.dense * comp.J.linear;
//     // 	if(comp.r) res(compliance::normal)(compliance::tangent) = comp.N.dense * comp.T.linear;
//     // 	// if(comp.a) res(compliance::normal)(compliance::act) = comp.N.dense * comp.A.linear;
//     //   }

//     //   if(comp.r) {
//     // 	const math::vec diag = comp.T.keys.dense(comp.system.constraint.friction.damping);

//     // 	res(compliance::tangent)(compliance::tangent) = comp.T.compliance();
//     // 	res(compliance::tangent)(compliance::tangent).diagonal() += diag;
	
//     // 	if(comp.p) res(compliance::tangent)(compliance::lambda) = comp.T.dense * comp.J.linear;
//     // 	if(comp.q) res(compliance::tangent)(compliance::normal) = comp.T.dense * comp.N.linear;
//     // 	// if(comp.a) res(compliance::tangent)(compliance::act) = comp.T.dense * comp.A.linear;
//     //   }


//     //   return res;
//     // }


//     struct diag_sum {

//       const math::real lambda;

//       void operator()(const phys::dof* , math::mat& m, const math::vec& diag) const {
// 	assert(m.rows() == m.cols());
// 	assert(m.rows() == diag.rows());
	
// 	m.diagonal() += lambda * diag;
//       }

//       void operator()(const phys::dof*, sparse::zero, const math::vec&) const {  }
//       void operator()(const phys::dof*, math::mat&, sparse::zero) const {  }
      
//     };



//     static dof::mass make_integration(const phys::system& system, math::real dt ) {
//       dof::mass res = system.mass;
//       sparse::iter( core::range::all(res), core::range::all(system.damping), diag_sum{dt} );
//       sparse::iter( core::range::all(res), core::range::all(system.stiffness), diag_sum{dt * dt} );
//       return res;
//     }


//     compliance::compliance(const phys::system& system, math::real dt) 
//       :  system(system),
// 	 mass( make_integration(system, dt )  ),
// 	 // mass_inv( sparse::linear::diag(mass).inverse() ),
// 	 // response(mass, mass_inv),
// 	 dofs( sparse::keys( mass ) ),
	 
// 	 // dense matrix + responses
// 	 J( system.constraint.bilateral.matrix ),
// 	 N( system.constraint.unilateral.matrix ),
// 	 T( system.constraint.friction.matrix ),
// 	 // A( system.actuator.matrix, response ),
	
// 	 // dimensions
// 	 n( dofs.dim() ),
// 	 p( J.keys.dim() ),
// 	 q( N.keys.dim() ),
// 	 r( T.keys.dim() )
// 	 // a( A.keys.dim() ),
// 	 // matrix( make_matrix(*this) )
//     {
//       diagonal.resize(p + q + r);
      
//       math::real epsilon = 1;

//       // diagonal.each([&](math::natural i) {
//       // 	  diagonal(i) = ( 1 + epsilon) * dense(i).dot(linear(i));
//       // 	});
      
//       damping.setZero(p + q + r);

//       // TODO bilateral ?

//       // FIXME BROKEN !
//       // if(p) damping.segment(0, p) = J.keys.dense(system.constraint.bilateral.damping);
//       // if(q) damping.segment(p, q) = N.keys.dense(system.constraint.unilateral.damping);
//       // if(r) damping.segment(p + q, r) = T.keys.dense(system.constraint.friction.damping);

      
//      }


//     math::natural compliance::dim() const { return p + q + r; }
    

//     math::vec compliance::rhs(const dof::velocity& v) const {
//       math::vec res; res.resize(dim());
      
//       if( p ) res.segment(0, p) = J.rhs( system.constraint.bilateral.values ) - J.dense * J.dofs.dense(v);
//       if( q ) res.segment(p, q) = N.rhs( system.constraint.unilateral.values ) - N.dense * N.dofs.dense(v);
//       if( r ) res.segment(p + q, r) = - T.dense * T.dofs.dense(v);
      
//       return res;
//     }
    

//     dof::velocity compliance::correction(const math::vec& x) const {
//       dof::velocity res;

//       // using sparse::linear::operator+;
//       // if( p ) res = res + J.dofs.sparse( J.linear * x.segment(0, p));
//       // if( q ) res = res + N.dofs.sparse( N.linear * x.segment(p, q));
//       // if( r ) res = res + T.dofs.sparse( T.linear * x.segment(p + q, r));

//       return res;
//     }

//     dof::momentum compliance::impulse(const math::vec& x) const {
//       dof::momentum res;

//       using sparse::linear::operator+;
//       if( p ) res = res + J.dofs.sparse( J.dense.transpose() * x.segment(0, p));
//       if( q ) res = res + N.dofs.sparse( N.dense.transpose() * x.segment(p, q));
//       if( r ) res = res + T.dofs.sparse( T.dense.transpose() * x.segment(p + q, r));

//       return res;
//     }

  
//     // gives the block and index offset for a constraint index in full
//     // system
//     struct matrix_index {
//       math::natural block, offset, local;
    
//       matrix_index(const compliance& C, math::natural i) 
// 	: block(0),
// 	  offset(0)
//       {
// 	assert( i < C.dim());
	
// 	if( i < C.p ) {
// 	  block = compliance::lambda;
// 	  offset = 0;
	
// 	} else if (i < (C.p + C.q) ) {
// 	  block = compliance::normal;
// 	  offset = C.p;
	  
// 	} else  {
// 	  block = compliance::tangent;
// 	  offset = C.p + C.q;
// 	}

// 	local = i - offset;
//       }
      
//     };
    
//     auto compliance::dense(math::natural i) const  -> decltype(compliance::halp().row(0) ) {
      
//       const matrix_index c(*this, i);
	
//       switch(c.block) {
//       case compliance::lambda: return J.dense.row(c.local);
//       case compliance::normal: return N.dense.row(c.local);
//       case compliance::tangent: return T.dense.row(c.local);
//       }
//       throw std::logic_error("bad constraint index");
//     }

      

//     math::real compliance::row_dot(math::natural i,  const math::vec& x) const {
//       assert( x.rows() == dim());
      
//       using namespace math;
//       real res = 0;
      
//       const matrix_index c(*this, i);
      
//       // throw std::logic_error("broken");
//       if( p ) res += matrix(compliance::lambda)(c.block).col(c.local).dot(x.segment(0, p));
//       if( q ) res += matrix(compliance::normal)(c.block).col(c.local).dot(x.segment(p, q));
//       if( r ) res += matrix(compliance::tangent)(c.block).col(c.local).dot(x.segment(p + q, r));
      
//       return res;
//     }

//     auto compliance::linear(math::natural i) const -> decltype(compliance::halp().col(0) ){
//       const matrix_index c(*this, i);

//       switch(c.block) {
//       // case compliance::lambda: return J.linear.col(c.local);
//       // case compliance::normal: return N.linear.col(c.local);
//       // case compliance::tangent: return T.linear.col(c.local);
//       }
//       throw std::logic_error("bad constraint index");
//     }
    
//     // math::real compliance::diag(math::natural i) const { 
//     //   const matrix_index c(*this, i);
      
//     //   // TODO cache
//     //   return dense(i).dot(linear(i));

//     //   const math::real res = matrix(c.block)(c.block)(c.local, c.local);
//     //   assert( res > 1e-8 );

//     //   return res;
//     // }

//   }
// }
