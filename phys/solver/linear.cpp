#include <Eigen/Cholesky>

#include <sparse/matrix.h>
#include <sparse/weight.h>

#include "linear.h"

#include <phys/system.h>
#include <core/stl.h>

#include <boost/graph/undirected_dfs.hpp>
#include <boost/graph/kruskal_min_spanning_tree.hpp>

#include <phys/error.h>

#include <Eigen/LU>
#include <math/vec.h>

#include <core/log.h>
#include <phys/solver/task.h>

#include <sparse/insert.h>
#include <sparse/update.h>

#include <core/bind.h>
#include <Eigen/Cholesky>

#include <core/macro/here.h>
#include <core/macro/derp.h>

#include <core/link.h>

#include <queue>

#include <boost/graph/graphviz.hpp>
#include <core/io.h>

#include <script/meuh.h>
#include <math/random.h>

#include <core/shuffle.h>



namespace phys {
  namespace solver {

    using namespace boost;



    struct vertex_writer {
      const linear::graph_type& graph;
      
      void operator()(std::ostream& out, linear::graph_type::vertex_descriptor v) const {
	out << "[label=\"" << (graph[v].type == linear::vertex::dof ? "D" : "C") << core::string(v) << "\"]";
      }
    };
 

    linear::vertex::vertex() 
      : type(dof),
	key(0),
	off(-1) {

    }
	



    void linear::clear() {
      
      graph.clear();
      tree.clear();
      forward.clear();

      dofs.clear();
      constraints.clear();

      acyclic = 0;

      dim = 0;
    }


    // void linear::add(constraint::bilateral::key k,
    // 		     const constraint::bilateral::row& row) {
    //   // assert( k->id == -1 );
      
    //   graph_type::vertex_descriptor v = add_vertex(graph);
      
    //   graph[v].type = vertex::constraint;
    //   graph[v].key = k;
    //   k->id = v;
      
    //   graph[v].D = math::mat::Zero(k->dim(), k->dim());
      
    //   core::each(row, [&](dof::key d, const math::mat& J) {
    // 	  assert(d->id != -1);

    // 	  auto e = add_edge(v, d->id, graph);
    // 	  assert(e.second);

    // 	  graph[e.first].J = J;
    // 	});
      
    //   constraints.push_back(k);
    // }
    
    linear::graph_type::vertex_descriptor linear::id(dof::key d) const {
      auto it = dofs.find( d );
      if( it == dofs.end() ) throw error("unknown dof");
      return it->second;
    }

    linear::graph_type::vertex_descriptor linear::id(constraint::bilateral::key d) const {
      auto it = constraints.find( d );
      if( it == constraints.end() ) throw error("unknown constraint");
      return it->second;
    }
    
    
    void linear::add(const constraint::bilateral::matrix& matrix) {
      matrix.each([&](constraint::bilateral::key c,
		      dof::key d,
		      sparse::const_mat_chunk J) {
		    
		    const bool new_constraint = constraints.find(c) == constraints.end();
		    
		    if( new_constraint ) {
		      graph_type::vertex_descriptor v = add_vertex(graph);
		    
		      graph[ v ].type = vertex::constraint;
		      graph[ v ].key = c;
		      
		      graph[ v ].D.setZero(c->dim(), c->dim());  
		      graph[ v ].D.diagonal().array() -= damping;
		      
		      constraints[c] = v; 
		      
		      //	      core::log("new constraint");
		    }
		    
		    auto e = add_edge( id(c), id(d), graph);
		    assert( e.second );
		    
		    graph[ e.first ].J = J; 
		  });
      
      //  core::log("edges:", boost::num_edges(graph));
      
      
    }

    
   


    linear::linear() 
      : dim(0), damping(0)
    {
      
    }

    // static math::natural id(const linear::vertex& v) {
    //   switch( v.type ) {
    //   case linear::vertex::dof: return id( v.as<phys::dof>() );
    //   case linear::vertex::constraint: return v.as<phys::constraint::bilateral>()->id;
    //   default:
    // 	throw phys::error("derp" + macro_here);
    //   }

    // }

