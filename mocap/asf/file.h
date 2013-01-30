#ifndef MOCAP_ASF_FILE_H
#define MOCAP_ASF_FILE_H

#include <string>

#include <mocap/asf/skipper.h>
#include <mocap/asf/units.h>
#include <mocap/asf/root.h>
#include <mocap/asf/bone.h>
#include <mocap/asf/hierarchy.h>

namespace mocap {
  namespace asf {
    
    struct file {

      std::string version;
      std::string name;

      asf::units units;
      
      std::string documentation;
      
      asf::root root;
      
      std::vector< asf::bone > bonedata;
      
      asf::hierarchy hierarchy;
      
      void debug() const;

      template<class Iterator, class Skipper = skipper<Iterator> >
      struct parser : qi::grammar<Iterator, file(), Skipper > {

	qi::rule<Iterator, file(), Skipper > start;
	qi::rule<Iterator, std::string(), Skipper > version, name, documentation;
	
	qi::rule<Iterator, std::string() > doc_line;
	
	qi::rule<Iterator, std::vector< asf::bone >(), Skipper > bonedata;
	
	asf::units::parser<Iterator, Skipper> units;
	asf::root::parser<Iterator, Skipper> root;
	asf::bone::parser<Iterator, Skipper> bone;
	
	asf::hierarchy::parser<Iterator, Skipper> hierarchy;
	
	parser() : parser::base_type( start ) {
	  using namespace qi::labels;

	  version 
	    %= qi::lit(":version")
	    >> qi::lexeme[ qi::digit >> *( qi::digit | qi::char_('.') ) ];
	  
	  name 
	    %= qi::lit(":name")
	    >> qi::lexeme[ +qi::alnum ];
	  
	  doc_line 
	    %= *(qi::char_ - qi::eol) - qi::lit(":root");

	  // TODO add back trailing eol
	  documentation 
	    = qi::lit(":documentation")
	    >> qi::lexeme[ doc_line[ _val += _1 ] % qi::eol ];
	  
	  bonedata
	    %= qi::lit(":bonedata")
	    >> +bone;

	  start 
	    %= version 
	    >> name
	    >> units 
	    >> documentation
	    >> root
	    >> bonedata
	    >> hierarchy
	    ;

	}

	
      };

    };

  }
}


BOOST_FUSION_ADAPT_STRUCT(
			  mocap::asf::file,
			  (std::string, version)
			  (std::string, name)
			  (mocap::asf::units, units)
			  (std::string, documentation)
			  (mocap::asf::root, root)
			  (std::vector< mocap::asf::bone >, bonedata)
			  (mocap::asf::hierarchy, hierarchy)
			  )

#endif
