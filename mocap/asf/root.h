#ifndef MOCAP_ASF_ROOT_H
#define MOCAP_ASF_ROOT_H

#include <mocap/asf/stuff.h>

#include <mocap/asf/vec3.h>
#include <mocap/asf/axis.h>
#include <mocap/asf/kind.h>
#include <mocap/asf/skipper.h>

#include <vector>

namespace mocap  {
  namespace asf {
    
    // :root section
    struct root {

      struct order_type {
	asf::kind kind;
	asf::axis axis;
      };
      
      std::vector< order_type  > order;
      std::vector< asf::axis > axis;
      
      vec3 position;
      vec3 orientation;
      
      void debug() const;

      template<class Iterator, class Skipper = skipper<Iterator> >
      struct parser : qi::grammar<Iterator, root(), Skipper > {

	qi::rule<Iterator, root(), Skipper > start;
	qi::rule<Iterator, order_type() > order;

	qi::rule<Iterator, asf::kind() > kind;
	qi::rule<Iterator, asf::axis() > axis;
	
	asf::vec3::parser<Iterator, Skipper> vec3;
	
	parser() : parser::base_type( start ) {
	  using qi::char_;
	  using qi::double_;
	  using qi::uint_;
	  using qi::lit;
	  using namespace qi::labels;

	  kind = qi::lit("T")[ _val = asf::kind::translation ]
	    | qi::lit("R")[ _val = asf::kind::rotation ];
	  
	  axis = char_('X') | char_('Y') | char_('Z');
	  
	  order = qi::lexeme[ kind >> axis ];

	  start %= qi::lit(":root")
	    >> qi::lit("order") >> (+order)
	    >> qi::lit("axis") >> (+axis)
	    >> qi::lit("position") >> vec3
	    >> qi::lit("orientation") >> vec3
	    ;

	};


      };

    };

  }
}

BOOST_FUSION_ADAPT_STRUCT(
			  mocap::asf::root::order_type,
			  (mocap::asf::kind, kind)
			  (mocap::asf::axis, axis) 
			  )

BOOST_FUSION_ADAPT_STRUCT(
			  mocap::asf::root,
			  (std::vector< mocap::asf::root::order_type >, order)
			  (std::vector< mocap::asf::axis >, axis)
			  (mocap::asf::vec3, position)
			  (mocap::asf::vec3, orientation)
			  )

#endif
