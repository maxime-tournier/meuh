#ifndef MOCAP_BVH_JOINT_H
#define MOCAP_BVH_JOINT_H

#include <mocap/bvh/namespaces.h>

#include <mocap/bvh/channel.h>
#include <mocap/bvh/offset.h>

#include <boost/spirit/include/qi.hpp>
#include <boost/fusion/include/adapt_struct.hpp>

#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_object.hpp>
#include <boost/spirit/include/phoenix_fusion.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>



namespace mocap {
  namespace bvh {

    struct joint {

      std::string name;

      bvh::offset offset;
      
      typedef std::vector< bvh::channel > channels_type;
      channels_type channels;
    
      std::vector< joint > children;

      template<class Iterator>
      struct parser : qi::grammar<Iterator, joint( std::string ), ascii::space_type > {
	
	qi::rule<Iterator, joint( std::string ), ascii::space_type > start;
	qi::rule<Iterator, std::string( std::string ), ascii::space_type > header;
	qi::rule<Iterator, joint(), ascii::space_type >  end_site;
	qi::rule<Iterator, std::vector<joint>(), ascii::space_type > children;
	qi::rule<Iterator, std::string(), ascii::space_type > identifier;
	qi::rule<Iterator, std::vector<bvh::channel>(), ascii::space_type> channels;
	
	bvh::channel::parser<Iterator> channel;
	bvh::offset::parser<Iterator> offset;
	
	parser() : parser::base_type( start ) {
	  using qi::char_;
	  using namespace qi::labels;
	  using phoenix::at_c;
	  using phoenix::push_back;
	  
	  start 
	    %=  header(_r1) [ at_c<0>(_val) = _1 ]
	    >> '{'
	    >> offset
	    >> channels
	    >> children
	    >> '}'
	    ;
	  
	  channels 
	    %= qi::lit("CHANNELS") 
	    >> qi::omit[ qi::uint_ ] 
	    >> +qi::lexeme[channel];

	  header 
	    %= qi::lit(_r1) >> identifier;
	  
	  identifier 
	    %= qi::lexeme[ qi::alpha >> *qi::alnum ];
	  
	  end_site 
	    = qi::lit("End Site") // TODO enhance flexibility
	    >> '{'
	    >> offset[ at_c<1>(_val) = _1 ]
	    >> '}';
	  
	  children 
	    = (+start( std::string("JOINT") )) [ _val = _1 ]  
	    | end_site[ push_back(_val, _1) ];
	  
	  
	}
	
      };



    };

    
  }
}

BOOST_FUSION_ADAPT_STRUCT(
			  mocap::bvh::joint,
			  (std::string, name)
			  (mocap::bvh::offset, offset)
			  (std::vector<mocap::bvh::channel>, channels)
			  (std::vector<mocap::bvh::joint>, children)
			  )


#endif
