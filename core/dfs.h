#ifndef CORE_DFS_H
#define CORE_DFS_H

#include <core/graph.h>

#include <boost/graph/depth_first_search.hpp>
#include <boost/graph/undirected_dfs.hpp>

#include <core/noop.h>

namespace core {

  namespace visitor {

    template<class F>
    struct prefix : boost::dfs_visitor<> {
      const F& f;
      
      prefix(const F& f) : f(f) { }

      template<class V, class G>
      void discover_vertex(V v, const G& ) const {
	f(v);
      }
    };

    template<class F>
    struct postfix : boost::dfs_visitor<> {
      const F& f;
      
      postfix(const F& f) : f(f) { }


      template<class V, class G>
      void finish_vertex(V v, const G& ) const {
	f(v);
      }
    };


      
    template<class Pre = core::noop , class Post = core::noop >
    struct prepostfix : boost::dfs_visitor<> {

      const Pre pre;
      const Post post;
      
      prepostfix(const Pre& pre = Pre(),
	      const Post& post = Post()) 
	: pre(pre),
	  post(post) { 
	
      };

      template<class V, class G>
      void discover_vertex(V v, const G& ) const {
	pre(v);
      }


      template<class V, class G>
      void finish_vertex(V v, const G& ) const {
	post(v);
      }
      
    };



  }

  template<class F>
  visitor::prefix<F> prefix(const F& f) { return {f}; }

  template<class F>
  visitor::postfix<F> postfix(const F& f) { return {f}; }

  template<class Pre, class Post>
  visitor::prepostfix<Pre, Post> prepostfix(const Pre& pre,
					    const Post& post) {
    return { pre, post };
  }
  
  template<class Vertex, class Edge, class Visitor>
  void dfs(graph<boost::undirectedS, Vertex, Edge>& g,
	   const Visitor& vis,
	   typename graph<boost::undirectedS, Vertex, Edge>::vertex_descriptor start = 0) {
    boost::undirected_dfs(g, boost::visitor( vis )
			  .root_vertex( start )
			  .edge_color_map( boost::get( boost::edge_color, g ) ) );
  };

  template<class Vertex, class Edge, class Visitor>
  void dfs(graph<boost::bidirectionalS, Vertex, Edge>& g,
	   const Visitor& vis,
	   typename graph<boost::undirectedS, Vertex, Edge>::vertex_descriptor start = 0) {
    boost::depth_first_search(g, boost::visitor(  vis )
			      .root_vertex( start )
			      .edge_color_map( boost::get( boost::edge_color, g ) ) );
  };
  
	      
  
}



#endif
