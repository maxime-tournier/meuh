#ifndef PHYS_SOLVER_LINEAR_H
#define PHYS_SOLVER_LINEAR_H

#include <math/mat.h>
#include <core/graph.h>

#include <phys/dof.h>
#include <phys/constraint/bilateral.h>
#include <core/thread.h>

#include <sparse/storage.h>
#include <unordered_set>

#include <Eigen/LU>
#include <Eigen/SVD>

namespace phys {

  struct system;
 
  
  namespace solver {

    struct task;
    struct linear {
      
      struct vertex {
	vertex();
	
	enum { constraint, dof } type;
	const void* key;

	template<class C> const C* as() const { 
	  assert( key );
	  return static_cast<const C*>(key); 
	}

	math::real off;

	math::mat D;
	
	typedef Eigen::FullPivLU< math::mat::base > inv_type;
	
	inv_type D_inv;
	
	math::mat J;
	
	math::natural dim() const;
	
	math::mat inv(const math::mat& ) const;
      };

      struct edge {
	edge() : J(), weight(0) { }
	edge(const math::mat& J, math::real w = 0) : J(J), weight(w) { }
	
	math::mat J;
	math::real weight;

	boost::default_color_type color;
      };

      // template<class Key>
      // struct storage_type {
      // 	sparse::vector<Key> solution;
      // 	sparse::matrix<Key> D, J;
      // };


      // storage_type<constraint::bilateral> constraint;
      // storage_type<dof> dof;      
      
      typedef core::graph<boost::undirectedS, vertex, edge> graph_type;
      graph_type graph;

      typedef core::graph<boost::bidirectionalS, core::unused, math::mat> tree_type;
      tree_type tree;
      
      typedef std::vector< graph_type::vertex_descriptor > forward_type;
      forward_type forward;
      
      typedef sparse::storage<constraint::bilateral::key, graph_type::vertex_descriptor>::type constraints_type;
      constraints_type constraints;

      typedef sparse::storage<dof::key, graph_type::vertex_descriptor>::type dofs_type;
      dofs_type dofs;

      graph_type::vertex_descriptor id(dof::key d) const;
      graph_type::vertex_descriptor id(constraint::bilateral::key c) const;
      
      // sets dim
      void number();      
      math::natural dim;
      
      linear();
      
      linear(const linear& ) = default;
      linear(linear&& ) = default;
      
      math::real damping;
      
      // setup dof vertices 
      void init_dofs(const dof::mass& M);

      void add(const constraint::bilateral::matrix& );

      typedef std::unordered_set< constraint::bilateral::key > isolated_type;
      // typedef std::vector< constraint::bilateral::key > isolated_type;
      
      void factor();
      
      
      void fill(const dof::momentum& f, const constraint::bilateral::vector& ) const;
    
      void debug( std::ostream& out) const;
      
      void solve(const task& ) const;
      void clear();

      ~linear();
      

      typedef std::vector<linear::graph_type::edge_descriptor> edge_vector;
      isolated_type span(const edge_vector& );


      edge_vector dfs( math::natural start = 0);
      edge_vector rnd();
      
      edge_vector mst(const constraint::bilateral::weight& w);

      edge_vector dfs_weight(const constraint::bilateral::weight& w);

      edge_vector bfs_weight(const constraint::bilateral::weight& w);
      edge_vector bfs_prop(const task& );
      
      edge_vector propagation(const task& );

      constraint::bilateral::weight propagation_weights(const task& );
      
      // dfs span unless spanning tree is given in e
      
      // setup constraint graph
      void init(const phys::system& system);

      isolated_type setup(const edge_vector& e = edge_vector() );

      
      typedef core::thread::local<math::vec> data_type;
      data_type data;

      typedef std::function< bool (constraint::bilateral::key) > acyclic_type;
      acyclic_type acyclic;

      
    };

  }
}



#endif
