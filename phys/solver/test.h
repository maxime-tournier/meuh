#ifndef PHYS_SOLVER_TEST_H
#define PHYS_SOLVER_TEST_H

#include <sparse/matrix.h>
#include <core/graph.h>

#include <phys/dof.h>
#include <phys/constraint/bilateral.h>

#include <unordered_set>
#include <core/storage.h>


namespace phys {

  struct system;

  namespace solver {
      struct task;


    namespace test {
    

      // vertex data
      struct vertex_type {
	vertex_type();
	enum { dof, constraint } type;
	const void* key;

	math::natural dim() const;
      };
      
    
      struct edge_type {
	edge_type();
	math::real weight;
      };
      
      // constraint graph
      typedef core::graph<boost::undirectedS, vertex_type, edge_type> graph_type;
      graph_type graph(const system& );
          
      // spanning trees
      typedef std::vector< graph_type::edge_descriptor > edge_set;
    
      edge_set dfs( graph_type& g, graph_type::vertex_descriptor start = 0);
      edge_set rnd( graph_type& g);
      edge_set cuthill_mckee( graph_type& g );
      edge_set mst( graph_type& g, const constraint::bilateral::weight& w );
      edge_set prop( graph_type& g, const system&, const task& t );
      


      std::set< constraint::bilateral::key > acyclic(const edge_set& edges, 
						     const graph_type& g);


      
      // if you remove an edge @e from @mst, returns all other edges
      // from @g still producing an MST
      edge_set perturb( graph_type& g, const edge_set& mst, graph_type::edge_descriptor e);
      
      // acyclic constraint tree
      typedef core::graph<boost::bidirectionalS, vertex_type> tree_type;

      struct span_type {
	tree_type tree;
	
	// cycle dofs
	typedef std::unordered_set< constraint::bilateral::key > cycles_type;
	
	cycles_type cycles;

	// cycles submatrix
	constraint::bilateral::matrix loop_closures(const system&) const;
	
	std::vector< tree_type::vertex_descriptor > postfix;
      };
    
      span_type span(const graph_type& g, 
		     const edge_set& edges);
    
    
      // data for linear-time solve
      struct data_type {
	span_type span;

	sparse::matrix< dof::key > resp_dof;
	sparse::matrix< constraint::bilateral::key > resp_constraint;

	// wtf ?
	constraint::bilateral::matrix matrix;
      
	// access
	sparse::mat_chunk Di(math::natural i);
	sparse::const_mat_chunk Di(math::natural i) const;
      

	sparse::mat_chunk J(math::natural i);
	sparse::const_mat_chunk J(math::natural i) const;

	void factor(const system& , math::real eps = 0);
	void factor_precond(const system& , math::real eps = 0);
      };


      struct linear {
	data_type data;

	linear(const linear& ) = default;
	linear(linear&& ) = default;
	
	linear( const data_type& );
	linear( data_type&& );
	
	mutable core::storage< math::real > work;
	
	void solve(const task&) const;
	void precond(const task&) const;

      };


      linear make_linear(const system& , math::real eps = 0);

    }
  }
}


#endif
