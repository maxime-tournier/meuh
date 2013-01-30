#ifndef TOOL_TRAJECTORY_H
#define TOOL_TRAJECTORY_H

#include <math/types.h>
#include <math/vec.h>
#include <math/form.h>
#include <map>
#include <vector>
#include <list>


#include <gui/frame.h>
#include <set>

namespace tool {

  namespace impl {
    struct trajectory {
      typedef std::map<math::real, math::vec3> data_type;
      data_type data;
    
      typedef std::vector< data_type > pyramid_type;
      pyramid_type pyramid;

      static pyramid_type make_lods(data_type data);
    
      static math::vec3 value(const pyramid_type& pyramid, 
			      const math::real& t,
			      unsigned int levels = -1);
    
    
      struct editor {
      
	struct point {
	  math::vec3& edited;
	  const math::vec3 init;
	  const math::real ratio;
	};

	std::list<point> points;

	static std::list<point> make_points(trajectory& traj, math::real t, math::natural level);
      
      public:
	editor(trajectory& traj, math::real t, math::natural level);
      
	void reset() const;
	void delta(const math::vec3& x) const;
      };

    };
  }


  class trajectory {
    impl::trajectory traj;
  public:
    math::natural level;
    
    trajectory();

    void data( const impl::trajectory::data_type& );
    const impl::trajectory::data_type& data() const;

    math::natural lods() const;
    
    impl::trajectory::editor edit(math::real t, math::natural level);
    
    typedef math::real domain;
    typedef math::vec3 range;
    
    range operator()(const domain& ) const;

    void gui_edit(std::set<gui::frame*>& frames,
		  math::real t, math::natural level);

    void draw() const;
  };


}



#endif
