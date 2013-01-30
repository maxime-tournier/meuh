#include "to_lua.h"

#include <core/dfs.h>

#include <core/log.h>

#include <phys/rigid/relative.h>

namespace tool {

  namespace impl {
    
    std::string to_lua();
    
    template<class X, class ... Args>
    std::string to_lua(const std::string& name, const X& x,
		       Args&& ... args);
    
  }


 
  
  
  std::string to_lua(const std::string& s) {
    return "[[" + s + "]]";
  }

  std::string to_lua(const math::real& x) {
    return core::string(x);
  }
  
  
  struct raw {
    std::string value;
  };

  std::string to_lua(const raw& x) { return x.value; }


 

  template<class X, class ... Args>
  std::string to_lua(const std::string& name, const X& x,
		     Args&& ... args) {
    return "{ " + impl::to_lua(name, x, std::forward<Args>(args)...) + " } ";
  }
  
  
  std::string to_lua(const math::quat& q) {
    return to_lua("w", q.w(),
		  "x", q.x(),
		  "y", q.y(),
		  "z", q.z() ); 
  }


  std::string to_lua(const math::SO3& q) {
    return to_lua( q.quaternion() );
  }


  std::string to_lua(const math::vec3& v) {
    return to_lua("x", v.x(),
		  "y", v.y(),
		  "z", v.z() ); 
  }

  std::string to_lua(const math::SE3& g) {
    return to_lua("pos", g.translation(),
		  "orient", g.rotation());
  }
  
  std::string to_lua(const phys::rigid::frame& f) {
    return to_lua("id", f.index,
		  "offset", f.coords);
  }
  
  

  namespace impl {

    std::string to_lua() { return {}; }

    template<class X, class ... Args>
    std::string to_lua(const std::string& name, const X& x,
		       Args&& ... args) {
    
      return name + " = " + tool::to_lua(x) + ", " + impl::to_lua( std::forward<Args>(args)... );
      
    }
    
  }

 
  std::string to_lua(const phys::rigid::skeleton& s,
		     const phys::rigid::config& c) {

    std::string body, joint, rest;
    
    
    body += "{";

    s.each([&](math::natural i) {
	
	body += to_lua("id", i,
		       "name", s.body(i).name,
		       "config", c(i));
	body += ", ";
	
      });

    body += "}";
    

    joint += "{";
    rest += "{";
    
    auto rel = phys::rigid::relative(s)(c);
  
    core::each( boost::edges( s.topology ),
		[&](phys::rigid::skeleton::topology_type::edge_descriptor e) {
		    
		  const auto& j = s.joint( e );
		  
		  joint += to_lua("first", j.first,
				  "second", j.second);

		  // the ordering described by edges (first, second)
		  // may not be the one computed using DFS (when
		  // computing relative configurations), so we must
		  // inverse relative configuration when needed

		  auto target = boost::target(e, s.topology);
		  auto source = boost::source(e, s.topology);
		  
		  // TODO figure out why this does not work (the contrary does :-/ )
		  bool ok = (target != s.root) && (s.parent(target) == j.first.index);

		  // core::log(target, "ok:", ok, "root", s.root);
		  rest += to_lua( !ok ? rel( target ) : rel( target ).inverse() );
		  
		  joint += ", ";		    
		  rest += ", ";		    
		});
    
    joint += "}";
    rest += "}";

   
    
    return to_lua("body", raw{body},
		  "joint", raw{joint},
		  "rest", raw{rest});
    
  }



}
