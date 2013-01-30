#ifndef GEO_OBJ_VERTEX_H
#define GEO_OBJ_VERTEX_H

#include <geo/obj/namespaces.h>
#include <geo/obj/real.h>

#include <boost/spirit/include/qi.hpp>
#include <boost/fusion/include/adapt_struct.hpp>


namespace geo {

  namespace obj {

    struct vertex {
      real x, y, z;
      
      template<class Iterator>
      struct parser : qi::grammar<Iterator, vertex(), obj::skipper<Iterator> > {
	
	qi::rule<Iterator, vertex(), obj::skipper<Iterator> > start;

	parser() : parser::base_type(start) {
	  using qi::double_;
	  start %= qi::lit("v") >> double_ >> double_ >> double_;
	}
	
      };

    };

  }

}


BOOST_FUSION_ADAPT_STRUCT(geo::obj::vertex,
			  (geo::obj::real, x)
			  (geo::obj::real, y)
			  (geo::obj::real, z)
			  )



#endif
