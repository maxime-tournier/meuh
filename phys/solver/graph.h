#ifndef PHYS_SOLVER_GRAPH_H
#define PHYS_SOLVER_GRAPH_H

#include <boost/any.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <set>

#include <phys/constraint/bilateral.h>

#include <phys/error.h>
#include <math/matrix.h>

namespace phys {

  namespace solver {
    namespace graph {

      struct vertex {
	enum { constraint, dof } type;
	const void* data;
      };

      struct edge {
	edge() : matrix(), color(), weight(0) { }
	edge(const math::mat& m, math::real w = 0) : matrix(m), color(), weight(w) { }
	
	math::mat matrix;
	boost::default_color_type color;
	math::real weight;

      };


      typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS,
				    vertex, edge> constraints;
     
      struct bad_vertex : core::error {
	bad_vertex();
      };

      // main construction call. O(m.log(n) )
      typedef std::map<phys::constraint::bilateral::key, math::real> weights_type;
      
      constraints make(const phys::constraint::bilateral::matrix& constraints,
		       const weights_type& weights = weights_type());
      
      // graphviz output
      struct writer {
	const constraints& input;
	
	writer(const constraints& input);
	void operator()(std::ostream& out, const constraints::vertex_descriptor& v) const;
      };


      template<class T, class G>
      const T* as( typename G::vertex_descriptor v, const G& g) {
      	return static_cast<const T*>(g[v].data);
      }
      
      // dofs in a graph O(n.log(n)) TODO
      template<class G>
      std::set<const phys::dof*> dofs(const G& g) {
	typename G::vertex_iterator v, ve;
	
	std::set<const phys::dof* > res;
	
	for( tie(v, ve) = boost::vertices(g); v != ve; ++v ) {
	  if( g[*v].type == vertex::dof ) {
	    res.insert( as<phys::dof>(*v, g)  );
	  }
	}
	
	return res;
      };




      // // dimension of a vertex in a graph
      // template<class G>
      // math::natural dimension( typename G::vertex_descriptor v, const G& g) {
      // 	switch( g[v].type ) {
      // 	case vertex::dof: return as< phys::dof >(v, g)->dimension;
      // 	case vertex::constraint: return as< phys::constraint::bilateral >(v, g)->dimension;
      // 	}
      // }

    }
    
  }
  
}

#endif
