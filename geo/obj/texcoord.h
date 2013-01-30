#ifndef GEO_OBJ_TEXCOORD_H
#define GEO_OBJ_TEXCOORD_H

#include <geo/obj/namespaces.h>
#include <geo/obj/real.h>

#include <boost/spirit/include/qi.hpp>
#include <boost/fusion/include/adapt_struct.hpp>

namespace geo {
  namespace obj {

    // TODO deal with 3-dimensional texcoords 
    struct texcoord {
      typedef unsigned int index;
      real u, v;

      template<class Iterator>
      struct parser : qi::grammar<Iterator, texcoord(), obj::skipper<Iterator> > {
	
	qi::rule<Iterator, texcoord(), obj::skipper<Iterator> > start;
	
	parser() : parser::base_type(start) {
	  using qi::double_;
	  start %= qi::lit("vt") >> double_ >> double_;
	}
	
      };
      
    };


  }
}


BOOST_FUSION_ADAPT_STRUCT(geo::obj::texcoord,
			  (geo::obj::real, u)
			  (geo::obj::real, v)
			  )


#endif
