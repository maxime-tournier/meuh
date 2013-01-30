#ifndef GEO_OBJ_FILE_H
#define GEO_OBJ_FILE_H

// #define BOOST_SPIRIT_DEBUG 1


#include <geo/obj/skipper.h>
#include <geo/obj/face.h>
#include <geo/obj/shading.h>
#include <geo/obj/mtllib.h>
#include <geo/obj/usemtl.h>

#include <geo/error.h>

#include <core/use.h>

namespace geo {

  namespace obj {

    // TODO deal with objects and groups and shading
    struct file {
      
      std::vector<vertex> vertices;
      std::vector<texcoord> texcoords;
      std::vector<normal> normals;
      std::vector<face> faces;

      template<class Iterator>
      struct parser  : qi::grammar<Iterator, file(), obj::skipper<Iterator> > {
	
	qi::rule<Iterator, file(), obj::skipper<Iterator> > start;
	qi::rule<Iterator, std::string(), obj::skipper<Iterator> > name;

	qi::rule<Iterator, std::vector<obj::vertex>(), obj::skipper<Iterator> > vlist;
	qi::rule<Iterator, std::vector<obj::texcoord>(), obj::skipper<Iterator> > tlist;
	qi::rule<Iterator, std::vector<obj::normal>(), obj::skipper<Iterator> > nlist;
	qi::rule<Iterator, std::vector<obj::face>(), obj::skipper<Iterator> > flist;
	
	obj::vertex::parser<Iterator> vertex;
	obj::texcoord::parser<Iterator> texcoord;
	obj::normal::parser<Iterator> normal;
	obj::face::parser<Iterator> face;
	obj::shading::parser<Iterator> shading;
	obj::mtllib::parser<Iterator> mtllib;
	obj::usemtl::parser<Iterator> usemtl;
	
	parser() : parser::base_type(start) {
	  using phoenix::at_c;
	  using namespace qi::labels;
	  using boost::spirit::_1;
	  
	  vlist %= +vertex;
	  tlist %= +texcoord;
	  nlist %= +normal;
	  flist %= +face;
	  
	  start 
	    = 
	    ( - mtllib )
	    >>
	    ( vlist[ at_c<0>(_val) = _1 ]
	      ^ tlist[ at_c<1>(_val) = _1 ]
	      ^ nlist [ at_c<2>(_val) = _1 ]
	      )
	    >> 	((-usemtl) ^ (-shading) ^ (-mtllib)  )
	    >> flist[ at_c<3>(_val) = _1 ];
	    ;
	  // debug(start);
 
	}
	
      };


      template<class Iterator>
      void parse(Iterator& first, const Iterator& last) {
	parser<Iterator> parser;
	skipper<Iterator> skipper;
	
	bool ok = qi::phrase_parse( first, last, parser, skipper, *this);
	core::use(ok);

	// if( ok ) std::cout << "successful match !" << std::endl;

	if( first != last )  {
	  std::cout << "stopped at: " << std::string(first, std::min(first + 100, last) ) << std::endl;
	  throw error("obj parsing failed !");
	}
	
      }
      

      file(const std::string& filename);
      file() { }

    };

  }
}



		  

BOOST_FUSION_ADAPT_STRUCT(geo::obj::file,
			  (std::vector< geo::obj::vertex >, vertices)
			  (std::vector< geo::obj::texcoord >, texcoords)
			  (std::vector< geo::obj::normal >, normals)
			  (std::vector< geo::obj::face >, faces)
			  )



#endif
