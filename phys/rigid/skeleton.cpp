#include "skeleton.h"

#include <boost/graph/depth_first_search.hpp>


namespace phys {
  namespace rigid {
    
  
    skeleton::topology_type::edge_descriptor skeleton::in_edge(topology_type::vertex_descriptor v) const {
      assert( boost::in_degree(v, topology) == 1 );
      
      if(v == root) throw std::logic_error("root has no in_edge");
      return *boost::in_edges(v, topology).first;
    }

    skeleton::topology_type::vertex_descriptor skeleton::parent(topology_type::vertex_descriptor v) const {
      return boost::source( in_edge(v), topology );
    }
    


    static skeleton::topology_type::vertex_descriptor find_root( const skeleton::topology_type& g) {
      skeleton::topology_type::vertex_descriptor v = *boost::vertices(g).first;
      
      while( boost::in_degree(v, g) == 1 ) {
	v = *boost::inv_adjacent_vertices(v, g).first;
      }
      
      return v;
    };


    static math::real sum_masses(const skeleton::topology_type& g) {
      math::real res = 0;

      core::each(boost::vertices(g), [&](skeleton::topology_type::vertex_descriptor v) {
	  res += g[v].mass;
	});
      
      return res;
    }

    
    // TODO
    static void check_cycles(const skeleton& ) { }
    static void check_connected_components(const skeleton& ) { }


    typedef  std::vector< skeleton::topology_type::vertex_descriptor > cache_type;

    static cache_type make_prefix(const skeleton& s) {
      cache_type res;

      struct help : boost::dfs_visitor<> {
	cache_type& cache;
	help(cache_type& cache) : cache(cache) { }
	
	void discover_vertex(const skeleton::topology_type::vertex_descriptor& v, 
			     const skeleton::topology_type& ) const {
	  cache.push_back( v );
	}
	  
      };
	
      boost::depth_first_search(s.topology, boost::visitor( help(res) ).root_vertex(s.root) );
      
      return res;
    }

     static cache_type make_postfix(const skeleton& s) {
      cache_type res;

      struct help : boost::dfs_visitor<> {
	cache_type& cache;
	help(cache_type& cache) : cache(cache) { }
	
	void finish_vertex(const skeleton::topology_type::vertex_descriptor& v, 
			   const skeleton::topology_type& ) const {
	  cache.push_back( v );
	}
	  
      };
	
      boost::depth_first_search(s.topology, boost::visitor( help(res) ).root_vertex(s.root) );
      
      return res;
    }



    skeleton::skeleton(const topology_type& topology) 
      : topology(topology),
	root(find_root(topology)),
	mass(sum_masses(topology)),

	_prefix(make_prefix(*this)),
	_postfix(make_postfix(*this))
    {

      check_cycles(*this);
      check_connected_components(*this);

    }

    math::natural skeleton::size() const { return boost::num_vertices(topology); }

    skeleton::topology_type::vertex_descriptor skeleton::find(const std::string& name) const {
      math::natural none = -1;
      math::natural res = none;

      core::each( boost::vertices(topology), [&](math::natural i) {
	  if( topology[i].name == name ) res = i;
	});

      if( res == none ) throw std::logic_error("no body named " + name );
      
      return res;
    }

    math::lie::group< config > skeleton::lie() const { return { size() }; }
    

    const rigid::body& skeleton::body(topology_type::vertex_descriptor v) const { return topology[v]; }
    const rigid::joint& skeleton::joint(topology_type::edge_descriptor e) const { return topology[e]; }
    
  }
}