    void linear::init_dofs(const dof::mass& mass) {
      
      graph = graph_type();
      
      mass.each([&](dof::key d, dof::key x, const math::mat& M) {
	  assert(d == x);  core::use(x);

	  graph_type::vertex_descriptor v = add_vertex(graph);
	  
	  graph[ v ].type = vertex::dof;
	  graph[ v ].key = d;
	  dofs[ d ] = v;
	  
	  graph[ v ].D = M;
	  graph[ v ].D_inv = vertex::inv_type( graph[v].D );
	  
	  assert(graph[v].D.squaredNorm() > 0);
	});
      
    }
    
    
    linear::edge_vector linear::rnd()  {
      edge_vector res;
      
      
      typedef color_traits<default_color_type> traits;
      const auto ec = get(edge_color, graph);
      const auto vc = get(vertex_color, graph);
      
      using namespace math;
      natural s = math::random<natural>::uniform(0, num_vertices(graph) - 1)();

      core::each(vertices(graph), [&](natural v) {
	  vc[v] = traits::white();
	});

      core::each(edges(graph), [&](graph_type::edge_descriptor e) {
	  ec[e] = traits::white();
	});
      
      std::function< void (natural) > dfs = [&](natural u) {
	
	vc[u] = traits::gray();
	
	std::vector<graph_type::edge_descriptor> edges;


	core::each(out_edges(u, graph), [&](graph_type::edge_descriptor e) {
	    edges.push_back(e);
	  });

	core::shuffle(edges);

	core::each( edges, [&](graph_type::edge_descriptor e) {
	    
	    auto old = ec[e];
	    natural v = target(e, graph);
	    
	    ec[e] = traits::black();
	    
	    if( vc[v] == traits::white() ) {
	      // (u, v) is a tree edge
	      res.push_back( boost::edge(u, v, graph).first );
	      dfs( v );
	    } else if( (vc[v] == traits::gray()) && (old == traits::white()) ){
	      // (u, v) is a back edge
	      
	    }

	  });

	vc[u] = traits::black();

      };
      
      dfs( s );
      return res;
    }

    
    linear::edge_vector linear::dfs(math::natural start)  {
      edge_vector res;
      
      struct get_edges : dfs_visitor<> {
      	edge_vector& res;
      	get_edges(edge_vector& res) : res(res) { }
	
      	void tree_edge(graph_type::edge_descriptor e, const graph_type& ) const {
	  res.push_back(e);
      	}
	
      };

      undirected_dfs(graph, 
		     visitor( get_edges(res) )
		     .edge_color_map( get(edge_color, graph) )
		     .root_vertex( start ) );
      
      return res;
    }
    
