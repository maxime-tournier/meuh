#ifndef GEO_OBJ_SHADING_H
#define GEO_OBJ_SHADING_H

#include <geo/obj/namespaces.h>
#include <boost/spirit/include/qi.hpp>

namespace geo {
  namespace obj {


    struct shading {

      template<class Iterator>
      struct parser : qi::grammar<Iterator, shading(), obj::skipper<Iterator> > {
	
	qi::rule<Iterator, shading(), obj::skipper<Iterator> > start;
	

	parser() : parser::base_type(start) {

	  start = qi::lit("s") >> 
	    (( qi::lit("on") | qi::lit("1") ) |
	     ( qi::lit("off") | qi::lit("0") ));
	  
	}
	
      };
      
    };
      

  }
}



#endif
