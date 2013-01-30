#include "control.h"

#include <math/func/euclid.h>
#include <phys/solver/affine.h>

namespace gui {
  namespace viewer {



    void control::track(::control::system& ctrl, const phys::dof* dof,
			const math::mat& F, const math::vec& c, math::real w) {
      if( F.rows() != c.rows() ) throw std::logic_error("bad feature rows");
      if( F.cols() != dof->dim ) throw std::logic_error("bad feature cols");
      if( math::nan(c) ) std::abort();
      
      if( w == 0 ) return;
      
      auto f = new ::control::feature(F.rows());


      ctrl.feature.storage.push_back( core::unique(f) );
      ctrl.feature.matrix[f][dof] = w * F;
      ctrl.feature.values[f] = w * c;
    }


    void control::reference_objective(::control::system& ctrl) {
      using namespace math;

      if(!contacts.center || !reference ) return;

      mat J = mat::Zero(obj->pga.n, obj->dim);

      // J.rightCols(obj->pga.n).setIdentity();
      vec weights = (vec::Ones(obj->pga.n) + obj->pga.data.eigen().head( obj->pga.n )).cwiseInverse();
      weights.setOnes();
      
      J.rightCols(obj->pga.n).diagonal() = weights;
      
      assert(J.rows() == obj->rest.rows());

      const ::control::pd<vec> pd(tracker.p(dt), tracker.d(), Lie<vec>(obj->pga.n) );
      func::tuple_get<2, object_type::conf_type > get_inner;

      track(ctrl, obj.get(), J, 
	    weights.cwiseProduct( pd(d(get_inner)(obj->state()), obj->rest) ),
	    reference);
      
    }



    void control::balance_objective(::control::system& ctrl ) {
      if(!contacts.center || !gamma ) return;
      
      using namespace math;
      const natural head = tracking->skeleton().find("head");

     
      using func::operator<<;
      const auto abs = tool::character::absolute(*obj);

      const real w_com = 20 * gamma;
      {
	vector< vec3, 2 > plane; 
	plane(0) = vec3::UnitX();
	plane(1) = vec3::UnitZ();
	const auto proj = func::euclid::project<vec3>(plane);
	
	const auto hd = proj << func::absolute<>() << func::coord(head, pose) << abs;
	const auto com = proj << phys::rigid::com(tracking->skeleton()) << abs;
	
	const ::control::pd<vec2> pd(tracker.p(dt), tracker.d());
	auto center = *contacts.center;
	auto support = proj(center);

	const real w_hd = 5 * gamma;
	track(ctrl, obj.get(), 
	      func::J(hd, obj->lie)(obj->conf),  
	      pd( hd.diff(obj->state()), support ),
	      w_hd);
      
	track(ctrl, obj.get(), 
	      func::J(com, obj->lie)(obj->conf),  
	      pd( com.diff(obj->state()), support ),
	      w_com);
      }
      
      // angular momentum
      
      const mat H = tool::character::angular(*obj, obj->conf) / obj->skeleton.mass;
      
      const vec3 h = H * obj->lie.alg().coords( obj->velocity );
      const vec3 hdot = (h - obj->AM) / dt;
      
      const T<vec3> dh = math::body(h, hdot);
      const vec3 resp = ::control::pd<vec3>(tracker.p(dt), tracker.d())(dh, vec3::Zero());
      
      // objective.add( new generic(*obj, H, value, gamma, "angular momentum") );
      vec3 value = h + dt * resp;
      // value.setZero();

      const real w_am = 20 * weight.am;
            
      track(ctrl, obj.get(), H, value, w_am);
      
      obj->AM = h;
      // log("AM:", h.norm()  );
      
      // vertical com motion 
      if( true ) {
	vector<vec3, 1> vert;
	vert(0) = vec3::UnitY();
	
	auto proj = func::euclid::project(vert);
	
	auto com_proj = proj << phys::rigid::com(tracking->skeleton()) << abs;
	
	const ::control::pd<vec1> pd(tracker.p(dt), tracker.d());
	
	track(ctrl, obj.get(), 
	      func::J(com_proj, obj->lie)(obj->conf),
	      pd( com_proj.diff(obj->state()), vec1::Constant(weight.com_height)), // fuck
	      5 * gamma);
      }
      
    }

   
    // TODO remove ?
    void control::contact_objective(::control::system& ctrl, 
				    const phys::system& system, const phys::dof* dof) {
      phys::constraint::unilateral::matrix N;
      phys::constraint::unilateral::vector n;

      // find relevant unilateral directions
      // cones
      // core::each(system.constraint.cones, [&](const phys::constraint::cone* c) {
      // 	  const phys::constraint::unilateral::row& row = 
      // 	    system.constraint.unilateral.matrix.find( c->normal )->second;
	  
      // 	  // if dof is involved, copy
      // 	  if(row.find(dof) != row.end() ) {
      // 	    N[ c->normal ] = row;
      // 	    n[ c->normal ] = system.constraint.unilateral.values.find( c->normal )->second;
      // 	  }
      // 	});

      // any unilateral
      core::each(system.constraint.unilateral.matrix, [&](const phys::constraint::unilateral* c,
      							  const phys::constraint::unilateral::row& row) {
      		   // if dof is involved, copy
      		   if(row.find(dof) != row.end() ) {
      		     N[ c ] = row;
      		     n[ c ] = system.constraint.unilateral.values.find( c )->second;
      		   }
      		 });
      

      // found shit ?
      if(!N.empty()) {
	
	// build dense matrix
	const phys::solver::chunk<phys::constraint::unilateral> cache( N );

	auto f = new ::control::feature( cache.keys.dim() );

	ctrl.feature.matrix[ f ][ dof ] = keep_contact * cache.dense;
	ctrl.feature.values[ f ] = keep_contact * cache.keys.dense( n );

	ctrl.feature.storage.push_back( core::unique( f ) );
      }      
    }

   
    



  }
}
