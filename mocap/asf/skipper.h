#ifndef MOCAP_ASF_SKIPPER_H
#define MOCAP_ASF_SKIPPER_H

#include <mocap/asf/stuff.h>

namespace mocap {
  namespace asf {

    template<class Iterator>
    struct skipper : qi::grammar<Iterator> {
      
      qi::rule<Iterator> start, space, comment;

      skipper() : skipper::base_type( start ) {
	
	space 
	  %= qi::space;

	comment
	  %= qi::char_('#') >> *(qi::char_ - qi::eol) >> qi::eol;
	
	start 
	  %= comment | space;
	
      }
      
    };


  }
}


#endif
