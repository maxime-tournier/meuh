#include "viewer.h"

#include <core/share.h>
#include <gl/skeleton.h>

#include <math/func/SE3.h>
#include <math/func/coord.h>
#include <math/func/compose.h>

#include <phys/rigid/com.h>
#include <math/gauss_newton.h>

#include <gui/viewer/fancy.h>

static gl::texture chess, none;


viewer::reduced_type::reduced_type(const tool::mocap& mocap, 
				   math::natural n,
				   const tool::pga::exclude_type& exclude) 
  : reduced(mocap.skeleton(), 
	    math::lie::group<dofs_type>(n)),
    pga(mocap, exclude) {
  
  // TODO FIXME ccsk throw exception !
  pose( pga.data.ccfk(n) );
  
  *proxy.lock() = absolute()( g() );
  *dproxy.lock() = d(absolute())(g())( v() );
  
  
  // pose( math::func::cst< math::vec >( pga.data.mean ) );
  
  damping = 0;
  interaction = zero();  
  
}
 
 

void viewer::spin(math::real x) {
  if(!character) return;

  auto& p = std::get<0>(character->p());

  auto& angular = std::get<0>(p);
  auto& linear = std::get<1>(p);

  angular += x * math::force::UnitY();
  
}

void viewer::straighten() {
  if(!character) return;

  using namespace math;
  auto rot = func::part<0>( std::get<0>(character->g()) );
  auto conf = func::part<0>(character->g()) << rot;
  
  auto full = character->absolute() << conf;

  auto h = character->skeleton.find("head");
  
  auto lf = character->skeleton.find("ltoes");
  auto rf = character->skeleton.find("rtoes");
  
  auto head = func::get_translation<>() << func::coord<phys::rigid::config>(h) << full;
  auto lfoot = func::get_translation<>() << func::coord<phys::rigid::config>(lf) << full; 
  auto rfoot = func::get_translation<>() << func::coord<phys::rigid::config>(rf) << full;
  
  auto mid = 0.5 * (lfoot + rfoot);
  
  auto com = phys::rigid::com(character->skeleton) << full;

  vector< vec3, 2 > basis;
  
  basis(0) = vec3::UnitX();
  basis(1) = vec3::UnitZ();
  
  auto proj = func::project( basis );

  auto fun = proj << ( head - com );
 
  auto levmar = math::make_gauss_newton(fun);
 
  levmar.outer.bound = 5;
  levmar.inner.bound = 10;
  levmar.eps = 1;
  
  levmar.sparse( std::get<0>( std::get<0>(character->g())),
                 vec2::Zero() );
  
}


viewer::viewer() {


  com.stiffness = 0;
  am.stiffness = 0;
  zmp.stiffness = 0;
  reference.stiffness = 0;
  
  laziness = 0;
  smoothness = 0;

  setSceneRadius( 50 );
  setAnimationPeriod( 0 );

  head = 0;
  grab = 0;

  iter.bound = 100;

  ground.model.geo.set( -2 * math::vec3::UnitY(),
			math::vec3::UnitY() );
  
  solver = uncontrolled;
  steps = 1;
  
  char_color = gl::white();

  flag.hints = true;
  flag.gravity = true;
  flag.pull = true;
  flag.draw_light = false;
  flag.bretelles = true;
  flag.fancy = true;
  
  pick.stiff = 2;
}



void viewer::init() {
  
  ground.init( texture("chess-light.png") );
  
  none.gen( texture("none.png") ).nearest().unbind();
  
  // control
  // this->com.target.transform( math::SE3::translation( phys::rigid::com(character->skeleton)(*character->proxy.lock()) ) );
  
  light.reset( new qglviewer::Camera );
  gui::fancy::init(this, light.get() );
  startAnimation();
  
  
}

void viewer::draw() {
  using namespace gl;

  // ground TODO wrap somewhere in gl::plane ? 

  // or better, tool::ground
  
  auto gnd = [&] {
    ground.draw();
  };
  
  
  auto chr = [&] {
    if( character ) {
      none.bind();
      gl::color( char_color );
      
      gl::blend{}([&] {
	  auto proxy = character->proxy.lock();

	  character->skeleton.each([&](math::natural v) {
	      if( exclude.find( character->skeleton.body(v).name ) != exclude.end() ) return;
	    
	      gl::skel::draw_box(character->skeleton, *proxy, v);
	    });
	});
    
  
  
    } else {
      macro_debug("no character lol");
    }  
    none.unbind();
  };


  auto hints = [&] {
 
    disable(GL_LIGHTING);
    // gl::disable( GL_TEXTURE_2D );
    // gl::disable(GL_BLEND);
    // blend()([&] {
	// gl::begin(GL_POINTS, [&] {
	// color( transp(grey() ) );
	auto cb = post_draw.lock();
	if( *cb )(*cb)();
	// });
      // });
    
    if( pick_cb ) pick_cb();
    if( flag.draw_light ) {
      color( white() );
      light->draw();
    }
    
    enable(GL_LIGHTING);
    // gl::enable(GL_BLEND);
    // gl::enable( GL_TEXTURE_2D );
  };
  
  auto full = [&] { chr(); gnd(); };
  
  if( flag.fancy ) {
    gui::fancy::cast( camera(), light.get(), full );
    gui::fancy::draw( camera(), light.get(), full );
  } else {
    full();
  }

  hints();
  
  
}
