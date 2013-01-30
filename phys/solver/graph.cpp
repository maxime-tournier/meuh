#include "graph.h"

#include <list>
#include <boost/graph/undirected_dfs.hpp>
#include <boost/property_map/property_map.hpp>

#include <math/nan.h>

#include <sparse/zero.h>
#include <core/stl.h>

#include <sparse/iter.h>

using namespace boost;

namespace phys {
  namespace solver {

    namespace graph {
      
      bad_vertex::bad_vertex() : core::error("vertex node should be a dof* or a contraint*") { }
      
      writer::writer(const constraints& input) : input(input) { }

      // TODO enhance :)
      void writer::operator()(std::ostream& out, const constraints::vertex_descriptor& v) const {
	  out << "[label=\"";
	
	  switch( input[v].type ) {
	  case vertex::dof : 
	    out << "D" ;
	    break;
	  case vertex::constraint:
	    out << "C" ;
	    break;
	  }	  
	  out << "\"]";
      }
    
      
      vertex make_vertex(const phys::dof* d) {
	vertex res;
	res.type = vertex::dof;
	res.data = d;
	return res;
      }

      vertex make_vertex(const phys::constraint::bilateral* c) {
	vertex res;
	res.type = vertex::constraint;
	res.data = c;
	return res;
      }
      


      struct helper {

	// binds dof handles to vertices
	typedef std::map<phys::dof::key, constraints::vertex_descriptor> vertex_map_type;
	mutable vertex_map_type vertex_map;

	mutable constraints res;

	void operator()(phys::constraint::bilateral::key c,
			const phys::constraint::bilateral::row& row,
			const math::real& w) const {

	  // add constraint vertex
	  constraints::vertex_descriptor vc = add_vertex( make_vertex(c) , res );
	
	  // iterate over constraint blocks, add corresponding dof
	  // vertices if needed
	  core::each(row, [&](phys::dof::key d, const math::mat& J) {
	      
	      constraints::vertex_descriptor vd;
	      
	      // look for existing vertex 
	      vertex_map_type::const_iterator v = vertex_map.find( d ); // O( log(n) )
	      if( v == vertex_map.end() ) {
		vd = add_vertex( make_vertex( d ), res );
		vertex_map[ d ] = vd; // O( log(n) )
	      } else {
		vd = v->second;
	      }
	    
	      // add edge
	      add_edge(vc, vd, edge( J, w ), res);
	    });
	  
	}
	    
	    


	void operator()(phys::constraint::bilateral::key k,
			const phys::constraint::bilateral::row& row,
			sparse::zero ) const {
	  operator()(k, row, 0.0);
	}
	
	
	void operator()(phys::constraint::bilateral::key,
			sparse::zero,
			const math::real& ) const {
	}



      };


      constraints make(const phys::constraint::bilateral::matrix& matrix,
		       const weights_type& weights) {
	
	helper h;
	sparse::iter(core::range::all(matrix), core::range::all(weights), h);
	
	return h.res;

      }

      
    

    }
  }
}


