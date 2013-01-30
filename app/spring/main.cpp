
#include <math/vec.h>
#include <math/tuple.h>
#include <math/form.h>
#include <math/mat.h>

#include <math/func/vector.h>
#include <math/func/covector.h>

#include <tool/simulator.h>

#include <phys/object.h>
#include <phys/system.h>

#include <phys/spring.h>
#include <phys/constant.h>

#include <phys/solver/task.h>
#include <phys/solver/simple.h>
#include <phys/solver/cg.h>
#include <phys/solver/cholesky.h>
#include <phys/solver/minres.h>

#include <phys/step/simple.h>
#include <phys/step/kinematic.h>

#include <gl/common.h>

#include <QApplication>

#include <math/func/sqrt.h>
#include <math/func/euclid.h>
#include <math/func/constant.h>
#include <math/func/jacobian.h>

#include <gl/sphere.h>
#include <gl/init.h>

#include <script/console.h>
#include <thread>

struct viewer : tool::simulator {

  typedef phys::object< math::vec3 > obj_type;

  std::unique_ptr< obj_type > obj;

  phys::spring spring;

  typedef phys::constraint::bilateral constraint_type;
  std::unique_ptr< constraint_type > constraint;
  
  math::real mass;
  math::vec3 origin;

  math::real order;
  
  gl::vbo sphere;

  std::function< phys::solver::any () > make_solver;
  script::api api;		// 

  viewer() 
    : api("spring")
  {
    api
      .fun("cg", [&] {
	  make_solver = [&]() -> phys::solver::any {
	    return phys::solver::make_cg( phys::solver::simple(data.system),
					  data.system );
	  };
	})
      .fun("cholesky", [&] {
	   make_solver = [&]() -> phys::solver::any {
	     return phys::solver::make_cholesky( phys::solver::simple(data.system),
						 data.system );
	   };
	})
      .fun("minres", [&] {
	  make_solver = [&]() -> phys::solver::any {
	    return phys::solver::minres( data.system );
	  };
	});
    
  }

  void init() {
    
    mass = 1.0;
    origin.setZero();

    spring.stiffness = 100.0;
    spring.rest = 1.0;
    post_steps = 0;

    order = 1.0;
    
    sphere.send( gl::sphere(0.05) );
    
    physics();
    reset();

    setAnimationPeriod( dt / phys::unit::ms );
    startAnimation();

    tool::pickable pickable;
    pickable.draw = [&] {

      if( !obj ) return;

      using namespace gl;
      
      matrix::push();
      translate( origin );
      sphere.draw();
      matrix::pop();


    };

    pickable.pick = [&](const math::vec3& at) -> math::vec3 {
      origin = at;
      return at;
    };

    
    picker.add( pickable );

    make_solver = [&]() -> phys::solver::any {
      return phys::solver::minres(data.system);
    };
  }

  void physics() {
    
    using namespace math;
    using namespace func;

    auto norm = func::sqrt() << func::euclid::squared_norm<vec3>();
    auto delta = id<vec3>() - val(origin);

    auto dist = (norm << delta) - cst<vec3>(spring.rest);

    auto h = std::sqrt(spring.stiffness) * dist;
    auto V = func::norm2( h );
    
    auto force = [&,V,h](math::real t, math::real dt) {
      phys::dof::force f;

      auto fi = f(obj.get());
      
      // gravity
      fi -= phys::constant::g * data.system.mass(obj.get()) * math::vec3::UnitY();

      // spring force
      if( V(obj->g() ) > 0 ) {
	// fi -= func::J( V )( obj->conf );
	fi -= spring.dV(origin, obj->g() ).tail<3>();
      }
      
      return (1.0 / order) * f;
    };
   

    auto solver = [&, V,h]() -> phys::solver::any {
      
      auto& J = data.system.constraint.bilateral.matrix;

      using namespace phys;
       
      if( V( obj->g() ) == 0 ) {
	return solver::simple(data.system);
      }
      
      if( constraint->dim() == 1 ) {
	J(constraint.get(), obj.get()) = -spring.dV(origin, obj->g() ).tail<3>().transpose();
	data.system.constraint.bilateral.damping( constraint.get() ).setConstant( -(order * order) / (dt * dt) );
      } else if( constraint->dim() == 3 ) {
	mat36 Ji;
	vec3 d;

	spring.JTDJ(Ji, d, origin, obj->g() );

	J(constraint.get(), obj.get()) = -Ji.rightCols<3>();
	data.system.constraint.bilateral.damping( constraint.get() ) = -(order * order) / (dt * dt) * d.cwiseInverse();
      }
	
      return make_solver();
    }; 
 
    // animator = phys::step::simple(data, force);
    animator = phys::step::kinematic(data, solver, force);
  }

  void reset() {
    obj.reset( new obj_type );
    obj->g() = spring.rest * math::vec3::UnitX();

    constraint.reset( new constraint_type(3) );
    
    data.clear();
    
    data.engine.add( obj.get() );
    
    data.system.mass( obj.get() ).diagonal().setConstant( mass );
    data.system.resp( obj.get() ).diagonal().setConstant( 1.0 / mass );
  }

  

  


  void draw() {
    if( ! obj ) return;
    
    using namespace gl;
      
    matrix::push();
    translate( obj->g() );
    color( red() );
    sphere.draw();
    matrix::pop();

    matrix::push();
    translate( origin );
    color( red() );
    sphere.draw();
    matrix::pop();

  
    disable( GL_LIGHTING );
    
    begin(GL_LINES, [&] {
	color( green() );
	vertex( origin, obj->g() );
      });

    enable( GL_LIGHTING );

  }

};





int main(int argc, char** argv ) {


  QApplication app{ argc, argv };
  
  auto wid = new viewer;

  wid->show();

  phys::dof::mass test;
  phys::dof::mass taiste = std::move(test);

  std::thread console{ script::console{} };
  
  return app.exec();
}
