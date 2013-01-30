#ifndef MOCAP_ASF_BONE_H
#define MOCAP_ASF_BONE_H

#include <mocap/asf/stuff.h>
#include <mocap/asf/skipper.h>

#include <mocap/asf/vec3.h>
#include <mocap/asf/axis.h>
#include <mocap/asf/kind.h>

#include <vector>


namespace mocap {
  namespace asf {

    struct bone {
      typedef unsigned int id_type;
      id_type id;
      std::string name;

      vec3 direction;
      real length;

      struct axis_type {
	vec3 offset;
	std::vector<asf::axis> order;
      } axis;
    
      struct dof_type {
	asf::kind kind;
	asf::axis axis;
      };

      std::vector< dof_type > dof;

      struct limits_type {
	real lower, upper;
      };
      
      std::vector< limits_type > limits;
      

      void debug() const;

      template<class Iterator, class Skipper = skipper<Iterator> >
      struct parser : qi::grammar<Iterator, bone(), Skipper > {

	qi::rule<Iterator, bone(), Skipper > start;
	qi::rule<Iterator, std::string(), Skipper > identifier;
	asf::vec3::parser<Iterator> vec3;
	
	qi::rule<Iterator, axis_type(), Skipper > axis;
	qi::rule<Iterator, dof_type(), Skipper > dof;
	qi::rule<Iterator, limits_type(), Skipper > limits;
	
	qi::rule<Iterator, asf::axis() > axis_name;
	qi::rule<Iterator, asf::axis() > small_axis_name;

	qi::rule<Iterator, asf::kind() > small_kind;
	
	parser() : parser::base_type( start ) {
	  using qi::char_;
	  using qi::double_;
	  using qi::uint_;
	  using qi::lit;
	  using namespace qi::labels;
	  
	  axis_name = char_('X') | char_('Y') | char_('Z');

	  small_axis_name = char_('x')[ _val = asf::axis('X')] | char_('y')[ _val = asf::axis('Y') ] | char_('z')[_val = asf::axis('Z') ];
	  small_kind = char_('r')[_val = asf::kind::rotation] | char_('t')[ _val = asf::kind::translation ] ;
	  
	  axis %= vec3 >> (+axis_name);
	  
	  identifier 
	    %= qi::lexeme[ qi::alpha >> *qi::alnum ];

	  dof %= small_kind >> small_axis_name;
	  
	  limits %= char_('(') >> double_ >> double_ >> char_(')');

	  start %= lit("begin") 
	    >> lit("id") >> uint_
	    >> lit("name") >> identifier
	    >> lit("direction") >> vec3 
	    >> lit("length") >> double_
	    >> lit("axis") >> axis
	    >> -( lit("dof") >> +dof )
	    >> -( lit("limits") >> +limits )
	    >> lit("end");
	    
	}

      };
    };
    
    
  }
}

BOOST_FUSION_ADAPT_STRUCT(
			  mocap::asf::bone::axis_type,
			  (mocap::asf::vec3, offset)
			  (std::vector<mocap::asf::axis>, order)
			  )

BOOST_FUSION_ADAPT_STRUCT(
			  mocap::asf::bone::dof_type,
			  (mocap::asf::kind, kind)
			  (mocap::asf::axis, axis)
			  )

BOOST_FUSION_ADAPT_STRUCT(
			  mocap::asf::bone::limits_type,
			  (mocap::asf::real, lower)
			  (mocap::asf::real, upper)
			  )


BOOST_FUSION_ADAPT_STRUCT(
			  mocap::asf::bone,
			  (mocap::asf::bone::id_type, id)
			  (std::string, name)
			  (mocap::asf::vec3, direction)
			  (mocap::asf::real, length)
			  (mocap::asf::bone::axis_type, axis)
			  (std::vector< mocap::asf::bone::dof_type >, dof)
			  (std::vector< mocap::asf::bone::limits_type >, limits)
			  )


#endif
