#ifndef MOCAP_ASF_VEC_H
#define MOCAP_ASF_VEC_H


#include <mocap/asf/stuff.h>
#include <mocap/asf/skipper.h>

namespace mocap {
  namespace asf {

    // some 3-vector structure
    struct vec3 {
      real x, y, z;

      template<class Iterator, class Skipper = skipper<Iterator> >
      struct parser : qi::grammar<Iterator, vec3(), Skipper > {
	qi::rule<Iterator, vec3(), Skipper > start;

	parser() : parser::base_type( start ) {
	  using qi::char_;
	  using qi::double_;
	  using qi::uint_;
	  using qi::lit;
	  using namespace qi::labels;
	  
	  start %= qi::skip( qi::space - qi::eol ) [double_ >> double_ >> double_];

	}

      };
      
    };

  }
}

BOOST_FUSION_ADAPT_STRUCT(
			  mocap::asf::vec3,
			  (mocap::asf::real, x)
			  (mocap::asf::real, y)
			  (mocap::asf::real, z)
			  )

#endif
