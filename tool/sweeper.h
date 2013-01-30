#ifndef TOOL_SWEEPER_H
#define TOOL_SWEEPER_H

#include <math/types.h>

#include <math/vec.h>
#include <math/positive.h>

#include <math/func/tuple.h>
#include <math/func/exp.h>
#include <math/func/euclid.h>

namespace tool {
  
  struct sweeper {
    typedef std::tuple< math::vec3, math::vec3 > dof_type;
    
    typedef std::tuple<dof_type, math::vec3> domain;
    typedef math::vec3 range;
    
    range operator()(const domain& ) const;
    
    struct push {
      const domain at;

      push(const sweeper&, const domain& );
      math::Lie<range>::algebra operator()(const math::Lie<domain>::algebra& ) const;
    };

  };
  
} 


#endif