    linear::edge_vector linear::mst(const constraint::bilateral::weight& weight) {
      
      weight.each( [&](constraint::bilateral::key c, math::real w) {
	  if( constraints.find(c) == constraints.end() ) return;

	  assert( graph[ constraints[c] ].type == vertex::constraint);
	  
	  core::each( out_edges( id(c), graph), [&](graph_type::edge_descriptor e) {
	      graph[ e ].weight = w;
	    });
	  core::each( in_edges(id(c), graph), [&](graph_type::edge_descriptor e) {
	      graph[ e ].weight = w;
	    });
	  
	});
      
    
      edge_vector spanning;
      kruskal_minimum_spanning_tree(graph, std::back_inserter(spanning),
				    weight_map(get(&edge::weight, graph)));
      return spanning;
    }



    
    // 
    linear::isolated_type linear::span(const edge_vector& spanning) {
      
      // temporary graph with spanning edges
      core::graph<> g( num_vertices(graph) );

      // build temp tree
      core::each(spanning, [&](const graph_type::edge_descriptor& e) {
	  auto ge = add_edge(source(e, graph), target(e, graph), g);
	  assert( ge.second ); core::use(ge);
	});
      
      // cleanup bad constraint vertices in g, mark bad vertices in graph
      isolated_type isolated( constraints.size() );
      
      core::each(vertices(graph), [&](graph_type::vertex_descriptor v) {
	  if( graph[v].type != vertex::constraint ) return;
	  
	  const math::natural old_degree = out_degree(v, graph);
	  const math::natural new_degree = out_degree(v, g);
	  
	  const bool admissible = (old_degree == new_degree) && (new_degree == 2);

	  if( admissible ) return;
	  	  
	  clear_vertex(v, g);
	  auto c = graph[v].as<constraint::bilateral>();
	  constraints.erase(c);
	  
	  isolated.insert( c );
	});

      // at this point we have the right set of dofs/constraints
      auto copy = constraints;
      acyclic = [copy](constraint::bilateral::key c) {
	auto res = copy.find(c) != copy.end();
	return res;
      };
      


      // // debug spanning tree
      // std::string filename("/tmp/tree.viz");
      // core::io::write(filename, [&](std::ostream& out) {
      // 	  write_graphviz(out, g, vertex_writer{graph} );
      // 	});
      // script::require("graph");
      // script::exec("graph.view('%%')", filename); 



      // label with an offset
      number();
      
      // parcourir g pour obtenir forward et creer l'arbre
      struct prefix : dfs_visitor<> {
	
	linear& solver;
	
	prefix(linear& solver) : solver(solver) { }

	void discover_vertex(core::graph<>::vertex_descriptor v, const core::graph<>& ) const {
	  switch( solver.graph[v].type ) {
	    
	  case vertex::dof: 
	    
	    if( solver.dofs.find(solver.graph[v].as<dof>() ) != solver.dofs.end() ) {
	      solver.forward.push_back( v );
	    }
	   
	    break;
	  case vertex::constraint:
	    if( solver.constraints.find(solver.graph[v].as<constraint::bilateral>() ) != solver.constraints.end() ) {
	      solver.forward.push_back( v );
	    }
	    break;
	  }
	}
	
	void tree_edge(core::graph<>::edge_descriptor e, const core::graph<>& g) const {
	  auto s = source(e, g);
	  auto t = target(e, g);
	  auto te = add_edge(s, t, solver.tree);

	  auto ge = boost::edge(s, t, solver.graph);
	  assert( ge.second);
	  solver.tree[te.first] = solver.graph[ge.first].J;
	  
	  assert( te.second );
	}

	void back_edge(core::graph<>::edge_descriptor , const core::graph<>& ) const {
	  assert( false );
	}

      };
      
      tree = tree_type( num_vertices(graph) );
      
      auto start = boost::source(spanning.front(), graph);

      // random start vertex ?
      start = math::random<math::natural>::uniform(0, num_vertices(g) - 1)();
      
      undirected_dfs(g, visitor( prefix(*this)).edge_color_map(get(edge_color, g)).root_vertex(start));
      
      return isolated;

    }

    
    template<class F>
    static void prefix(const linear& solver, const F& f) {
      core::each(solver.forward, f);
    }


    template<class F>
    static void postfix(const linear& solver, const F& f) {
      for(auto it = solver.forward.rbegin(), end = solver.forward.rend(); 
	  it != end;
	  ++it) { f(*it); } 
    }
    

 
    void linear::factor() {
      assert( num_vertices(tree) );

      // init J matrices
      postfix(*this, [&](graph_type::vertex_descriptor v) {
	  tree_type::in_edge_iterator p, ep;
	  tie(p, ep) = in_edges(v, tree); // parent edge

	  if(p == ep) return;	// root
	  
	  switch(graph[v].type) {
	  case vertex::dof :
	    graph[v].J = tree[*p].transpose();
	    break;
	  case vertex::constraint :
	    graph[v].J = tree[*p];
	    break;
	  default: assert(false);
	  }
	  
	  assert( !graph[v].J.empty() );
	});
  
      
      // compute D/Dinv matrices
      postfix(*this, [&](graph_type::vertex_descriptor v) {
	  vertex& n = graph[v];
	  
	  // for children
	  tree_type::adjacency_iterator c, ec;
	  for( tie(c, ec) = adjacent_vertices(v, tree); c != ec; ++c) {
	    auto& child = graph[*c];

	    assert( !child.J.empty() );

	    n.D.noalias() -= child.J.transpose() * child.D * child.J; 
	  }
	  
	  // inversion
	  n.D_inv = vertex::inv_type( n.D );
	  
	  tree_type::in_edge_iterator f, ef;
	  tie(f, ef) = in_edges(v, tree);
	  
	  // if not root
	  if( f != ef) {
	    math::mat tmp = n.J;
	    
	    n.J = n.inv( tmp );
	    if( n.J.empty() ) n.J.setZero(tmp.rows(), tmp.cols());
	    assert( !n.J.empty() );
	  }
	});
      
      
    }

    
    static auto make_chunk = [](math::vec& d,
				const linear::graph_type& graph,
				const linear::graph_type::vertex_descriptor& v) {
      assert( graph[v].off != -1 );
      return d.segment(graph[v].off, graph[v].dim());
    };
    
    

