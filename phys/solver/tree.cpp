#include "tree.h"

#include <boost/graph/depth_first_search.hpp>
#include <boost/graph/undirected_dfs.hpp>
#include <boost/foreach.hpp>

#include <math/nan.h>
#include <boost/graph/kruskal_min_spanning_tree.hpp>
#include <boost/iterator/transform_iterator.hpp>
#include <core/stl.h>

using namespace boost;

namespace phys {
  namespace solver {
    namespace tree {


      typedef std::set<graph::constraints::vertex_descriptor> bad_constraints_type;
      
      struct loops_finder : dfs_visitor<>  {
	bad_constraints_type& loop_constraints;
      
	loops_finder( bad_constraints_type& loop_constraints) : loop_constraints(loop_constraints) { }

	void back_edge(graph::constraints::edge_descriptor e, const graph::constraints& g) {
	  graph::constraints::vertex_descriptor s = source(e, g), t = target(e, g);
	
	  if( g[s].type == graph::vertex::constraint ) {
	    loop_constraints.insert( s );

	    // core::each( boost::out_edges(s, g), [&](graph::constraint::edge_descriptor e) 

	  } else if ( g[t].type == graph::vertex::constraint ) {
	    loop_constraints.insert( t );
	  } else {
	    assert(false && "one vertex at least should be a constraint lol" );
	  }
	}      
		
       };

      bool binary(graph::constraints::vertex_descriptor v, const graph::constraints& g) {
	assert( g[v].type == graph::vertex::constraint );
	return out_degree(v, g) == 2;
      }
    
      bad_constraints_type find_bad_constraints(graph::constraints& g) {
	bad_constraints_type bad;

	// find loops
	undirected_dfs( g, visitor( loops_finder(bad) ).edge_color_map( get(&graph::edge::color, g)) );
	
	// non binary constraints
	graph::constraints::vertex_iterator v, ve;
	for(std::tie(v, ve) = vertices(g); v != ve; ++v) {
	  if( (g[*v].type == graph::vertex::constraint) && (!binary( *v, g)) ) {
	    bad.insert( *v );
	  }
	}



	return bad;
      }

    
      struct edges_converter : dfs_visitor<> {
	tree::constraints& tree;
	edges_converter( tree::constraints& tree ) : tree(tree) { }
	
	// TODO put this in initialize edge instead ? (cheaper ?)
	void tree_edge(graph::constraints::edge_descriptor e, 
		       const graph::constraints& g) {
	  // TODO assert insertion succeeded
	  tree::constraints::edge_descriptor t = add_edge( source(e, g), target(e, g), tree).first;
	  
	  tree[t].matrix = std::move(g[e].matrix);	  // TODO !
	  // tree[t].matrix = g[e].matrix;
	}
      };


      // clean bad constraint vertices O( k log(k) )
      analysis::isolated_type clean_bad_constraints(const bad_constraints_type& bad, graph::constraints& g) {
	analysis::isolated_type res;
	bad_constraints_type::const_reverse_iterator c, ce;

	// remove edges
	for( c = bad.rbegin(), ce = bad.rend(); c != ce; ++c) {
	  assert( g[*c].type == graph::vertex::constraint );
	  clear_vertex(*c, g);
	}
	
	std::set<graph::constraints::vertex_descriptor> to_remove;

	// for each isolated vertices
	graph::constraints::vertex_iterator v, ve;
	for(std::tie(v, ve) = vertices(g); v != ve; ++v ) {
	
	  if( out_degree(*v, g) == 0 ) {
	    switch( g[*v].type ) {
	    case graph::vertex::constraint : 
	      res.constraints.insert( graph::as< phys::constraint::bilateral >(*v, g) );
	      break;
	    case graph::vertex::dof: 
	      res.dofs.insert( graph::as< phys::dof >(*v, g) );
	      break;
	    }
	    
	    to_remove.insert( * v );
	  }
	
	}
      
	// remove isolated vertices
	BOOST_REVERSE_FOREACH( graph::constraints::vertex_descriptor r, to_remove ) {
	  remove_vertex(r, g);
	}
      
	return res;

      }


      void convert(tree::constraints& res, graph::constraints& g) {
	// auto edges = boost::edges(g);
	// auto vertices = boost::vertices(g);

	const math::natural n = boost::num_vertices(g);
	res = tree::constraints( n );
	
	for(math::natural i = 0; i < n; ++i) {
	  res[ boost::vertex(i, res) ].type = g[ boost::vertex(i, g) ].type;
	  res[ boost::vertex(i, res) ].data = g[ boost::vertex(i, g) ].data;
	}
	
	undirected_dfs( g, visitor( edges_converter( res ) ).edge_color_map( get(&graph::edge::color, g)) );
     
	// for some reason this does not work:
	// for(auto e = edges.first; e != edges.second; ++e) {
	//   tree::constraints::edge_descriptor t = add_edge( source(*e, g), target(*e, g), res).first;
	//   res[t].matrix = g[*e].matrix;
	// }
	
      }


      analysis make(graph::constraints g) {
	analysis res;
	
	const bad_constraints_type bad = find_bad_constraints(g);
	
	// first remove garbage
	res.isolated = clean_bad_constraints(bad, g);
	
	convert(res.tree, g );
	return res;
      }


      analysis span(graph::constraints g) {
	using namespace boost;
	auto weight = get(&graph::edge::weight, g);
	std::vector < graph::constraints::edge_descriptor > spanning_tree;
	
	kruskal_minimum_spanning_tree(g, std::back_inserter(spanning_tree),
				      weight_map(weight));

	// adaptor to obtain pairs from edges
	struct  {
	  const graph::constraints& g;
	  
	  typedef std::pair<math::natural, math::natural> result_type;
	  result_type operator() (graph::constraints::edge_descriptor e) const {
	    return std::make_pair(source(e, g), target(e, g) );
	  }
	  
	} pair{g};
	
	
	// recreate only spanning tree edges
	graph::constraints g_tree(boost::make_transform_iterator(spanning_tree.begin(), pair),
				  boost::make_transform_iterator(spanning_tree.end(), pair),
				  num_vertices(g),
				  spanning_tree.size());
      
	core::each(vertices(g), [&](graph::constraints::vertex_descriptor v) {
	    g_tree[v] = g[v];
	  });
	
	core::each(spanning_tree, [&](graph::constraints::edge_descriptor e) {
	    g_tree[e] = g[e];
	  }); 
	
	return make(g_tree);
      }

      
      writer::writer(const constraints& input) : input(input) { }

      // TODO enhance :) moar bling
      void writer::operator()(std::ostream& out, const constraints::vertex_descriptor& v) const {
	out << "[label=\"";
	
	switch( input[v].type ) {
	case graph::vertex::dof:
	  out << "D - " << v;
	  break;
	case graph::vertex::constraint:
	  out << "C - " << v ;
	  break;
	}
	out << "\"]";
      }
      
     
    }
  }
}
