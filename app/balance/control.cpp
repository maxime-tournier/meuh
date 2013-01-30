#include "viewer.h"

#include <math/func/jacobian.h>

#include <phys/constant.h>
#include <phys/rigid/momentum.h>
#include <phys/rigid/com.h>


phys::step::control viewer::control(const math::mat& M,
				    const math::mat& Minv) {

  using namespace phys;
  step::control control;


  
  using namespace math;
 
  control.act.matrix = math::mat::Zero( character->inner_dofs(),
					character->dim() );

  vec scaling = character->pga.data.dev.head( character->inner_dofs() );
  control.act.matrix.rightCols( character->inner_dofs() ).diagonal().setConstant( dt );
   

  // features 
  assert( features );
  features->clear();
  
  
  // if(!support.center)  return control;

  auto ensure = [&](math::real r) -> math::real {
    if( !support.center ) return 0;
    return r;
  };
  

  // // core::log("actuator scaling:", scaling.transpose());
  
  // auto& N = contacts.K;

  // // control objectives
  // natural a = character->inner_dofs();
  // natural offset = 8 + a;
  // natural total = offset + N.geometry.rows().dim();

  // control.obj.matrix = math::mat::Zero( total, character->lie.alg().dim() );
  // control.obj.vector = math::vec::Zero( total );


  
  // com
  auto com = phys::rigid::com(character->skeleton) << character->absolute();

  vector< vec3, 2 > basis;
  basis(0) = vec3::UnitX();
  basis(1) = vec3::UnitZ();
    
  auto proj = math::func::project(basis);

  mat J = math::func::J(character->absolute(), character->lie, character->skeleton.lie())( character->g() );

  mat L = math::func::J(com, character->lie)( character->g() );
  mat H = phys::rigid::momentum::angular(character->skeleton, *character->proxy.lock() ) * J;
  
  vec3 c = com(character->g());

  // center of support
  vec3 p;
  
  if( support.center ) p = *support.center;
  
  math::vec vel = math::vec::Zero(character->lie.alg().dim());
  math::euclid::get(vel, character->v() );
  
  vec3 h = H * vel;
  vec3 l = L * vel;
    
  vec3 grav = - character->skeleton.mass * phys::constant::g * vec3::UnitY();
    
  vec3 d = p - c;
     

  mat23 P;
  P.row(0) = basis(0).transpose();
  P.row(1) = basis(1).transpose();

  
  auto lf = character->skeleton.find("lfoot");
  auto rf = character->skeleton.find("rfoot");
  auto lh = character->skeleton.find("lfingers");
  auto rh = character->skeleton.find("rfingers");
  auto hd = character->skeleton.find("head");
  
  vec3 lf_local = (0.5 * character->skeleton.body(lf).dim).cwiseProduct( vec3::UnitY() );
  vec3 rf_local = (0.5 * character->skeleton.body(rf).dim).cwiseProduct( vec3::UnitY() );
 
  typedef phys::rigid::config rigid_config;
  auto lf_abs =  func::rigid_map<>(lf_local) << func::coord<rigid_config>(lf) << character->absolute();
  auto rf_abs =  func::rigid_map<>(rf_local) << func::coord<rigid_config>(rf) << character->absolute();
 
  auto lh_abs =  func::get_translation<>() << func::coord<rigid_config>(lh) << character->absolute();
  auto rh_abs =  func::get_translation<>() << func::coord<rigid_config>(rh) << character->absolute();
  
  auto hd_abs =  func::get_translation<>() << func::coord<rigid_config>(hd) << character->absolute();
  
  auto mid_feet = 0.5 * (lf_abs + rf_abs);

  // this ensures we don't have any component on root jacobian
  auto filter = func::part<1>(character->g()) << func::get<1>(character->g());
  
  auto grab_left = (rh_abs - rf_abs) << filter;
  auto grab_right = (rh_abs - lf_abs) << filter;
  auto grab_feet = (lf_abs - rf_abs) << filter;

  
  real alpha = 0.5;
 
  tool::control::gains pd{alpha, 1 - alpha};
  
  // features->add("grab-left", this->grab, grab_left, character.get() );
  features->add("grab-right", this->grab, grab_right, character.get(), pd ); 
  features->add("grab-feet", 1.2 * this->grab, grab_feet, character.get(), pd );
  
  // com-mid
  auto com_mid = proj << (com - mid_feet);
  features->add("com-mid", ensure(this->com.stiffness), com_mid, character.get(), pd );
  
  // head-mid
  auto head_mid = proj << (hd_abs - mid_feet);
  features->add("head-mid", ensure(this->head), head_mid, character.get(), pd );
  
  // reference
  natural a = character->inner_dofs();

  auto reference = func::tuple_get<1, character_type::config_type>();
  features->add("ref", ensure(this->reference.stiffness), reference, character.get(), pd, lie::group<vec>(a) );
  
  
  


  // features->add("com-support", 2, [&]( tool::control::mat_block M,
  // 				       tool::control::vec_block v) {
		  
  // 		  M = P * func::J(com, character->lie)( character->g() ); 
  // 		  v = P * (  -(1 - alpha) * ( com(character->g()) - p )
  // 			     - alpha * func::d(com)(character->g())(character->v() ));
  // 		}, this->com.stiffness );
  


  features->add("zmp", 3, [&]( tool::control::mat_block M,
			       tool::control::vec_block v) {
		  
		  M = H - math::hat(d) * L;
		  v = h - d.cross( l + dt * grav);
		}, ensure( this->zmp.stiffness / character->skeleton.mass ));
  
  features->add("am", 3, [&]( tool::control::mat_block M,
			      tool::control::vec_block ) {
		  M = H;
		}, ensure(this->am.stiffness / character->skeleton.mass) );
  
  
 
 
  // sticky contacts

  auto& N = contacts.K;
  if( N.geometry.rows().dim() ) {
    
    features->add("contacts", N.geometry.rows().dim(), [&] ( tool::control::mat_block M,
							     tool::control::vec_block ) {
		    natural row = 0;
		    
		    N.geometry.each([&](constraint::unilateral::key r,
					dof::key c,
					sparse::mat_chunk v) {
				      M.block(row, 0, v.rows(), M.cols() ) = this->support.weight * v;
				      row += v.rows();
				    });
		    
		  }, ensure( this->support.weight ) );
    
  }  
  
  // x
  if( solver == full ) {
    
    assert( !Minv.empty() );
    control.metric.matrix = this->laziness * 
      control.act.matrix * Minv * control.act.matrix.transpose()
    //    + mat::Identity(a, a);	// fills kernel of the above
    ;


    control.metric.vector = vec::Zero( a );
    
    control.metric.matrix += smoothness * mat::Identity(a, a);
    control.metric.vector -= smoothness * character->act;

    
  }
  
  // math::vec bob = control.act.matrix * vel;
  // control.metric.matrix += smoothness * bob * bob.transpose();
  
  
  features->update();
  
  // // TODO this is costly lol
  if( features->dim() ) {
    control.obj.vector = // features->matrix.transpose() * 
      features->vector;
    control.obj.matrix = // features->matrix.transpose() * 
      features->matrix;
  }
  
  return control;
}

