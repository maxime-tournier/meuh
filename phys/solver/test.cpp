#include "test.h"


#include <core/macro/derp.h>
#include <phys/error.h>

#include <phys/solver/task.h>
#include <core/dfs.h>

#include <boost/graph/undirected_dfs.hpp>
#include <boost/graph/kruskal_min_spanning_tree.hpp>

// cuthill mckee starting node
#include <boost/graph/detail/sparse_ordering.hpp>

#include <phys/system.h>

#include <sparse/matrix.h>
#include <sparse/diag.h>
#include <sparse/weight.h>

#include <Eigen/LU>
#include <math/matrix.h>

#include <core/const_vector.h>

#include <boost/pending/disjoint_sets.hpp>
#include <core/link.h>

#include <math/random.h>
#include <core/shuffle.h>

namespace phys {
  namespace solver {

    namespace test {

      using namespace boost;

      edge_type::edge_type() : weight( 0 ) {}
      vertex_type::vertex_type() : key( 0 ) { }


      graph_type graph(const system& sys ) {
	
	std::unordered_map<constraint::bilateral::key, int> constraints( sys.constraint.bilateral.matrix.rows().size() );
	std::unordered_map<dof::key, int> dofs( sys.mass.rows().size() );
	
	
	graph_type res;
	
	// dof vertices
	::sparse::diag(sys.mass).each([&](dof::key d, core::unused) {
	    auto v = boost::add_vertex( res );
	    
	    res[v].type = vertex_type::dof;
	    res[v].key = d;
		  
	    auto ret = dofs.insert( std::make_pair(d, v) ); 
	    assert( ret.second ); core::use(ret);
	  });
	
	// constraint vertices/edges
	sys.constraint.bilateral.matrix.each( [&](constraint::bilateral::key c,
						  dof::key d,
						  core::unused ) {
						
						auto itc = constraints.find( c );
	    
						if( itc == constraints.end() ) { // insert vertex if needed
						  auto v = boost::add_vertex( res );
	      
						  res[v].type = vertex_type::constraint;
						  res[v].key = c;
						  
						  auto ret = constraints.insert( std::make_pair(c, v) );
						  assert( ret.second );
	      
						  itc = ret.first;
						}
		
						auto itd = dofs.find( d );
						assert( itd != dofs.end() );
	    
						// insert edge
						auto ret = boost::add_edge(itc->second, itd->second, res); core::use(ret);
						assert( ret.second );
					      });
	
	return res;
      };
    
    
      
      edge_set dfs(graph_type& g, graph_type::vertex_descriptor start) {
	
	using namespace boost;
	
	struct vis : dfs_visitor<> { 
	
	  edge_set& res;
	  vis( edge_set& res ) : res( res )  { }

	  void tree_edge(graph_type::edge_descriptor e, const graph_type& ) {
	    res.push_back( e );
	  };
	
	};

	edge_set res;
	undirected_dfs(g, visitor( vis(res) ).edge_color_map( get( edge_color, g ) ).root_vertex( start ));
	
	return res;
      }
      

      edge_set mst(graph_type& graph, const constraint::bilateral::weight& weight) {

	core::each(vertices( graph ), [&](int v) {

	    if( graph[v].type == vertex_type::dof ) return;
	    
	    auto c = constraint::bilateral::key( graph[v].key );
	    
	    core::each( out_edges( v, graph), [&](graph_type::edge_descriptor e) {
		graph[ e ].weight = weight(c);
	      });
	    core::each( in_edges(v, graph), [&](graph_type::edge_descriptor e) {
		graph[ e ].weight = weight(c);
	      });
	  });
	
	edge_set spanning;
	kruskal_minimum_spanning_tree(graph, std::back_inserter(spanning),
				      weight_map(get(&edge_type::weight, graph)));
	return spanning;
      }


      edge_set rnd(graph_type& graph) {
	edge_set res;
	
	res.reserve( num_vertices(graph) );
	
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
	
	std::function< void (natural) > go = [&](natural u) {
	  
	  vc[u] = traits::gray();
	
	  std::vector<graph_type::edge_descriptor> edges;
	  edges.reserve( out_degree(u, graph) );
	  
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
		go( v );
	      } else if( (vc[v] == traits::gray()) && (old == traits::white()) ){
		// (u, v) is a back edge
	      }

	    });
	  
