#ifndef GEO_OBJ_SKIPPER_H
#define GEO_OBJ_SKIPPER_H


#include <geo/obj/namespaces.h>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/qi_auxiliary.hpp>


namespace geo {
  namespace obj {

    template<class Iterator>
    struct comment : qi::grammar<Iterator> {
      qi::rule<Iterator> start;

      comment() : comment::base_type(start) {
	using qi::char_;
	using qi::eol;
	start = char_('#') >> *( char_ - eol ) >> eol;
      }
    };

    template<class Iterator>
    struct skipper : qi::grammar<Iterator> {
      qi::rule<Iterator> start;
      obj::comment<Iterator> comment;

      skipper() : skipper::base_type(start) {
	
	start = qi::space | comment;
      }
    };

  }
}


#endif
