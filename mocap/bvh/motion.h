#ifndef MOCAP_BVH_MOTION_H
#define MOCAP_BVH_MOTION_H

#include <mocap/bvh/namespaces.h>
#include <mocap/bvh/real.h>

#include <boost/spirit/include/qi.hpp>
#include <boost/fusion/include/adapt_struct.hpp>

#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_object.hpp>


namespace mocap {
  namespace bvh {

    struct motion {
      
      unsigned int frames;
      real frame_time;
      
      std::vector< std::vector< real > > data;
      
      template<class Iterator>
      struct parser : qi::grammar<Iterator, motion(), ascii::space_type > {

	qi::rule<Iterator, motion(), ascii::space_type > start;
	qi::rule<Iterator, std::vector<real>(), ascii::space_type > frame;
	
	parser() : parser::base_type( start ) {
	  using qi::char_;
	  using qi::double_;
	  using qi::uint_;
	  using qi::lit;
	  using namespace qi::labels;
	  
	  start %= 
	    lit("MOTION")
	    >> lit("Frames:") >> uint_
	    >> lit("Frame Time:") >> double_ 
	    >> +frame;
	  
	  frame 
	    %= qi::lexeme[ (double_  % qi::blank) >> qi::eol ];
	  
	}
	
      };



    };

  }
}

BOOST_FUSION_ADAPT_STRUCT(
			  mocap::bvh::motion,
			  (unsigned int, frames)
			  (mocap::bvh::real, frame_time)
			  (std::vector<std::vector< mocap::bvh::real > >, data)
			  )



#endif