    void linear::fill(const dof::momentum& f, const constraint::bilateral::vector& b) const {
      // access solution chunk given vertex_descriptor
      auto chunk = core::bind( make_chunk )(data.get(), graph);
      
      data.get().setZero( dim );
      
      // fill solution chunks
      f.each([&](dof::key d, sparse::const_vec_chunk f) {
	  chunk( id(d) ).noalias() = f;
	});
       
      b.each([&](constraint::bilateral::key c, sparse::const_vec_chunk b) {
	
	  // only fills managed constraints
	  auto it = constraints.find(c);
	  if( it != constraints.end() ) chunk( it->second ).noalias() = b;
	  
	});
    }


    void linear::solve(const solver::task& task) const  {
      
      // fill solution chunks
      fill( task.momentum, task.bilateral );
      
      // access solution chunk given vertex_descriptor
      auto chunk = core::bind( make_chunk )(data.get(), graph);

      
      // solve first
      postfix(*this, [&](graph_type::vertex_descriptor v) {

	  // on children
	  tree_type::adjacency_iterator c, ec;
	  for( tie(c, ec) = adjacent_vertices(v, tree); c != ec; ++c) {
	    chunk(v).noalias() -= graph[*c].J.transpose() * chunk(*c);
	  }
	  
	});
      

      // solve second
      math::vec alloc;
      prefix(*this, [&](graph_type::vertex_descriptor v) {
	  math::natural d = chunk(v).rows();
	  if(alloc.rows() < int(d)) alloc.resize(d);
	  
	  alloc.head(d) = graph[v].D_inv.solve( chunk(v) );
	  chunk(v).noalias() = alloc.head(d);
	});
	
      prefix(*this, [&](graph_type::vertex_descriptor v) {
	  tree_type::inv_adjacency_iterator p, ep;
	  tie(p, ep) = inv_adjacent_vertices(v, tree); 
	  
	  // not root
	  if( p != ep ) chunk(v).noalias() -= graph[v].J * chunk(*p);
	  
	});

      auto& res = task.velocity;
     
      core::each( core::all(dofs), [&](dof::key d, math::natural id) {
	  res(d).noalias() = chunk( id );
	});

      if( task.lambda ) {

	core::each( core::all(constraints), [&](constraint::bilateral::key c, math::natural id) {
	    (*task.lambda)(c).noalias() = chunk( id );
	  });
	
      }
      
    
    };


    math::natural linear::vertex::dim() const {
      switch(type) {
      case vertex::constraint: return as<phys::constraint::bilateral>()->dim();
      case vertex::dof: return as<phys::dof>()->dim();
      default: throw phys::error( DERP );	 
      }
    }  

    void linear::init(const phys::system& system) {
      clear();
      init_dofs( system.mass );
      add( system.constraint.bilateral.matrix );
    }

    linear::isolated_type linear::setup(const edge_vector& e) {
     
      auto res = e.empty() ? span(dfs()) : span( e ); 
      factor();
      return res;
    }


    linear::~linear() { 
      // clear();
    }
   

    
    math::mat linear::vertex::inv(const math::mat& x) const {
      assert(x.rows() == int( dim() ) );
      math::mat res = math::mat::Zero(dim(), x.cols());
						
      res.each_col([&](math::natural j) {
	  res.col(j) = D_inv.solve(x.col(j));
	});
      
      return res;
    }
    

    
    void linear::number() {
      dim = 0;
      
      // reset everyone to -1
      core::each( vertices(graph), [&](graph_type::vertex_descriptor v) {
	  graph[v].off = -1;
	});

      core::each( core::all(dofs), [&](dof::key d, math::natural id) {
	  assert( graph[ id ].key == d);
	  
	  graph[ id ].off = dim;
	  dim += d->dim();
	});
      
      core::each( core::all(constraints), [&](constraint::bilateral::key c, math::natural id) {
	  assert( graph[ id ].key == c);
	  
	  graph[ id ].off = dim;
	  dim += c->dim();
	});

    }
    



