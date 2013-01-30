#ifndef GEO_OBJ_NORMAL_H
#define GEO_OBJ_NORMAL_H

#include <geo/obj/namespaces.h>
#include <geo/obj/real.h>

#include <boost/spirit/include/qi.hpp>
#include <boost/fusion/include/adapt_struct.hpp>


namespace geo {
  namespace obj {

    
    struct normal {
      typedef unsigned int index;
      real x, y, z;

      template<class Iterator>
      struct parser : qi::grammar<Iterator, normal(), obj::skipper<Iterator> > {
	
	qi::rule<Iterator, normal(), obj::skipper<Iterator> > start;

	parser() : parser::base_type(start) {
	  using qi::double_;
	  start %= qi::lit("vn") >> double_ >> double_ >> double_;
	}
      };
    };



  }
}


BOOST_FUSION_ADAPT_STRUCT(geo::obj::normal,
			  (geo::obj::real, x)
			  (geo::obj::real, y)
			  (geo::obj::real, z)
			  )
	


#endif
