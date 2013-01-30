#ifndef CORE_GRAPH_H
#define CORE_GRAPH_H


#include <boost/graph/adjacency_list.hpp>
#include <core/unused.h>

namespace core {
  
  template<class Direction, class Vertex, class Edge>
  struct graph_traits {
    
    typedef boost::property<boost::vertex_bundle_t, Vertex,
			    boost::property< boost::vertex_color_t, boost::default_color_type> > vertex_properties;
    
    typedef boost::property<boost::edge_bundle_t, Edge, 
			    boost::property< boost::edge_color_t, boost::default_color_type> > edge_properties;
    
    typedef boost::adjacency_list<boost::vecS, boost::vecS, Direction, vertex_properties, edge_properties > base;
    
  };
  
  template<class Direction = boost::undirectedS, class Vertex = unused, class Edge = unused>
  struct graph : graph_traits<Direction, Vertex, Edge>::base {
    
    typedef graph_traits<Direction, Vertex, Edge> traits;
    
    template<class ... Args>
    graph(Args&&... args) : traits::base( std::forward<Args>(args)...) { }
    
    graph(const graph& ) = default;
    graph(graph&& other) : traits::base(other) { }
    graph(const graph&& other) : traits::base(other) { }
    
    graph& operator=(const graph& ) = default;
    graph& operator=(graph&& other ) {
      traits::base::operator=(other);
      return *this;
    }

    
  };


 


  
}



#endif