    constraint::bilateral::weight linear::propagation_weights(const task& t) {
      number();

      fill(t.momentum, t.bilateral);
      auto chunk = core::bind( make_chunk )(data.get(), graph);

      typedef color_traits<default_color_type> traits;
      auto color = get(vertex_color, graph);
      
      // setup dof responses
      core::each(core::all(dofs), [&](dof::key , math::natural id) {
	  graph[ id ].D_inv = vertex::inv_type(graph[ id ].D);
	});
      
      auto error = [&](int vc) {
	assert( graph[vc].type == vertex::constraint);
	return chunk(vc).squaredNorm();
      };
      
      auto weight = [&](int vc) { return -error(vc); };
      
      
      typedef std::multimap<math::real, int> sorted_type;
      sorted_type sorted;
      
      std::map< int, sorted_type::iterator > iter; 

      // 1 compute constraint errors to pick start vertex
      auto init_sorted = [&] {
	
	assert( !constraints.empty() );
	core::each( core::all(constraints), [&](constraint::bilateral::key , math::natural vc) {
	    color( vc ) = traits::white();

	    core::each( out_edges(vc, graph), [&](graph_type::edge_descriptor e) {
		auto vd = target(e, graph);
		chunk(vc) -= graph[e].J * graph[vd].D_inv.solve( chunk(vd) );
		
	      });
	    
	    auto it = sorted.insert( std::make_pair( weight(vc), vc ) );
	    iter[vc] = it;
	  });
	
      };
      
      
      // solve for lambdas based on current error and dof momentum
      auto solve = [&](graph_type::vertex_descriptor vc) {
	assert( graph[vc].type == vertex::constraint );
	
	math::natural d = graph[vc].dim();
	
	// solve constraint
	math::mat K = math::mat::Zero(d, d);
	
	core::each( out_edges(vc, graph), [&](graph_type::edge_descriptor e) {
	    auto vd = target(e, graph);
	    K += graph[e].J * graph[ vd ].D_inv.solve( graph[e].J.transpose() );
	  });
	
	math::vec lambda = K.ldlt().solve( chunk(vc) );
	return lambda;
      };
      

      // propagate impulses to dof children, then to unmarked
      // constraint grand children
      auto propagate = [&](graph_type::vertex_descriptor vc, 
			   const math::vec& lambda) {
	assert( graph[vc].type == vertex::constraint );

	core::each( out_edges(vc, graph), [&](graph_type::edge_descriptor e) {
	    auto vd = target(e, graph);
	    
	    math::vec impulse = graph[e].J.transpose() * lambda;
	    chunk(vd) += impulse;

	    core::each( out_edges(vd, graph), [&](graph_type::edge_descriptor e) {
		auto vc = target(e, graph);
		if( color(vc) == traits::white() ) {
		  assert( graph[vc].type == vertex::constraint);
		  
		  chunk(vc) -= graph[e].J * graph[vd].D_inv.solve( impulse );

		  sorted.erase( iter[vc] );

		  // TODO use hint
		  auto it = sorted.insert( std::make_pair(weight(vc), vc) );
		  iter[vc] = it;
		}
	      });
	  });
	
      };
      
      constraint::bilateral::weight res;

      init_sorted();

      while( !sorted.empty() ) {
	auto front = sorted.begin();
	color( front->second ) = traits::black();
	
	auto lambda = solve( front->second );

	auto c = graph[front->second].as<constraint::bilateral>();
	// res(c) = front->first;
	res(c) = -lambda.squaredNorm();
	
	propagate(front->second, lambda);
	
	sorted.erase( front );
	iter.erase( front->second );
      };

      return res; 
    }



    linear::edge_vector linear::propagation(const task& t) {
      return mst( propagation_weights(t));
    }

 



