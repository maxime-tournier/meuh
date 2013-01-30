#include "reroot.h"

#include <core/tree/algo.h>
#include <core/find.h>
#include <core/stl.h>

#include <math/func/compose.h>
#include <math/func/ref.h>

#include <math/func/SE3.h>
#include <math/func/coord.h>
#include <math/func/tuple.h>

namespace mocap {

   struct by_names {

    bool operator()(mocap::skeleton* j, mocap::skeleton* k) const {
      return j->get().name < k->get().name;
    }

   };


 

  // new to old
  std::map<mocap::skeleton*, mocap::skeleton* > make_node_map(mocap::skeleton* orig, 
							      mocap::skeleton* reroot) {
    std::set<mocap::skeleton*, by_names> orig_nodes;

    orig->each([&](mocap::skeleton* j) { orig_nodes.insert(j); } );
    
    std::map<mocap::skeleton*, mocap::skeleton*> res;
    reroot->each([&](mocap::skeleton* j) {
	res[j] = *core::find(orig_nodes, j);
      });
    
    return res;    
  }



  
  std::set<mocap::skeleton*> make_changed(const std::map<mocap::skeleton*,
					  mocap::skeleton* >& node_map) {
    std::set<mocap::skeleton*> res;
    typedef std::pair<mocap::skeleton*, mocap::skeleton*> node_pair;

    core::each(node_map, [&](mocap::skeleton* current, mocap::skeleton* orig) {
	
	// gather which one has its root changed
	if(current->root() ) return;
	if(orig->root()) return;
	if( orig->parent() != core::find(node_map, current->parent() )->second ) {
	  res.insert( current );
	}
	
      });  

    return res;
  }
 
  // on fixe les branchements pour les joints qui ont change d'orientation
  void fix_changed(const std::set<skeleton* >& changed) {
 
    auto zero_offset = [](skeleton* j) { return j->get().offset.norm() < math::epsilon; };
    auto reversed_branching = [&](skeleton* j) {
      if( j->first() == j->last() ) return false;

      math::natural i = 0;
      j->children( [&](skeleton* c) {
	  if( !zero_offset(c) ) ++i;
	});
      return i == 1;
    };

    std::list<skeleton*> fix;
    core::each(changed, [&](skeleton* j) {

	// we detect which one to fix first, cause topology changes
	if( reversed_branching(j) ) {
	  fix.push_back( j );
	}
      });

    
    core::each(fix, [&](skeleton* j) {
	assert( !j->root() && !j->parent()->root());
	assert( !zero_offset(j) );
	assert( zero_offset(j->parent()) );
	
	// trouver les offsets nuls
	j->exec( core::tree::safe_children( [&](skeleton* c) {
	      if( zero_offset(c) ) {
		j->insert( c->detach() );
	      }
	    }) );
	
      });
    
  }
  
  
  mocap::skeleton* make_skeleton(mocap::skeleton* skeleton, mocap::skeleton* new_root) {
    using namespace core::tree;
    
    mocap::skeleton* copy = core::tree::copy(skeleton);

    // corresponding joint in new skeleton
    mocap::skeleton* n = core::tree::find( [new_root](mocap::skeleton* j) {
	return j->get().name == new_root->get().name;
      })(copy);
    
    // reroot topologique
    copy->reroot( n );
    
   
    
    return n;
  }



  reroot::reroot(mocap::skeleton* orig, mocap::skeleton* joint) 
    : skeleton( make_skeleton(orig, joint) ),
      node_map( make_node_map( orig, skeleton.get() ) ),
      changed( make_changed( node_map ) )
  {
    fix_changed( changed );
    
    // on numerote 
    mocap::number(skeleton.get());
    
    // les offsets
    const pose& orig_teepose = mocap::tee_pose( orig ); 
    using math::func::operator<<;
    const pose& new_teepose = 
      (local(skeleton.get()) << math::func::ref(*this) << global(orig)) (orig_teepose);
    
    skeleton->each( [&new_teepose](mocap::skeleton* j) {
	if(j->root()) j->set().offset.setZero();
	else j->set().offset = new_teepose( j->get().id ).translation();
      });
    
  }
  
  reroot::~reroot() { }


  reroot::range reroot::operator()(const domain& g) const {
    assert( g.rows() == int( node_map.size() ) );
    
    typedef std::pair< mocap::skeleton*, mocap::skeleton*> node_pair;

    range res;
    res.resize(node_map.size());
    
    // TODO check semantic ?
    core::each( node_map, [&](mocap::skeleton* current, mocap::skeleton* orig) {
	res(current->get().id) = g(orig->get().id);
      });
    
    // we should orient these as their old parent
    core::each( changed, [&](mocap::skeleton* j ) {
	using namespace math;
	const natural i = j->get().id;

	mocap::skeleton* old = core::find(node_map, j)->second;
	const natural old_p = old->parent()->get().id;
	
	const vec3 translation = res(i).translation();
	const SO3 rotation = g( old_p ).rotation();
	
	res(i) = math::SE3(translation, rotation);
	
      });

    return res;
  }



  // TODO ! derivative -_-'
  math::T<reroot::range> reroot::operator()(const math::T<domain>& dg) const {
    assert( dg.at().rows() == int( node_map.size() ) );
    using namespace math;
    
    typedef std::pair< mocap::skeleton*, mocap::skeleton*> node_pair;

    range at;
    Lie<range>::algebra body;
    
    at.resize(node_map.size());
    body.resize(node_map.size());
    
    core::each(node_map, [&](mocap::skeleton* current, mocap::skeleton* orig) {
	at(current->get().id) = dg.at()(orig->get().id);
	body(current->get().id) = dg.body()(orig->get().id);
      });
    
    const Lie<domain> dmn(dg.at());

    // we should orient these as their old parent
    core::each( changed, [&](mocap::skeleton* j ) {
	using namespace math;
	const natural i = j->get().id;
	mocap::skeleton* old = core::find(node_map, j)->second;
	const natural old_p = old->parent()->get().id;
	
	using namespace func;
	auto dtrans = func::translation<>().diff( math::body(at(i), body(i) ) );
	auto drot = ( func::rotation<>() << func::coord<SE3>(old_p) ).diff( dg );
	
	const T<SE3> dres_i = func::rigid<>() ( dtrans, drot );
	
	at(i) = dres_i.at();
	body(i) = dres_i.body();
      });

    return math::body(at, body);
  }




}
