#include "topology.h"

#include <boost/graph/depth_first_search.hpp>
#include <boost/graph/undirected_dfs.hpp>
#include <boost/property_map/property_map.hpp>

#include <core/log.h>
#include <core/stl.h>

namespace phys {
  namespace rigid {
    namespace topology {

      directed direct(undirected g, undirected::vertex_descriptor root) {
	
	using namespace boost;
	directed res(num_vertices(g));

	core::each(vertices(g), [&](undirected::vertex_descriptor v) {
	    res[v] = g[v];
	  });
	
	struct vis : dfs_visitor<> { 
	  directed& res;
	  vis(directed& res) : res(res) { }

	  // void discover_vertex(undirected::vertex_descriptor v, const undirected& g) {
	  //   core::log()("dfs: discovering vertex", g[v].name );
	  // }

	  void tree_edge(undirected::edge_descriptor e,  const undirected& g)  {
	    add_edge(source(e, g), target(e, g), g[e], res);
	  }
	};
	
	undirected_dfs(g, 
		       visitor(vis(res))
		       .edge_color_map( get(edge_color_t(), g) )
		       .root_vertex( root )
		       );
	
	return res;
      }
    }
  }
}
