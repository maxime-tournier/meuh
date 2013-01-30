#ifndef GEO_OBJ_MTLLIB_H
#define GEO_OBJ_MTLLIB_H

#include <geo/obj/namespaces.h>
#include <boost/spirit/include/qi.hpp>

#include <boost/spirit/include/phoenix_stl.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_object.hpp>
#include <boost/spirit/include/phoenix_fusion.hpp>

namespace geo {
  namespace obj {

    struct mtllib {

      std::string path;

      template<class Iterator>
      struct parser : qi::grammar<Iterator, mtllib(), obj::skipper<Iterator> > {
	
	qi::rule<Iterator, mtllib(), obj::skipper<Iterator> > start;
	qi::rule<Iterator, std::string(), obj::skipper<Iterator> > path;
	

	parser() : parser::base_type(start) {
	  using namespace qi::labels;

	  using boost::spirit::_1;

	  path %= qi::lexeme[ +(qi::alnum | '.') ];
	  start = qi::lit("mtllib") >> path[ phoenix::at_c<0>(_val) = _1 ];
	  
	}
	
      };
      
    };


  }
}

BOOST_FUSION_ADAPT_STRUCT(geo::obj::mtllib,
			  (std::string, path)
			  )

#endif
