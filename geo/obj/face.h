#ifndef GEO_OBJ_FACE_H
#define GEO_OBJ_FACE_H

#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_object.hpp>
#include <boost/spirit/include/phoenix_fusion.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>

#include <geo/obj/vertex.h>
#include <geo/obj/texcoord.h>
#include <geo/obj/normal.h>

namespace geo {
  namespace obj {
    
    struct face {
      typedef unsigned int index;
      
      std::vector< index > vertices;
      std::vector< index > texcoords;
      std::vector< index > normals;
      
      bool ok() const {
	if( vertices.empty() ) return false;
	if( !texcoords.empty() && texcoords.size() != vertices.size() ) return false;
	if( !normals.empty() && normals.size() != vertices.size() ) return false;
	
	return true;
      }


      template<class Iterator>
      struct parser : qi::grammar<Iterator, face(), obj::skipper<Iterator> > {
	
	qi::rule<Iterator, face(), obj::skipper<Iterator>> start;
	qi::rule<Iterator > slash;

	parser() : parser::base_type(start) {
	  using qi::uint_;
	  using qi::char_;
	  using qi::repeat;
	  using qi::lexeme;
	  using qi::eps;
	  using qi::inf;
	  using phoenix::at_c;
	  using phoenix::push_back;
	  using namespace qi::labels;

	  using boost::spirit::_1;

	  slash = char_('/');
	  
	  start 
	    = qi::lit("f") 
	    >>    
			    
	    repeat(3, inf) [ lexeme [ 
				     // vertex index
				     uint_ [ push_back( at_c<0>(_val), _1 ) ] 
				     >> 
				     // then maybe something after
				     -( slash  
					>> 
					( 
					 uint_ [ push_back( at_c<1>(_val), _1 ) ] 
					 ||  (  slash >> uint_ [ push_back( at_c<2>(_val), _1 ) ]  )
					  )
					)
				     
				     
				      ]
			     ]
	    ;
	  
	}
      };

    };



  }
}


BOOST_FUSION_ADAPT_STRUCT(geo::obj::face,
			  (std::vector< geo::obj::face::index >, vertices)
			  (std::vector< geo::obj::face::index >, texcoords)
			  (std::vector< geo::obj::face::index >, normals)
			  )


#endif
