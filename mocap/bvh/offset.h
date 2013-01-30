#ifndef MOCAP_BVH_OFFSET_H
#define MOCAP_BVH_OFFSET_H

#include <mocap/bvh/real.h>
#include <mocap/bvh/namespaces.h>

#include <boost/spirit/include/qi.hpp>
#include <boost/fusion/include/adapt_struct.hpp>


namespace mocap {
  namespace bvh {

    struct offset {
      real x, y, z;

      template<class Iterator>
      struct parser : qi::grammar<Iterator, offset(), ascii::space_type > {

	qi::rule<Iterator, offset(), ascii::space_type > start;
	
	parser() : parser::base_type( start  ) {
	  using qi::double_;
	  
	  start 
	    %= qi::lit("OFFSET") >> double_ >> double_ >> double_;
	}
	
      };
      
    };
    

  }
}

BOOST_FUSION_ADAPT_STRUCT(
			  mocap::bvh::offset,
			  (mocap::bvh::real, x)
			  (mocap::bvh::real, y)
			  (mocap::bvh::real, z)
			  )



#endif
