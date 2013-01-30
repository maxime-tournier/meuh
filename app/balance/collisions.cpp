#include "viewer.h"

#include <math/func/SE3.h>

void viewer::collisions() {
  using namespace math;

  geo::aabb scene;
  scene += sceneRadius() * vec3::Ones();
  scene += -sceneRadius() * vec3::Ones();
  
  // octree
  coll::hierarchy hierachy(scene, character->skeleton.size());
  
  // ground
  coll::group ground;
  ground.primitives.push_back(&this->ground.model);
  hierachy.add(scene, &ground);
  
  // character
  coll::group char_group;
  std::vector< coll::vertices > char_prims(this->character->skeleton.size());
  
  character->skeleton.each([&](math::natural i) {

      if( exclude.find( character->skeleton.body(i).name ) != exclude.end() ) return;
      
      geo::aabb box;
      
      auto g_i = (*character->proxy.lock())(i);
      
      real x = -1.0, y = -1.0, z = -1.0;

      natural off = 0; 

      char_prims[i].data.resize( 8 );
      char_prims[i].coll::primitive::data = character.get();

      for(natural j = 0; j < 2; ++j) {
	for(natural k = 0; k < 2; ++k) {
	  for(natural m = 0; m < 2; ++m)  {
	    
	    // absolute mapped vertex
	    vec3 v = g_i( 0.5 * vec3(x, y, z).cwiseProduct( character->skeleton.body(i).dim ) );
	    
	    char_prims[i].data(off) = v;
	    
	    box += v;
	    
	    ++off;
	    z *= -1.0;
	  }
	  y *= -1.0;
	}
	x *= -1.0;
      }
      
      char_group.primitives.push_back(&char_prims[i]);
    });
  
  hierachy.add(scene, &char_group);
  
  auto hits = hierachy.hits();
  
  math::vec3 sum = math::vec3::Zero();
  math::natural count = 0;
  
  contacts.clear();
  contacts.map(character.get(), [&](const coll::hit& h) {
      auto& pos = h.pos;
      
      // which primitive is mine ?
      auto index = [&](const coll::primitive* p) {
	return std::distance(&char_prims[0],
			     (coll::vertices*)p);
      };
      
      auto mine =  [&](const coll::primitive* p) {
	int i = index(p);
	return i >= 0 && i < int(char_prims.size());
      };
      
      auto p = mine(h.first) ? h.first : h.second;
      
      // like a boss
      int i = index(p);
      
      // fear
      assert( dynamic_cast< const coll::vertices* >(p) );
      assert( p );
      assert(i >= 0);
      assert(i < char_prims.size());
      
      auto frame = func::coord< phys::rigid::config >(i) << character->absolute();

      vec3 local = frame(character->g()).inverse()(pos);
		
      // FIXME HACK
      sum += pos;
      count ++;
		
      return func::rigid_map<>(local) << frame;
    });
  
  // contacts.update( hits );
  contacts.update_cones( hits, 4 );
  

  if(count) support.center.reset( new math::vec3( sum / count ) );
  else support.center.reset();
  

  if( support.center ) {

    vec target = com.target.pos();
    target.x() = support.center->x();
    target.z() = support.center->z();

    com.target.transform( math::SE3::translation( target ) );
  }


  // core::log("hits:", hits.size());
  // now contact.geometry and contact.error should be

}