    linear::edge_vector linear::dfs_weight(const constraint::bilateral::weight& w) {
      
      edge_vector res;

      typedef color_traits<default_color_type> traits;
      const auto& ec = get(edge_color, graph);
      const auto& vc = get(vertex_color, graph);

      math::real min = 0;
      graph_type::vertex_descriptor argmin = 0;
      
      // 1 setup weights
      w.each([&](constraint::bilateral::key c, math::real wc) {
	  core::each( out_edges( id(c), graph), [&](graph_type::edge_descriptor e) {
	      graph[ e ].weight = wc;
	      ec[ e ] = traits::white();
	    });
	  
	  if(wc < min) { 
	    min = wc;
	    argmin = id(c);
	  }

	});

      core::each(vertices(graph), [&](graph_type::vertex_descriptor v) {
	  vc[ v ] = traits::white();
	});
      
      // 
      std::function<void(graph_type::vertex_descriptor v)> fun = [&](graph_type::vertex_descriptor v) {
	
	std::multimap<math::real, graph_type::edge_descriptor> map;

	vc[ v ] = traits::gray();

	// set weights
	core::each( out_edges(v, graph), [&](graph_type::edge_descriptor e) {
	    map.insert( std::make_pair(graph[e].weight, e));
	  });
	
	while( !map.empty() ) {

	  // pop front
	  auto e = map.begin()->second;
	  map.erase(map.begin());
	  
	  if(ec[e]  != traits::black() ) {
	    ec[e] = traits::black();
	
	    auto t = target(e, graph);

	    if( vc[t] == traits::white() ) {
	      res.push_back(e);
	      fun(t);
	    }
	  }
	  
	}
	
	vc[ v ] = traits::black();
	
      };

      // start with the constraint with lowest weight
      fun( argmin );
      
      return res;
    }




    

    linear::edge_vector linear::bfs_prop(const solver::task& t) {

      // std::string filename("/tmp/graph.viz");
      // core::io::write(filename, [&](std::ostream& f) {
      // 	  debug(f);
      // 	});
      
      // script::require("graph");
      // script::exec("graph.view('%%')", filename); 
      
      number();
      
      fill(t.momentum, t.bilateral);
      auto chunk = core::bind( make_chunk )(data.get(), graph);

      typedef color_traits<default_color_type> traits;
      auto color = get(vertex_color, graph);
      
      auto weight = [&](graph_type::vertex_descriptor vc) -> double{
	assert( graph[vc].type == vertex::constraint);
	return chunk(vc).squaredNorm();
      };

  

      
      // 1 compute constraint errors to pick start vertex
      auto start_vertex = [&] {

	math::real min = 0;
	assert( !constraints.empty() );
	graph_type::vertex_descriptor start = constraints.begin()->second;
	
	core::each( core::all(constraints), [&](constraint::bilateral::key , math::natural vc) {
	    
	    core::each( out_edges(vc, graph), [&](graph_type::edge_descriptor e) {
		auto vd = target(e, graph);
		chunk(vc) -= graph[e].J * graph[vd].inv( chunk(vd) );
		
	      });
	    
	    math::real err2 = weight(vc);
	    if( -err2 < min ) {
	      min = -err2;
	      start = vc;
	    }
	    
	  });
	return start;
      };



      // solve for lambdas based on current error 
      auto solve = [&](graph_type::vertex_descriptor vc) -> math::vec {
	assert( graph[vc].type == vertex::constraint );
	
	// core::log("solving", vc);

	math::natural d = graph[vc].dim();
	assert( d );
	
	math::mat K = math::mat::Zero(d, d);
	
	core::each( out_edges(vc, graph), [&](graph_type::edge_descriptor e) {
	    auto vd = target(e, graph);
	    K += graph[e].J * graph[ vd ].inv( graph[e].J.transpose() );
	  });
	
	return vertex::inv_type(K).solve( chunk(vc) );
      };
      
      
      // update white errors from a dof given constraint force
      auto update_errors = [&](graph_type::vertex_descriptor vd, const math::vec& impulse) {
	assert( graph[vd].type == vertex::dof);
	
	double min = 1e42;

	core::each( out_edges(vd, graph), [&](graph_type::edge_descriptor e) {
	    auto vc = target(e, graph);
	    assert( graph[vc].type == vertex::constraint );

	    if( color[vc] == traits::black() ) return;
	    
	    chunk(vc) -= graph[e].J * graph[vd].inv( impulse );
	    min = std::min(min, -weight(vc) );
	  });
	
	return min;
      };

      
      auto update_impulses = [&](graph_type::vertex_descriptor vc, const math::vec& lambda) {
	assert( graph[vc].type == vertex::constraint);
	

	std::multimap<double, int> sorted;
	
	core::each( out_edges(vc, graph), [&](graph_type::edge_descriptor e) {
	    auto vd = target(e, graph);
	    assert( graph[vd].type == vertex::dof);
	    
	    if( color[vd] == traits::black() ) return;

	    math::vec impulse = graph[e].J.transpose() * lambda;

	    chunk(vd) += impulse;

	    double w = update_errors(vd, impulse);
	    
	    if(color[vd] == traits::white() ) 
	      sorted.insert( std::make_pair(w, vd) );
	  });

	return sorted;
      };

      auto update = [&](graph_type::vertex_descriptor vc) {
	assert( graph[vc].type == vertex::constraint);

	math::vec lambda = solve(vc);
	return update_impulses(vc, lambda);
      };
      

      std::queue<int> queue;
      edge_vector res;

      std::function<void (int) > bfs_constraint;
      std::function<void (int) > bfs_dof;
      
      bfs_constraint = [&](int vc) {
	assert( graph[vc].type == vertex::constraint );
	
	
	auto sorted = update( vc );

	core::each( sorted, [&](double, graph_type::vertex_descriptor vd) {
	    assert( color[vd] == traits::white() );
	    
	    
	    color[vd] = traits::gray();
	    queue.push( vd );

	    // core::log("pushing", vc, vd);
	    res.push_back( boost::edge(vc, vd, graph).first );
	  });
	color[vc] = traits::black();
      };

      bfs_dof = [&](int vd) {
	assert( graph[vd].type == vertex::dof );


	std::multimap<double, int> sorted;
	
	core::each( out_edges(vd, graph), [&](graph_type::edge_descriptor e) {
	    auto vc = target(e, graph);
	    if(color[vc] != traits::white() ) return;
	    
	    sorted.insert(std::make_pair(-weight(vc), vc));
	  });
	
	core::each( sorted, [&](double, int vc) {
	    color[vc] = traits::gray();
	    queue.push( vc );
	  
	    // core::log("pushing", vd, vc);
	    res.push_back( boost::edge(vd, vc, graph).first );
	  });
	color[vd] = traits::black();
      };


      auto bfs = [&] {

	while( !queue.empty( )) {
	  auto v = queue.front();
	  queue.pop();

	  switch(graph[v].type) {
	  case vertex::dof: bfs_dof(v); break;
	  case vertex::constraint: bfs_constraint(v); break;
	  };
	  
	}

      };



      // set everyone white
      core::each(boost::vertices(graph), [&](int v) {
	  color[v] = traits::white();
	});
      
      queue.push( start_vertex() );
      bfs();


      return res;
    }
    



    