	  vc[u] = traits::black();
	  
	};
	
	go( s );
	return res;
	
      }



      span_type span(const graph_type& g, const edge_set& edges) {
	using namespace boost;

	span_type res;

	res.tree = tree_type( num_vertices(g) );
	core::each( vertices(g), [&](int v) {
	    res.tree[v] = g[v];
	  });


	typedef core::graph< undirectedS > tmp_type;
	tmp_type tmp( num_vertices(g) );

	// duplicate edges
	core::each( edges, [&](graph_type::edge_descriptor e) {
	    auto s = source(e, g);
	    auto t = target(e, g);
	    
	    add_edge( s, t, tmp );
	  });
      
	// TODO finer alloc
	res.cycles.rehash( num_vertices(g) - edges.size() );
	res.postfix.reserve( edges.size() );

	// cleanup bad constraints
	core::each( vertices( tmp ), [&]( int v ) {
	    if( g[v].type == vertex_type::constraint ) {

	      // is constraint broken by spanning tree ? :'(
	      const bool broken = degree(v, tmp) != degree(v, g);
	      
	      const bool remove = broken || (degree(v, tmp) != 2);
	      
	      if( remove ) {
		

		clear_vertex( v, tmp );
		res.cycles.insert( constraint::bilateral::key( g[v].key ) );

	      }
	    }
	  });

      
	// build tree
	struct vis : dfs_visitor<> { 
	  span_type& span;

	  vis( span_type& span ) : span(span) { }

	  void finish_vertex(tmp_type::vertex_descriptor v, const tmp_type& ) const {
	  
	    if( span.cycles.find( constraint::bilateral::key( span.tree[v].key ) ) == span.cycles.end() ) {
	      span.postfix.push_back( v );
	    }
	  
	  }
	
	  void tree_edge(tmp_type::edge_descriptor e, const tmp_type& g) const {
	    auto s = source(e, g);
	    auto t = target(e, g);
	    
	    auto ret = add_edge(s, t, span.tree);
	    assert( ret.second ); core::use(ret);
	  }

	  void back_edge( tmp_type::edge_descriptor , const tmp_type& ) const {
	    assert( false );
	  }
	
	};
	
	
	graph_type::vertex_descriptor start = edges.empty() ? 0 : source( *edges.begin(), g);
	
	undirected_dfs(tmp, visitor( vis(res) ).edge_color_map( get(edge_color, tmp) )
		       .root_vertex(start) );
	
	return res;
      }


      ::sparse::mat_chunk data_type::Di(math::natural i) {
	auto k = span.tree[i].key;

	switch( span.tree[i].type ) {
	case vertex_type::dof: return resp_dof( dof::key(k), dof::key(k) );
	case vertex_type::constraint: return resp_constraint( constraint::bilateral::key(k),
							      constraint::bilateral::key(k)
							      );
	};
      
	throw error( DERP );
      }
    
    
      ::sparse::const_mat_chunk data_type::Di(math::natural i) const {
	auto k = span.tree[i].key;

	switch( span.tree[i].type ) {
	case vertex_type::dof: return resp_dof( dof::key(k), dof::key(k) );
	case vertex_type::constraint: return resp_constraint( constraint::bilateral::key(k),
							      constraint::bilateral::key(k)
							      );
	};
      
	throw error( DERP );
      }
    

      
      template<class Matrix>
      typename ::sparse::chunk< Matrix >::type 

      static matrix_chunk(const tree_type& tree, 
			  Matrix& matrix,
			  int i) {
	auto p = boost::inv_adjacent_vertices(i, tree);
	if( p.first == p.second ) throw error("root should not have its J accessed" );
	
	switch( tree[i].type ) {
	case vertex_type::dof: {

	  auto d = dof::key( tree[i].key );
	  auto c = constraint::bilateral::key( tree[ *p.first ].key );
	  auto chunk = matrix(c, d);

	  // transpose
	  return { chunk.data(), chunk.cols(), chunk.rows() };

	} break;
	case vertex_type::constraint: {
	  auto d = dof::key( tree[*p.first].key );
	  auto c = constraint::bilateral::key( tree[i].key );
	
	  auto chunk = matrix(c, d);
	
	  return chunk;
	} break;
	}
	
	throw error( DERP );
	
      }
	
    
			       

    
    
      ::sparse::mat_chunk data_type::J(math::natural i) {

	return matrix_chunk( span.tree, matrix, i); 
    
      }


      ::sparse::const_mat_chunk data_type::J(math::natural i) const {
	return matrix_chunk( span.tree, matrix, i); 
      }
 


      constraint::bilateral::matrix span_type::loop_closures(const phys::system& system) const {
	phys::constraint::bilateral::matrix res(cycles.size());
	
	 // TODO improve ?
	 auto index = system.constraint.bilateral.matrix.index();

	 core::each( core::all( cycles ), [&](constraint::bilateral::key c) {
	     core::each( index[c], [&](dof::key d) {
		 res(c, d) = system.constraint.bilateral.matrix(c, d);
	       });
	   });

	 return res;
      }

      void data_type::factor(const system& sys, math::real eps) {
	assert(!span.postfix.empty());
      
	data_type tmp;
	tmp.span = span;
	
	// allocs
	tmp.resp_dof.reserve( sys.mass.keys().dim() );
	tmp.resp_constraint.reserve( sys.constraint.bilateral.matrix.rows().dim() ); // TODO tighter
	
	// init diagonal blocks: diag( M, 0 )
	core::each(span.postfix, [&](int v) {
	    auto k = span.tree[v].key;
	    
	    switch( span.tree[v].type ) {
	    case vertex_type::dof: { 
	      auto d = dof::key(k); 
	      tmp.resp_dof(d, d) = sys.mass(d, d); 
	    } break;
	    case vertex_type::constraint: { 
	      auto c = constraint::bilateral::key(k); 

	      const bool single = out_degree(v, span.tree) == 0 || 
		(in_degree(v, span.tree) == 0 && (out_degree(v, span.tree) == 1));
	      
	      if( single ) throw error(DERP);
	      math::real value = single ? std::min(-eps, -1e-8) : -eps;
	      
	      tmp.resp_constraint(c, c).diagonal().setConstant( value ); 
	    } break;
	    };
	  });
	      
	// use tmp as temporary for D
	auto D = [&](int v) { return tmp.Di(v); };
	
	math::vec alloc;

	// acyclic factor
	core::each(span.postfix, [&](tree_type::vertex_descriptor v) {
	    
	    auto p = inv_adjacent_vertices(v, span.tree);
	    
	    // fill J when not root
	    if( p.first != p.second ) {
	      J(v) = matrix_chunk( span.tree, sys.constraint.bilateral.matrix, v);
	    }
	    
	    auto Dv = D(v);

	    // for each child, Dv -= JcT * Dc * Jc
	    core::each( adjacent_vertices(v, span.tree), [&](tree_type::vertex_descriptor c) {
		auto Jc = J(c);

		if( alloc.size() < Jc.size() ) alloc.resize( Jc.size() );
		::sparse::mat_chunk tmp(alloc.data(), Jc.rows(), Jc.cols());

		tmp.noalias() = D(c) * Jc;
		Dv.noalias() -= Jc.transpose() * tmp;
	      });
	    	    
	    // Di = D^{-1}
	    Di(v) = Dv.inverse();

	    // if not root
	    if( p.first != p.second ) { 
	      auto Jv = J(v);
	      
	      if( alloc.size() < Jv.size() ) alloc.resize( Jv.size() );
	      ::sparse::mat_chunk tmp(alloc.data(), Jv.rows(), Jv.cols());
	      
	      // Jv = Di * Jv
	      tmp.noalias() = Di(v) * Jv;
	      Jv = tmp;
	    }
	  });
      
      
      }

   
      static core::storage<math::real> work_vector(const span_type& span) {
	math::natural dim = 0;
	math::natural count = 0;
	
	// count first to avoid multiple allocs
	core::each( vertices( span.tree), [&](int v) {
	    dim += span.tree[v].dim();
	    ++count;
	  });
	
	core::storage<math::real> res;
	res.reserve( count, dim );
	
	core::each( vertices( span.tree), [&](int v) {
	    auto ret = res.add( span.tree[v].dim() ); 
	    assert( int(ret) == v ); core::use( ret );
	  });
	
	return res;
      }

      linear::linear(const data_type& data) : data(data), work( work_vector( data.span) ){ }
      linear::linear(data_type&& data) : data(std::move(data)), work( work_vector( data.span ) ) { }
      
      
      static ::sparse::vec_chunk chunk( core::chunk<math::real> c) {
	return {c.begin, c.dim()};
      }
      

      void linear::solve(const task& t) const {

	work.fill( 0 );
	
	auto sol = [&](int i) {
	  return chunk( work( i ) );
	};
	
	// initialize solution
	core::each(data.span.postfix, [&](int i) {
	    const auto& v = data.span.tree[i];
	    
	    switch( v.type ) {
	    case vertex_type::dof: {
	      t.momentum.find( dof::key( v.key ), [&]( ::sparse::const_vec_chunk f ) {
		  sol(i) = f;
		});
	      
	    } break;
	    case vertex_type::constraint: {
	      t.bilateral.find( constraint::bilateral::key( v.key ), [&]( ::sparse::const_vec_chunk b ) {
		  sol(i) = b;
		});
	    } break;
	    default:
	      throw error(DERP);
	    }
	  });
	
	
	// postfix
	core::each(data.span.postfix, [&](int v) {
	  
	    // for each child
	    core::each( adjacent_vertices(v, data.span.tree), [&](int c) {
		sol(v).noalias() -= data.J(c).transpose() * sol(c);
	      });
	  
	  });
	
	// prefix 
	math::vec alloc;
	core::each(core::reverse(data.span.postfix), [&](int v) {

	    auto s = sol(v);
	    
	    if( alloc.rows() <= s.rows() ) alloc.resize( s.rows() ); 
	    
	    ::sparse::vec_chunk tmp(alloc.data(), s.rows() );
	    tmp.noalias() = data.Di(v) * s;
	    s = tmp;
	    
	    auto p = inv_adjacent_vertices(v, data.span.tree);

	    // if not root
	    if( p.first != p.second ) sol(v).noalias() -= data.J(v) * sol( *p.first );
	    
	    
	    assert(! math::nan( math::vec(sol(v)) ) );
	    
	    // finish
	    auto k = data.span.tree[v].key;
	    switch( data.span.tree[v].type ) {
	    case vertex_type::dof: t.velocity( dof::key(k) ) = sol(v); break;
	    case vertex_type::constraint: 
	      if( t.lambda ) (*t.lambda)( constraint::bilateral::key(k) ) = -sol(v);
	      break;
	    }
	    
	  } );
	
      };



      linear make_linear(const system& sys, math::real eps) {
	auto g = graph( sys );
	auto e = dfs(g);

	auto s = span(g, e);

	auto d = data_type{ std::move(s), {}, {}, {} };
	
	d.factor( sys, eps );
      
	return { std::move(d) };
      }
     


      math::natural vertex_type::dim() const {
	math::natural res = 0;
	switch( type ) {
	case dof: res = ::sparse::dim( dof::key(key) ); break;
	case constraint: res =  ::sparse::dim( constraint::bilateral::key(key)); break;
	default:
	  throw error( DERP );
	};

	assert( res );
	return res;
      }
      

      // propagation MST

      edge_set prop( graph_type& g, const system& s, const task& t ) {
	
	typedef graph_type::vertex_descriptor Vertex;
	typedef graph_type::edge_descriptor Edge;
	typedef graph_type::vertices_size_type VertexIndex;
	
	const math::natural n = num_vertices(g);
	edge_set res;	       
	res.reserve( n );
	
	std::vector<VertexIndex> rank(n);
	std::vector<Vertex> parent(n);
	
	typedef VertexIndex* Rank;
	typedef Vertex* Parent;
	
	disjoint_sets<Rank, Parent> dset(&rank[0], &parent[0]);
	
	// storage data dimensions
	math::natural vec_dim = 0, mat_dim = 0;

	// initial connected component
	core::each(vertices(g), [&](Vertex v) { 
	    dset.make_set(v); 

	    math::natural d = g[v].dim();
	    vec_dim += d;
	    mat_dim += d * d;
	  });

	// init storage
	core::storage<math::real> mat_storage, vec_storage;

	mat_storage.reserve(n, mat_dim);
	vec_storage.reserve(n, vec_dim);

	core::each(vertices(g), [&](Vertex v) {
	    math::natural d = g[v].dim();

	    auto mat = mat_storage.add(d * d); assert( mat == v ); core::use(mat);
	    auto vec = vec_storage.add(d); assert( vec == v ); core::use(vec);
	  });
	
	// response 
	auto Di = [&](Vertex v) -> ::sparse::mat_chunk {
	  auto c = mat_storage(v);
	  auto d = g[v].dim();
	  return ::sparse::mat_chunk( c.begin, d, d );
	};
	
	// net momentum/constraint error
	auto chunk = [&](Vertex v) {
	  auto c = vec_storage(v);
	  return ::sparse::vec_chunk( c.begin, g[v].dim() );
	};
	
	// init dofs
	core::each(vertices(g), [&](Vertex v) { 
	    if( g[v].type == vertex_type::dof ) {
	      auto d = dof::key(g[v].key);
	      
	      chunk(v).setZero();
	      t.momentum.find(d, [&](::sparse::const_vec_chunk f) {
	      	  chunk(v) = f;
	      	});
	      
	      Di(v) = s.resp(d, d);
	    }
	  });

	// constraints
	math::mat D;
	core::each(vertices(g), [&](Vertex v) { 
	    if( g[v].type == vertex_type::constraint ) {
	      auto c = constraint::bilateral::key(g[v].key);

	      chunk(v) = t.bilateral( c );
	      D.setZero( c->dim(), c->dim() );
	      
	      core::each(out_edges(v, g), [&](Edge e) {
		  Vertex u = target(e, g); assert( g[u].type == vertex_type::dof );
		  auto d = dof::key( g[u].key );
		  
		  auto J = s.constraint.bilateral.matrix( c, d );
		  chunk(v).noalias() -= J * (Di(u) * chunk(u) );
		  D += J * Di(u) * J.transpose();
		});

	      Di(v) = D.inverse();
	    }
	  });
	
	auto weight = [&](Vertex c) -> math::real {
	  math::real res = -chunk(c).norm();
	  // math::real res = - ( Di(c) * chunk(c) ).squaredNorm();

	  assert( !math::nan(res) );
	  core::link(&res);	// FIXME WTF? GCC 4.6.2 crashes unless
				// we somehow prevent full inlining
	  return res;
	};

	typedef std::multimap<double, Vertex> queue_type;
	queue_type queue;
	std::map<Vertex, queue_type::iterator> position;

	// fill queue
	auto it_p = position.end();
	core::each( vertices(g), [&](Vertex v) {
	    if( g[v].type == vertex_type::constraint ) {
	      
	      auto it = queue.insert( std::make_pair( weight(v), v) );
	      it_p = position.insert(it_p,  std::make_pair(v, it) );
	    }
	    
	  }); 


	auto pop = [&] {
	  Vertex v = queue.begin()->second;
	  
	  queue.erase( queue.begin() );
	  position.erase( v );
	  return v;
	};


	math::vec lambda, delta;
	while (!queue.empty()) {
	  Vertex c = pop();
	  
	  // MST
	  core::each(out_edges(c, g), [&](Edge e) {

	      Vertex u = dset.find_set(source(e, g));
	      Vertex v = dset.find_set(target(e, g));
	      if ( u != v ) {
		res.push_back( e ); 
		dset.link(u, v);
	      }

	    });

	  // solve constraint
	  lambda.noalias() = Di(c) * chunk(c);

	  // update dofs
	  core::each(adjacent_vertices(c, g), [&](Vertex d) {
	      auto Jcd = s.constraint.bilateral.matrix( constraint::bilateral::key( g[c].key ), 
							dof::key( g[d].key ));
	      delta.noalias() = Jcd.transpose() * lambda;
	      chunk(d) += delta;
	      
	      // update related constraints
	      core::each(adjacent_vertices(d, g), [&](Vertex cc) {
		  auto it_pos = position.find(cc);
		  
		  // only update constraints still in queue
		  if( it_pos != position.end() ) {
		    auto Jccd = s.constraint.bilateral.matrix( constraint::bilateral::key(g[cc].key), 
							       dof::key(g[d].key));
		    chunk(cc).noalias() -= Jccd * (Di(d) * delta);
		    
		    // erase and reinsert constraint 
		    queue.erase( it_pos->second );
		    position[cc] = queue.insert( std::make_pair(weight(cc), cc) );
		  }
		});
	      
	    });
	  
	}
	
	return res;
      } 
      



      
      edge_set perturb(graph_type& g, const edge_set& mst, graph_type::edge_descriptor removed) {
	
	typedef graph_type::vertex_descriptor Vertex;
	typedef graph_type::edge_descriptor Edge;
	typedef graph_type::vertices_size_type VertexIndex;
	
	const int n = num_vertices(g);

	std::vector<VertexIndex> rank(n);
	std::vector<Vertex> parent(n);

	typedef VertexIndex* Rank;
	typedef Vertex* Parent;
	
	disjoint_sets<Rank, Parent> dset(&rank[0], &parent[0]);

	core::each(vertices(g), [&](Vertex v) { 
	    dset.make_set(v); 
	  });
	
	// build connected components
	core::each(mst, [&](Edge e) {
	   
	    if( e == removed ) return;
	    
	    Vertex u = dset.find_set(source(e, g));
	    Vertex v = dset.find_set(target(e, g));
	    if ( u != v ) {
	      dset.link(u, v);
	    }
	    
	  });
	
	edge_set res;
	
	core::each(edges(g), [&](Edge e) {
	    
	    if( e == removed ) return;

	    Vertex u = dset.find_set(source(e, g));
	    Vertex v = dset.find_set(target(e, g));
	    
	    if( u != v ) {
	      res.push_back( e );
	    }
	    
	  });
	
	return res;
      }


      
     
      void data_type::factor_precond(const system& sys, math::real eps) {
	// factor acyclic part
	factor(sys, eps);

	// TODO optimize ? + allocs !
	auto index = sys.constraint.bilateral.matrix.index();
	core::each(core::all(span.cycles), [&](constraint::bilateral::key c) {
	    
	    auto D = resp_constraint(c, c);
	    
	    core::each(core::all(index[c]), [&](dof::key d) {
		
		auto J = sys.constraint.bilateral.matrix(c, d);
		
		D -= J * resp_dof(d, d) * J.transpose();
		matrix(c, d) = J;
		
	      });
	    
	    D.diagonal().array() -= eps;
	    auto Di = D.inverse();
	    
	    assert( Di.size() );

	    D = Di;
	  });
      }
      
      

      void linear::precond(const task& t) const {
	if(! t.lambda ) throw error("i need an output lambda");
	
	work.fill( 0 );
	
	auto sol = [&](int i) {
	  return chunk( work( i ) );
	};
	
	std::unordered_map< dof::key, math::natural > dof_index(data.resp_dof.rows().size());
	
	// initialize solution
	core::each(data.span.postfix, [&](math::natural i) {
	    const auto& v = data.span.tree[i];
	    
	    switch( v.type ) {
	    case vertex_type::dof: {
	      dof_index[ dof::key( v.key ) ] = i;
	    } break;
	    case vertex_type::constraint: {
	      t.bilateral.find( constraint::bilateral::key( v.key ), [&]( ::sparse::const_vec_chunk b ) {
		  sol(i) = b;
		});
	    } break;
	    default:
	      throw error(DERP);
	    }
	    assert( !math::nan( math::vec( sol(i) )) );
	  });
	
	
	// postfix
	core::each(data.span.postfix, [&](int v) {
	  
	    core::each( adjacent_vertices(v, data.span.tree), [&](int c) {
		sol(v).noalias() -= data.J(c).transpose() * sol(c);
	      });
	  
	  });

	// TODO optimize !!!
	auto index = data.matrix.index();
	
	// loop-closures:
	core::each(core::all(data.span.cycles), [&](constraint::bilateral::key c) {
	    auto lambda = (*t.lambda)(c);

	    lambda = t.bilateral(c);
	    
	    core::each(index[c], [&](dof::key d) {
		lambda.noalias() -=  data.matrix(c, d) * (data.resp_dof(d, d) * sol( dof_index[d] ));
		
	      });

	  });
	

	core::each(core::all(data.span.cycles), [&](constraint::bilateral::key c) {
	    auto lambda = (*t.lambda)(c);
	    
	    lambda = data.resp_constraint(c, c) * lambda;

	    // for each children dofs
	    core::each(index[c], [&](dof::key d) {
		sol( dof_index[d] ).noalias() -= data.matrix(c, d).transpose() * lambda;
		
	      });
	    
	    lambda = -lambda;
	  });
	
	// prefix 
	math::vec alloc;
	core::each(core::reverse(data.span.postfix), [&](int v) {

	    auto s = sol(v);
	    
	    if( alloc.rows() <= s.rows() ) alloc.resize( s.rows() ); 
	    
	    ::sparse::vec_chunk tmp(alloc.data(), s.rows() );
	    tmp.noalias() = data.Di(v) * s;
	    s = tmp;
	    
	    auto p = inv_adjacent_vertices(v, data.span.tree);

	    // if not root
	    if( p.first != p.second ) sol(v).noalias() -= data.J(v) * sol( *p.first );
	    
	    
	    assert(! math::nan( math::vec(sol(v)) ) );
	    
	    // finish
	    auto k = data.span.tree[v].key;
	    switch( data.span.tree[v].type ) {
	    case vertex_type::dof: 
	      // TODO unneeded ?
	      t.velocity( dof::key(k) ) = sol(v);
	      break;
	    case vertex_type::constraint: 
	      if( t.lambda ) (*t.lambda)( constraint::bilateral::key(k) ) = -sol(v);
	      break;
	    }
	    
	  } );
	
      };


      edge_set cuthill_mckee(graph_type& g){
	
	typedef typename graph_type::vertex_descriptor Vertex;	
	typedef typename graph_type::edge_descriptor Edge;

	// vertex color
	typedef color_traits<default_color_type> Color;
	auto color = get(vertex_color, g );

	auto clear_color = [&] {
	  core::each( vertices(g), [&](Vertex v) {
	      color[ v ] = Color::white();
	    });
	};


	// bfs queue
	std::queue< Vertex > queue;

	edge_set res;
	res.reserve( num_vertices(g) );
	
	auto process = [&](Vertex s) {

	  std::multimap<math::natural, Edge> by_degree;

	  core::each(out_edges(s, g), [&](Edge e) {
		      
	      Vertex t = target(e, g);
	      
	      if( color[t] == Color::white() ) {
		by_degree.insert( std::make_pair( degree(t, g), e ) );
	      }
	      
	    });
	  
	  // enqueue by increasing degree
	  core::each( by_degree, [&](math::natural, Edge e) {

	      Vertex t = target(e, g);

	      color[t] = Color::gray();
	      queue.push( t );

	      res.push_back(e);

	    });

	  color[s] = Color::black();
	};
	

	clear_color();

	// assumes we only have one connected component !
	Vertex start = find_starting_node(g, 0, color, make_out_degree_map(g) );
	
	clear_color();
	
	color[ start ] = Color::gray();
	queue.push( start );
	
	while( !queue.empty() ) {
	  Vertex s = queue.front();
	  queue.pop();
	  
	  process(s);
	}
	
	return res;
      }

      

      std::set< constraint::bilateral::key > acyclic(const edge_set& edges, 
						     const graph_type& g) {
	typedef constraint::bilateral::key key_type;
	std::set< key_type > res;

	span_type s = span(g, edges);
	
	core::each(boost::vertices(s.tree), [&]( int v ) {
	    
	    if( g[v].type == vertex_type::constraint ) {
	      auto k = key_type( g[v].key);
	      
	      if(s.cycles.find(k) == s.cycles.end() ){
		res.insert( k );
	      }
	    } 

	});
	
	return res;
      }



      
    }

  }
}
