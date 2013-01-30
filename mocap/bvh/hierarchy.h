#ifndef MOCAP_BVH_HIERARCHY_H
#define MOCAP_BVH_HIERARCHY_H

#include <mocap/bvh/joint.h>

namespace mocap {
  namespace bvh {

    struct hierarchy {

      joint root;
      
      template<class Iterator>
      struct parser  : qi::grammar<Iterator, hierarchy(), ascii::space_type > {
	
	qi::rule<Iterator, hierarchy(), ascii::space_type > start;
	bvh::joint::parser<Iterator> joint;
	
	parser() : parser::base_type(start) {
	  using phoenix::at_c;
	  using namespace qi::labels;
	  
	  start 
	    = qi::lit("HIERARCHY") 
	    >> joint(std::string("ROOT"))[ at_c<0>(_val) = _1 ] ;
	  
	}
      };

    };
    
  }
}

BOOST_FUSION_ADAPT_STRUCT(
			  mocap::bvh::hierarchy,
			  (mocap::bvh::joint, root)
			  )


#endif
