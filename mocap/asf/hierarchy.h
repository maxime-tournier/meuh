#ifndef MOCAP_ASF_HIERARCHY_H
#define MOCAP_ASF_HIERARCHY_H


#include <mocap/asf/stuff.h>
#include <mocap/asf/skipper.h>

namespace mocap {
  namespace asf {

    struct hierarchy {

      // first is the parent
      typedef std::vector< std::string > children_type;
      
      std::vector< children_type > topology;
      
      void debug() const;

      template<class Iterator, class Skipper = skipper<Iterator> >
      struct parser : qi::grammar<Iterator, hierarchy(), Skipper > {

	qi::rule<Iterator, hierarchy(), Skipper > start;
	qi::rule<Iterator, children_type() > children;
	qi::rule<Iterator, std::string() > identifier;

	parser() : parser::base_type( start ) {
	  using qi::char_;
	  using qi::double_;
	  using qi::uint_;
	  using qi::lit;
	  using namespace qi::labels;

	  identifier 
	    %= qi::lexeme[ qi::alpha >> *qi::alnum ] - lit("end");
		
	  children %= qi::skip(qi::space - qi::eol)[+identifier];
	  
	  start %= lit(":hierarchy") 
	    >> lit("begin")
	    >> qi::skip(qi::space - qi::eol)[qi::eol >> +(children >> qi::eol) ]
	    >> lit("end");
	}

      };
    
    };
  }
}

BOOST_FUSION_ADAPT_STRUCT(
			  mocap::asf::hierarchy,
			  (std::vector<mocap::asf::hierarchy::children_type>, topology)
			  )

#endif