    linear::edge_vector linear::bfs_weight(const constraint::bilateral::weight& w) {
      
      edge_vector res;

      typedef color_traits<default_color_type> traits;

      const auto& ec = get(edge_color, graph);
      const auto& vc = get(vertex_color, graph);

      math::real min = 1e42;
      graph_type::vertex_descriptor start = 0;
      
      // 1 setup weights
      w.each([&](constraint::bilateral::key c, math::real wc) {
	  core::each( out_edges( id(c), graph), [&](graph_type::edge_descriptor e) {
	      graph[e].weight = wc;
	      ec[e] = traits::white();
	    });
	  
	  if(wc < min) { 
	    min = wc;
	    start = id(c);
	  }

	});

      core::each(vertices(graph), [&](graph_type::vertex_descriptor v) {
	  vc[ v ] = traits::white();
	});
      

      std::list< graph_type::edge_descriptor > queue;

      auto enqueue = [&](graph_type::vertex_descriptor v) {

	// sorted out-edges
	std::multimap<math::real, graph_type::edge_descriptor> sorted;

	core::each(out_edges(v, graph), [&](graph_type::edge_descriptor e) {
	    sorted.insert( std::make_pair(graph[e].weight, e) );
	  });
	
	core::each(sorted, [&]( math::real, graph_type::edge_descriptor e) {
	    if( vc( target(e, graph) ) == traits::white() ) {
	      // no cycle
	      vc( target(e, graph) ) = traits::black();
	      queue.push_back( e );
	    }
	  });
	
      };
      
      enqueue( start );

      while(!queue.empty() ) {
	auto f = queue.front();

	queue.pop_front();
	enqueue( target(f, graph) );	

	res.push_back(f);
      }
      
      
      return res;
    }



    void linear::debug(std::ostream& out) const {
      write_graphviz(out, graph, vertex_writer{graph} );
    }
    
  }
}
 
