#include "adaptor.h"

#include <mocap/skeleton.h>
#include <mocap/offset.h>

#include <phys/rigid/articulated.h>
#include <math/func/lie/translation.h>
#include <math/func/compose.h>
#include <math/func/coord.h>

#include <core/log.h>
#include <core/string.h>

namespace tool {


  // returns the first ancestor of j in the node map
  static auto parent(const  std::map<mocap::skeleton*, math::natural>& nodes,
		     mocap::skeleton* j) -> decltype( nodes.end() ) {
    if( !j ) return nodes.end();
      
    auto it = nodes.find(j);
    if( it != nodes.end() ) return it;
	
    return parent(nodes, j->parent());
  }


  static math::SE3 offset(const math::vec3& offset) {
    using namespace math;
      
    // we want the y axis to be aligned with the bone
    quat q; q.setFromTwoVectors( vec3::UnitY(), offset );
      
    // bvh: z = front, y = up
      
    // but at the same time, we'd like the z axis (front) to be
    // preserved (when possible)
      
    // front vector in local frame
    vec3 local_z = q.inverse() * vec3::UnitZ();
    vec3 local_x = q.inverse() * vec3::UnitX();
      
    // projected on z,x plane
    vec3 projz(local_z.x(), 0, local_z.z());
    vec3 projx(local_x.x(), 0, local_x.z());
	  
    quat total = q;
    real eps = 2e-1;

    if( projz.norm() > eps ) {
      // rotate z so that it matches front projection (not touching y)
      quat r; r.setFromTwoVectors(vec3::UnitZ(), projz );
      total = q * r;
    } else if (  projx.norm() > eps ) {
      // try with x
      quat r; r.setFromTwoVectors(vec3::UnitX(), projx );
      total = q * r;
    }
	  
    // return SE3::identity(); 
    return SE3::rotation(total) * SE3::translation( 0.5 * offset.norm() * vec3::UnitY() )
      ; 
  }


  // obtains the offsets for the selected nodes
  static math::vector< math::SE3 > make_offset(const adaptor::index_type& nodes) {

    math::vector< math::SE3 > res;
    res.resize(nodes.size());
      
    core::each(nodes, [&](mocap::skeleton* j, math::natural i) {
	res(i) = offset( mocap::offset(j) );
	  
      });

    return res;
  }
    


  // associates "real" bones to integers
  static adaptor::index_type make_index(mocap::skeleton* s) {
    std::map<mocap::skeleton*, math::natural> res;

    math::natural count = 0;

    s->each([&](mocap::skeleton* j){
	    
	// is this a "real" rigid body ?
	if( mocap::length(j) ) res[j] = count++;
	  
      });
      
    return res;
  }
	
    

  math::natural adaptor::reverse_index(math::natural i) const {

    math::natural res = -1;
    core::each( index, [&](mocap::skeleton* s, math::natural j) {
	if( j == i ) res = s->parent()->get().id;
      });
    if( int(res) == -1 ) throw std::logic_error( std::string("unknown skeleton index: ") + core::string(i));
    
    return res;
  }


  // builds the topology for the selected "real" bones
  static phys::rigid::skeleton::topology_type make_topology(const adaptor::index_type& nodes, 
							    const bio::info& info){
      
    // result
    using namespace phys::rigid;
    articulated g(nodes.size());
      
    using namespace math;
    using phys::rigid::body;

    // maps to get bone start/end frames
    auto start = [&](const body& b) { return SE3::translation( -0.5 * b.dim.y() * vec3::UnitY()); };
    auto end = [&](const body& b) { return SE3::translation( 0.5 * b.dim.y() * vec3::UnitY()); };
    
    // 1.for each node, fill the graph      
    std::set<mocap::skeleton*> roots;
    
    core::each(nodes, [&](mocap::skeleton* j, math::natural ci) {

	body& b = g[ci];

	b.name = j->get().name;

	// setup dimensions/mass
	b.dim = info.dim( b.name );
	b.mass = info.mass( b.name );
	b.inertia = info.inertia( b.name );
	  
	assert( std::abs(mocap::length(j) - b.dim.y()) < math::epsilon );
      });


    // connect to parents
    core::each(nodes, [&](mocap::skeleton* j, math::natural ci) {
	auto p = parent(nodes, j->parent() );
	  
	if( p == nodes.end() ) roots.insert(j);
	else {
	  math::natural pi = p->second;
	  articulated::edge_descriptor e = boost::add_edge(pi, ci, g).first;
	  
	  phys::rigid::frame parent{pi, end(g[pi])}, child{ci, start(g[ci])};
	  g[ e ] = phys::rigid::joint{ parent, child};
	};
      });
      
    core::logger log;
      
    // deal with multiple roots
    if( roots.size() > 1 ) {
      // log("got", roots.size(), "roots" );

      mocap::skeleton* root = *roots.begin();
      math::natural rooti = nodes.find(root)->second;
      roots.erase(root);

      core::each(roots, [&](mocap::skeleton* j) {
	  const math::natural ji = nodes.find(j)->second;
	    
	  articulated::edge_descriptor e = boost::add_edge(rooti, ji, g).first;

	  g[e] = phys::rigid::joint{ {rooti, start(g[rooti])}, {ji, start(g[ji])} };
	    
	});
    }
      
    math::natural root = 0;
    
    // TODO FIXME HARDCODE
    core::each(boost::vertices(g), [&](math::natural i) {
	if(g[i].name == "lowerback") root = i;
      });
      
    return topology::direct(g, root);
  }




  // first select non-zero bones, then map absolute configurations
  adaptor::adaptor(mocap::skeleton* s, const bio::info& info)
    : index( make_index(s) ),
      skeleton( make_topology(index, info) ),
      offset( make_offset(index) )
  {
      
  }


  adaptor::range adaptor::operator()(const domain& g) const {
    range res;
    
    res.resize( skeleton.size() );

    core::each(index, [&](mocap::skeleton* j, math::natural i) {
	res(i) = math::func::lie::R(offset(i))
	  ( g(j->get().id) );
      });

    return res;
  }

  // TODO REDO

  // math::T<adaptor::range> adaptor::diff(const math::T<domain>& dg) const {
  //   range at;
  //   at.resize( skeleton.size() );
    
  //   math::Lie<range>::algebra body;
  //   body.resize( skeleton.size() );
    
    
  //   core::each(index, [&](mocap::skeleton* j, math::natural i) {
	
  // 	using namespace math::func;
  // 	auto i_th = coord<math::SE3>( j->get().id );
  // 	auto res_i = (lie::R(offset(i)) << i_th).diff(dg);
	
  // 	at(i) = res_i.at();
  // 	body(i) = res_i.body();
	
  //     });

  //   return math::body( std::move(at), std::move(body) );
  // }


}


