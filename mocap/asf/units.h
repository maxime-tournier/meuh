#ifndef MOCAP_ASF_UNITS_H
#define MOCAP_ASF_UNITS_H

#include <mocap/asf/stuff.h>
#include <mocap/asf/skipper.h>

namespace mocap  {
  namespace asf {

    // :units section
    struct units {
      real mass;
      real length;
      
      enum angle_type {
	deg,
	rad
      };

      angle_type angle;
      
      void debug() const;

      template<class Iterator, class Skipper = skipper<Iterator> >
      struct parser : qi::grammar<Iterator, units(), Skipper > {
	
	qi::rule<Iterator, units(), Skipper > start;
	qi::rule<Iterator, angle_type(), Skipper > angle;
	
	parser() : parser::base_type( start ) {
	  using qi::char_;
	  using qi::double_;
	  using qi::uint_;
	  using qi::lit;
	  using namespace qi::labels;
	  
	  angle = qi::lit("deg")[ _val = units::deg ] 
	    | qi::lit("rad")[ _val = units::rad ];


	  start %= 
	    lit(":units")
	    >> lit("mass") >> double_
	    >> lit("length") >> double_ 
	    >> lit("angle") >> angle;
	}
      };


    };

    
    


  }
}



BOOST_FUSION_ADAPT_STRUCT(
			  mocap::asf::units,
			  (mocap::asf::real, mass)
			  (mocap::asf::real, length)
			  (mocap::asf::units::angle_type, angle)
			  )


#endif
