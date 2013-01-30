#include "trajectory.h"

#include <math/func/vector.h>
#include <core/stl.h>
#include <math/func/interp.h>

#include <algorithm>


#include <core/string.h>
#include <gl/common.h>

namespace tool {

 
  namespace impl {
 
  
    template<class Key, class Data>
    static Data interp_linear(const std::map<Key, Data>& map, math::real t) {
      const auto nodes = math::func::interp::closest(map, t);

      auto fun = [&](math::real x) -> Data {
	math::real alpha = (x - nodes(0).first) / (nodes(1).first - nodes(0).first);
      
	return (1 - alpha) * nodes(0).second + alpha * nodes(1).second;
      };
    
      auto res = fun(t);
      // core::log("interp result:", res.transpose());
      return res;
    }
  

    static const auto interp = [](const trajectory::data_type& nodes ) {
      // return [&](math::real t) { return interp_linear(nodes, t); };
      return [&](math::real  t) { return math::func::curve(nodes)(t); };
    };
  

    trajectory::pyramid_type trajectory::make_lods(data_type data) {
      // return tool::make_lods( tool::make_pyramid( data ) );


      trajectory::pyramid_type res;

      while(data.size() > 2 ) {
	data_type high, details;
      
	std::vector< trajectory::data_type::iterator > to_erase;
     
	bool odd = false;		// keep first
	for(auto it = data.begin(), end = data.end(); it != end; ++it) {
	  // subsample
	  if( odd ) {
	    high[it->first] = it->second;	  
	    to_erase.push_back(it);
	  }
	  odd = !odd;
	};
      
	core::each(to_erase, [&](trajectory::data_type::iterator it) {
	    data.erase(it);
	  });
      
	// prediction
	core::each(high, [&](math::real t, const math::vec3& value) {
	    details[t] = value - interp(data)(t);
	  });
      
	res.push_back(details);
      
      };
      res.push_back( data );
    
      std::reverse( res.begin(), res.end() );

      return res;

    }

    math::vec3 trajectory::value(const pyramid_type& pyramid,
				 const math::real& t,
				 unsigned int levels) {
      // core::log("reconstructing...");
      data_type nodes = pyramid.front();
      
      levels = std::min<unsigned int>( pyramid.size(), levels );
      
      for(unsigned int i = 0; i < levels - 1; ++i) {

	int next = i + 1;
      
	auto clos = math::func::interp::closest(pyramid[next], t, 4);
      
	data_type pred;
      
	clos.each([&](math::natural i) {
	    pred[ clos(i).first ] = interp(nodes)(clos(i).first) + clos(i).second;
	  });
      
	nodes.insert(pred.begin(), pred.end());
      
	// remove unneeded nodes in closest
	if(nodes.size() > 2) {
	  clos = math::func::interp::closest(nodes, t, 4);
	
	  nodes.clear();
	  auto it = nodes.begin();
	  clos.each([&](math::natural i) {
	      it = nodes.insert(it, clos(i));
	    });

	}

	// if(pred.find(t) != pred.end()) break;
      }
    
    
      return interp(nodes)(t);
    
    }
  

    std::list< trajectory::editor::point > trajectory::editor::make_points(trajectory& traj, 
									   math::real t, math::natural level) {
      std::list< point > res;

      auto clos = math::func::interp::closest(traj.pyramid[level], t, 2);
    
      math::natural index = std::abs(t - clos(0).first) < std::abs(t - clos(1).first) ? 0 : 1;
    
      math::real pos = std::abs(t - clos(index).first) / std::abs( clos(1).first - clos(0).first);
      
      // magic: get the spline coefficient associated to the closest
      // point
      auto tmp = math::func::spline::coeffs::catmull_rom<>()(pos);

      math::real ratio = std::get<1>(tmp);
      

      core::log("ratio:", ratio);

      // if closest moves by delta, wanted point will move by (1 - ratio) *
      // delta
      auto& zob = traj.pyramid[level][clos(index).first];
      point p{ zob, zob, 1/ratio };
      res.push_back(p);
    
      // clos.each([&](math::natural i) {
	
      // 	auto it = traj.pyramid[level].find(clos(i).first);

      // 	if( it != traj.pyramid[level].end()) {
      // 	  point p{ it->second, it->second };
      // 	  res.push_back(p);
      // 	} else {
      // 	  // add a zero detail
      // 	  auto& delta = traj.pyramid[level][clos(i).first];
      // 	  delta = math::vec3::Zero();
	  
      // 	  point p{delta, math::vec3::Zero() };
      // 	  res.push_back(p);
      // 	}
      //   });
    
      return res;
    
    }
  
    trajectory::editor::editor(trajectory& traj, math::real t, math::natural level)
      : points( make_points(traj, t, level) ) {
    
    }
  
    void trajectory::editor::reset() const { 
      delta(math::vec3::Zero());
    }
  
    void trajectory::editor::delta(const math::vec3& d) const {
      core::each( points, [&](const point& p) {
	  p.edited = p.init + p.ratio * d; 
	});
    }
  }


  void trajectory::data(const impl::trajectory::data_type& d) {
    traj.data = d;
    traj.pyramid = impl::trajectory::make_lods(d);
    level = traj.pyramid.size() - 1;
  }

  
  impl::trajectory::editor trajectory::edit(math::real t, math::natural lev) {
    level = std::min<math::natural>(level, traj.pyramid.size() - 1);
    
    return {traj, t, lev};
  }
  
  trajectory::range trajectory::operator()(const domain& t) const {
    return impl::trajectory::value(traj.pyramid, t, level);
  }

  trajectory::trajectory() : level(-1) { }

  math::natural trajectory::lods() const { return traj.pyramid.size(); }

  const impl::trajectory::data_type& trajectory::data() const { return traj.data; }
 



  void trajectory::draw() const {
    using namespace gl;
    begin(GL_LINE_STRIP, [this] {
	core::each( traj.data, [&](math::real t, const math::vec3& v) {
	    vertex( operator()(t) );
	  });
      });
  }


  void trajectory::gui_edit(std::set<gui::frame*>& frames,
			    math::real t, math::natural lev) {

    auto f = new gui::frame;

    const math::vec3 at = operator()(t);
  
    f->transform( math::SE3::translation(at) );
    frames.insert(f);
    
    auto ed = edit(t, lev);
  
    f->on_manipulated.action =  [&, f, at, ed]{
      const math::vec3 delta = f->transform().translation() - at;
      ed.delta( delta );
    };

    auto undo = [f, ed, at] {
      f->transform( math::SE3::translation(at) );
      ed.reset();		// useless actually
    };

    
    auto remove = [this, f, &frames, undo] {
      undo();
      
      f->release();
      frames.erase(f);
      
    }; 


  
    f->menu.title = std::string("Level ") + core::string(lev);
    f->menu.actions["Remove"] = remove;
    
    f->menu.actions["Undo"] = undo;
    
    if( lev < lods() - 1) {
      f->menu.actions["Finer"] = [this, remove, f, t, lev, &frames] {
	remove();
	gui_edit(frames, t, lev+1);
      };
    }

    if( lev > 1) {
      f->menu.actions["Coarser"] = [this, remove, f, t, lev, &frames] {
	remove();
	gui_edit(frames, t, lev-1);
      };
    }

  }
 
}
