#ifndef TOOL_SYMMETRIC_H
#define TOOL_SYMMETRIC_H


#include <math/types.h>
#include <math/vec.h>
#include <math/so3.h>
#include <math/tuple.h>

namespace tool {

  struct symmetric {
    typedef std::tuple< math::SO3, math::vec2 > dof_type;

    static math::Lie<dof_type> lie();

    typedef std::tuple<dof_type, math::vec3> domain;
    typedef math::vec3 range;

    range operator()(const domain& ) const;
    math::T<range> diff ( const math::T<domain>& dx ) const ;

    struct field {
      const dof_type& at;
      
      typedef math::vec3 domain;
      typedef math::vec3 range;
      
      range operator()(const domain& x) const;
      math::T<range> diff(const math::T<domain>& dx) const;

    };

    struct map {
      const field::domain& at;
      
      typedef dof_type domain;
      typedef field::range range;

      range operator()(const domain& x) const;
      math::T<range> diff(const math::T<domain>& dx) const;
      
    };

    struct weight {
      typedef symmetric::domain domain;
      typedef math::real range;
      
      range operator() ( const domain& x ) const ;
      math::T<range> diff ( const math::T<domain>& dx ) const ;
    };
    

  };


}


#endif
