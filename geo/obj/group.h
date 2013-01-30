#ifndef GEO_OBJ_GROUP_H
#define GEO_OBJ_GROUP_H

#include <geo/obj/namespaces.h>
#include <boost/spirit/include/qi.hpp>

#include <boost/spirit/include/phoenix_stl.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_object.hpp>
#include <boost/spirit/include/phoenix_fusion.hpp>

namespace geo {
  namespace obj {

    struct group {

      std::string name;

      template<class Iterator>
      struct parser : qi::grammar<Iterator, group(), obj::skipper<Iterator> > {
	
	qi::rule<Iterator, group(), obj::skipper<Iterator> > start;
	qi::rule<Iterator, std::string(), obj::skipper<Iterator> > name;
	

	parser() : parser::base_type(start) {
	  using namespace qi::labels;

	  name %= qi::lexeme[ +(qi::alnum | '.') ];
	  start = qi::lit("g") >> path[ phoenix::at_c<0>(_val) = _1 ];
	  
	}
	
      };
      
    };


  }
}

BOOST_FUSION_ADAPT_STRUCT(geo::obj::group,
			  (std::string, name)
			  )

#endif
