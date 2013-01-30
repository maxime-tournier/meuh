#include <gui/app.h>

#include "viewer.h"

#include <tool/mocap.h>

#include <script/meuh.h>
#include <core/share.h>

#include <math/func/compose.h>

#include <phys/rigid/relative.h>
#include <phys/rigid/absolute.h>

#include <math/func/SE3.h>

#include <tool/to_lua.h>

#include <gui/convert.h>
#include <gl/common.h>

// std::unique_ptr<viewer::ragdoll_type> make_ragdoll(const std::string& filename) {
  
//   tool::mocap mocap( filename );
  

//   auto res = new viewer::ragdoll_type( mocap.skeleton(),
// 				       math::lie::group<math::vector<math::SO3> >( mocap.skeleton().size() - 1));
//   using namespace math::func;
//   math::natural n = mocap.skeleton().size();
//   // std::get<1>(res->g())

//   math::real start = ::mocap::start( mocap.clip.data );
  
//   res->cache =  mocap.absolute()( start );
//   res->init = res->project( res->cache );
//   res->g() = res->init;
  
//   res->rest = std::get<1>(res->init);
  
//   return std::unique_ptr<viewer::ragdoll_type>( res );
// }

std::unique_ptr<viewer::reduced_type> make_reduced(const std::string& filename, math::natural n) {
  
  tool::mocap mocap( filename );

  auto res = new viewer::reduced_type( mocap, n );
  
  std::get<1>( std::get<0>(res->init) ) = 16 * math::vec3::UnitY();
  res->g() = res->init;
  
  // core::log( tool::to_lua(res->skeleton,
  // 			  res->absolute()( res->g())) );
  
  // script::require("json");
  // script::exec(// "json.save('/tmp/skeleton.lua', " +
  // 	       std::string("skel = ") + 
  // 	       tool::marshall::skeleton(res->skeleton) 
  // 	       // " ) "
  // 	       );

  return std::unique_ptr<viewer::reduced_type>( res );
}




int main(int argc, char** argv) {
  gui::app app(argc, argv);
  
  viewer wid;
 
  script::api api("balance");
  
  api
    .fun("load", [&](const std::string& name, math::natural k) {
      std::string full = core::share("/mocap/") + name;

      // wid.character = make_ragdoll( full );
      wid.character = make_reduced( full, k );
      wid.setup();
      wid.straighten();
      wid.character->init = wid.character->g();
    })
    .fun("stiff", [&](math::real k ) {
	wid.character->stiffness = k;
      })
    .fun("damping", [&](math::real k ) {
	wid.character->damping = k;
      })
    .fun("com", [&](math::real k) {
	wid.com.stiffness = std::sqrt(k);
      })
    .fun("am", [&](math::real k) {
	wid.am.stiffness = std::sqrt(k);
      })
    .fun("lazy", [&](math::real k) {
	wid.laziness = k;
      })
    .fun("smooth", [&](math::real k) {
	wid.smoothness = k;
      })
    .fun("support", [&](math::real k) {
	wid.support.weight = std::sqrt(k);
      })
    .fun("zmp", [&](math::real k) {
	wid.zmp.stiffness = std::sqrt(k);
      })
    .fun("ref", [&](math::real k) {
	wid.reference.stiffness = std::sqrt(k);
      })
    .fun("save", [&] {
	wid.character->init = wid.character->g();
      })
    .fun("iter", [&](math::natural n) {
	wid.iter.bound = n;
      })
    .fun("eps", [&](math::real e) {
	wid.iter.epsilon = e;
      })
    .fun("solver", [&](math::natural i) {
	wid.solver = viewer::solver_type(i);
      })
    .fun("log_act", [&] {
	core::log(wid.character->act.transpose());
      })
    .fun("straighten", [&] {
	wid.straighten();
      })
    .fun("hints", [&](bool b) {
	wid.flag.hints = b;
      })
    .fun("exclude", [&](const std::string& s) {
	if(s.empty()) wid.exclude.clear();
	else wid.exclude.insert( s );
      })
    .fun("head", [&](math::real x) {
	wid.head = x;
      })
    .fun("gravity", [&](bool b) {
	wid.flag.gravity = b;
      })
    .fun("pull", [&](bool b) {
	wid.flag.pull = b;
	wid.setup();
      })
    .fun("spin", [&](math::real b) {
	wid.spin( b );
      })
    .fun("grab", [&](math::real b) {
	wid.grab = b;
      })
    .fun("steps", [&](math::natural b) {
	wid.steps = b;
	wid.setup();
      })
    .fun("pos", [&](math::real x,
		    math::real y,
		    math::real z) {
	   if(!wid.character) return;
	   std::get<1>(std::get<0>(wid.character->g())) = math::vec3(x, y, z);
	 })
    .fun("sleep", [&](math::real s) {
	std::this_thread::sleep_for(  std::chrono::milliseconds( int(s * 1000) ) );
      })
    .fun("light_pos", [&](math::real x, math::real y, math::real z) {
	wid.light->setPosition( gui::convert( math::vec3(x, y, z) ) );
      })
    .fun("light_lookat", [&](math::real x, math::real y, math::real z) {
	wid.light->lookAt( gui::convert( math::vec3(x, y, z) ) );
      })
    .fun("light_draw", [&](bool b) {
	wid.flag.draw_light = b;
      })
    .fun("char_color", [&](math::real x, math::real y, math::real z) {
	wid.char_color = math::vec3(x, y, z);
      })					
    .fun("bretelles", [&](bool b) {
	wid.flag.bretelles = b;
      })
    .fun("pick_stiff", [&](math::real r) {
	wid.pick.stiff = r;
      })
    .fun("fancy", [&](bool b) {
	wid.flag.fancy = b;
      })
    
    ;
  
  wid.show();

  script::app("balance");   // TODO move this call to app.script("balance")
  return app.console().exec();
};
