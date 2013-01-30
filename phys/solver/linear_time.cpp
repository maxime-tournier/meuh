#include "linear_time.h"

#include <boost/graph/depth_first_search.hpp>
#include <boost/foreach.hpp>

#include <Eigen/LU>
#include <Eigen/Dense>

#include <phys/dof.h>
#include <sparse/linear.h>
#include <sparse/linear/nan.h>
#include <phys/exception.h>

#include <core/macro/here.h>

#include <core/stl.h>
#include <core/find.h>
#include <iostream>

#include <math/svd.h>


namespace phys {
  namespace solver {

    using namespace boost;

    struct set_masses {
      tree::constraints& t;
      set_masses(tree::constraints& t) : t(t) { }
      
      void operator()(const phys::dof* , math::natural i, const math::mat& data) const {
	tree::vertex& n = t[ boost::vertex(i, t) ];
	n.D = data;
      }

      void operator()(dof::key, sparse::zero, const math::mat& ) const {  }
      void operator()(dof::key, math::natural , sparse::zero ) const { 
	throw error("no mass for dof !");
      }
      
    };

    // O( m.(log m + log n) )
    static void update(tree::constraints& g, 
		       const std::map<dof::key, math::natural>& dofs,
		       const std::map<constraint::bilateral::key, math::natural>& constraints,
		       const constraint::bilateral::matrix& matrix) {
      using namespace boost;

      // TODO iterate only in constraints ?
      core::each(matrix, [&](constraint::bilateral::key c, const constraint::bilateral::row& row) {
	  auto cit = constraints.find(c);
	  
	  // not in constraint graph: skip
	  if( cit == constraints.end() ) return;
	  
	  math::natural ci = cit->second;
	  auto vc = vertex(ci, g);
	  
	  core::each(row, [&](dof::key d, const math::mat& J) {
	      
	      auto dit = dofs.find( d );
	      if( dit == dofs.end() ) return;
	      
	      math::natural di = dit->second;
	      auto vd = vertex(di, g);
	      
	      // if edge is in the acyclic graph
	      if(  edge(vd, vc, g).second || edge(vc, vd, g).second ) {
		auto e = edge(vd, vc, g).second ? edge(vd, vc, g).first : edge(vc, vd, g).first;
		
		g[e] = J;
	      }
	      
	    });
	});
    }
    

    // O(m + n) 
    static void factor(tree::constraints& g, const linear_time& solver) {
      
      // set dofs D matrix
      sparse::iter( core::range::all(solver.id.dof), core::range::all(solver.masses), set_masses(g) );

      // set constraint D matrix
      core::each(solver.id.constraint, [&](phys::constraint::bilateral::key c,  math::natural v) {
	  const math::natural d = sparse::linear::dim(c);
	  g[ vertex(v, g) ].D.setZero(d, d);
	});

      // init
      core::each(solver.cache.postfix,  [&](tree::constraints::vertex_descriptor v) {
	  tree::constraints::in_edge_iterator p, ep;
	  tie(p, ep) = in_edges(v, g); // parent edge
	  bool root = (p == ep);

	  switch(g[v].type) {
	  
	  case graph::vertex::dof :
	    if( !root ) g[v].J.noalias() = g[*p].matrix.transpose();
	    break;
	  case graph::vertex::constraint :
	    if( !root )  g[v].J = g[*p].matrix;
	    break;
	  }
	});

      // postfix
      core::each(solver.cache.postfix,  [&](tree::constraints::vertex_descriptor v) {
	  tree::vertex& n = g[v];

	  // for children
	  tree::constraints::adjacency_iterator c, ec;
	  for( tie(c, ec) = adjacent_vertices(v, g); c != ec; ++c) {
	    n.D.noalias() -= g[*c].J.transpose() * g[*c].D * g[*c].J; 
	  }
	
	  // n.D_inv = math::svd(n.D).inverse();
	  n.D_inv = n.D.inverse();

	  tree::constraints::in_edge_iterator f, ef;
	  tie(f, ef) = in_edges(v, g);
	
	  // if not root ?
	  if( f != ef) n.J = n.D_inv * n.J;

	});
    }



    // allocates n.solution, fills accordingly
    struct set_solution {

      tree::constraints& g;
      set_solution(tree::constraints& g) : g(g) { }

      template<class K>
      void operator()(K* k, math::natural i, const math::vec& data) const {
	tree::vertex& n = g[ boost::vertex(i, g) ];
	n.solution = data;
	
	core::unused{k};
	assert(data.rows() == int( sparse::linear::dim(k)));
	assert(!math::nan(data) );
      }


      template<class K>
      void operator()(K* , sparse::zero, const math::vec& ) const { }

      template<class K>
      void operator()(K* k, math::natural i, sparse::zero ) const { 
	tree::vertex& n = g[ boost::vertex(i, g) ];
	n.solution.setZero( sparse::linear::dim(k) );
      }

    };


