#ifndef MOCAP_BVH_CHANNEL_H
#define MOCAP_BVH_CHANNEL_H

#include <mocap/bvh/namespaces.h>
#include <boost/spirit/include/qi.hpp>
#include <boost/fusion/include/adapt_struct.hpp>

#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_object.hpp>

namespace mocap {
  namespace bvh {

    struct channel {

      typedef enum { X = 'X',
		     Y = 'Y',
		     Z = 'Z' } axis_type;
      
      axis_type axis;
      
      typedef enum { position,
		     rotation } kind_type ;
      
      kind_type kind;
      

      template<class Iterator>
      struct parser : qi::grammar<Iterator, channel() > {

	qi::rule<Iterator, channel()> start;

	qi::rule<Iterator, channel::axis_type()> axis;
	qi::rule<Iterator, channel::kind_type()> kind;

	parser() : parser::base_type( start ) {
	  using qi::char_;
	  using namespace qi::labels;

	  axis = char_('X') | char_('Y') | char_('Z');
	  kind = qi::lit("position")[ _val = channel::position ]
	    | qi::lit("rotation")[ _val = channel::rotation ];
	  
	  start %= axis >> kind;
	}
	
      };



    };


  }
}


BOOST_FUSION_ADAPT_STRUCT(
			  mocap::bvh::channel,
			  (mocap::bvh::channel::axis_type, axis)
			  (mocap::bvh::channel::kind_type, kind)
			  )

#endif
