#ifndef PHYS_SOLVER_TREE_H
#define PHYS_SOLVER_TREE_H

#include <phys/solver/graph.h>
#include <math/vec.h>

namespace phys {
  namespace solver {
    namespace tree {

      struct vertex : graph::vertex {
	
	math::mat D;
	math::mat D_inv;

	math::mat J;
	
	math::vec solution; 	// TODO this might be a ref instead ?
	
      };
      
      typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::bidirectionalS,
				    tree::vertex, graph::edge> constraints;
      
      struct analysis {
	constraints tree;
	
	// contraints/dofs NOT in the constraint tree (must be solved
	// by an unconstrained solver)
	struct isolated_type {
	  std::set<constraint::bilateral::key> constraints;
	  std::set<dof::key> dofs;
	} isolated;
	
      };
      
      
      // O( m + n + k log(k) ) ( with k cyclic constraints) TODO this is
      // solvable by using listS as vertex container in graph... is
      // this possible ?
      analysis make(graph::constraints g);

      
      // spanning tree based on weights
      analysis span(graph::constraints g);

      // usage: boost::write_graphviz( out, tree, writer(tree) );      
      struct writer {
      const constraints& input;
	
	writer(const constraints& input);
	void operator()(std::ostream& out, const constraints::vertex_descriptor& v) const;
      };

        
  

    }
  }
}

#endif