    // O(m + n)
    void solve_first(tree::constraints& g, const linear_time& solver,
		     const dof::momentum& f,
		     const phys::constraint::bilateral::values& value) {
      
      // TODO improve iterations
      
      // init dofs solution with f
      sparse::iter( core::range::all(solver.id.dof), core::range::all(f), set_solution(g) );
      
      // init constraints solution with value
      sparse::iter( core::range::all(solver.id.constraint), core::range::all(value), set_solution(g) );
      

      // postfix
      core::each(solver.cache.postfix,  [&](tree::constraints::vertex_descriptor v) {
	  tree::vertex& n = g[v];
	
	  // for children
	  tree::constraints::adjacency_iterator c, ec;
	  for( tie(c, ec) = adjacent_vertices(v, g); c != ec; ++c) {
	    n.solution.noalias() -= g[*c].J.transpose() * g[*c].solution;
	  }
	
	});
    }


    
    // O( m + n )
    static void solve_second(tree::constraints& g, const linear_time& solver) {

      // initialize
      core::each(solver.cache.prefix, [&](tree::constraints::vertex_descriptor v) {
	  tree::vertex& data = g[v];
	  data.solution = data.D_inv * data.solution;
	});
	
      core::each(solver.cache.prefix, [&](tree::constraints::vertex_descriptor v) {
	  tree::vertex& n = g[v];
	  
	  tree::constraints::inv_adjacency_iterator p, ep;
	  tie(p, ep) = inv_adjacent_vertices(v, g); 
	
	  // not root
	  if( p != ep ) {	
	    n.solution.noalias() -= n.J * g[*p].solution;
	  }
	});
    }


		       
    // fill maps binding dof/constraint handles to graph vertices
    // warning: O( m.log(m) + n.log(n) )
    void fill(std::map<dof::key, math::natural>& dof,
	      std::map<constraint::bilateral::key, math::natural>& constraint,
	      const tree::constraints& t) {
      dof.clear();
      constraint.clear();
      
      using namespace math;
      const natural n = num_vertices(t);
      for(natural i = 0; i < n; ++i) {
	
	auto v = boost::vertex(i, t);
	
	switch( t[v].type ) {
	case graph::vertex::dof: 
	  dof[ graph::as<phys::dof>(v, t) ] = i;
	  break;
	  
	case graph::vertex::constraint: 
	  constraint[ graph::as<phys::constraint::bilateral>(v, t) ] = i;
	  break;
	}
      }      
      
    }
    
    

    
    struct fill_cache : dfs_visitor<> {

      linear_time::cache_type& res;
      
      fill_cache(linear_time::cache_type& res) : res(res) { }

      // postfix
      void finish_vertex(tree::constraints::vertex_descriptor v, const tree::constraints& ) {
	res.postfix.push_back(v);
      }

      // prefix
      void discover_vertex(tree::constraints::vertex_descriptor v,
			   const tree::constraints& ) const {
	res.prefix.push_back(v);
      }
    };


    // traversal cache
    static linear_time::cache_type make_cache(const tree::constraints& g) {
      linear_time::cache_type res;

      res.prefix.reserve( num_vertices(g) );
      res.postfix.reserve( num_vertices(g) );

      depth_first_search( g, visitor( fill_cache(res) ) );
      return res;
    }
    
    void linear_time::solve(const task& ) const {
      throw phys::error("unimplemented");
    }

    void linear_time::update() {
      solver::update(tree, id.dof, id.constraint, pack.matrix);
      factor(tree, *this);
    }

    
    linear_time::linear_time(const dof::mass& masses,
			     tree::constraints& t,
			     const phys::constraint::bilateral::pack& pack) 
      :  masses(masses),
	 tree(t),
	 pack(pack),
	 cache(make_cache(tree))
    {
      // warning: O(m.log(m) + n.log(n) ) 
      fill( id.dof, id.constraint, tree );
     
      update();
    }
    
    struct get_results {
      tree::constraints& t;
      get_results(tree::constraints& t) : t(t) { }

      math::vec&& operator()(math::natural i) const {
	tree::vertex& n = t[ boost::vertex(i, t) ];
	
	return std::move(n.solution);
      }
      

    };


    dof::velocity linear_time::operator()(const dof::momentum& f,
					    const result& r) const {
      
      solve_first(tree,  *this, f, pack.values );
      solve_second(tree, *this);
      
      dof::velocity res;
      sparse::map(res, id.dof, get_results(tree) );
      
      if(r.lambda) {
	constraint::bilateral::vector lambda;
	sparse::map(lambda, id.constraint, get_results(tree) );
	
	r.update_lambda( lambda );
      }
      
      return res;
    }

    


  }
}
