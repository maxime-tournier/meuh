#ifndef MOCAP_BVH_FILE_H
#define MOCAP_BVH_FILE_H

#include <mocap/bvh/hierarchy.h>
#include <mocap/bvh/motion.h>

namespace mocap {
  namespace bvh {
    
    struct file {

      bvh::hierarchy hierarchy;
      bvh::motion motion;
      
      template<class Iterator>
      struct parser  : qi::grammar<Iterator, file(), ascii::space_type > {

	qi::rule<Iterator, file(), ascii::space_type > start;

	bvh::hierarchy::parser<Iterator> hierarchy;
	bvh::motion::parser<Iterator> motion;

	parser() : parser::base_type(start) {
	  
	  using phoenix::at_c;
	  using namespace qi::labels;

	  start 
	    =  hierarchy[ at_c<0>(_val) = _1 ]  
	    >> motion[ at_c<1>(_val) = _1 ];
	  
	}
      };

      file()  { }
      file(const std::string& filename);

      template<class Iterator>
      void parse(Iterator first, Iterator last) {
	file::parser<Iterator> parser;
	
	bool ok = qi::phrase_parse( first, last, parser, ascii::space, *this);
	
	struct error : std::exception { };
	if(!ok || first != last ) { throw error(); }
	
      }

      

    };

  }
}

BOOST_FUSION_ADAPT_STRUCT(
			  mocap::bvh::file,
			  (mocap::bvh::hierarchy, hierarchy)
			  (mocap::bvh::motion, motion)
			  )

#endif
