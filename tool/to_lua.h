#ifndef TOOL_EXPORT_H
#define TOOL_EXPORT_H

#include <string>
#include <phys/rigid/skeleton.h>

namespace tool {
  
  // misc lua exports
  
  std::string to_lua(const std::string& s);
  std::string to_lua(const math::real& x);

  std::string to_lua(const math::quat& q);
  std::string to_lua(const math::SO3& q);
  std::string to_lua(const math::SE3& g);
  
  // std::string to_lua(const phys::rigid::skeleton& skel);
  
  std::string to_lua(const phys::rigid::skeleton& skel,
		     const phys::rigid::config& conf);
  
  
  


}



#endif
