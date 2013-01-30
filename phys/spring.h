#ifndef PHYS_SPRING_H
#define PHYS_SPRING_H

#include <math/vec.h>
#include <math/mat.h>

namespace phys {

  struct spring {

    // TODO encapsulate
    math::real stiffness;
    math::real rest;

    spring(math::real stiffness = 1, math::real rest = 0);
    
    // signed distance
    math::real elongation(const math::vec3& first, const math::vec3& second) const;

    // potential energy
    math::real V(const math::vec3& first, const math::vec3& second) const;

    // energy gradient
    math::vec6 dV(const math::vec3& first, const math::vec3& second) const;
    
    // energy hessian matrix
    math::mat66 d2V(const math::vec3& first, const math::vec3& second) const;
    
    // energy hessian decomposition
    void JTDJ(math::mat36& J, math::vec3& D, const math::vec3& first, const math::vec3& second) const;

  };

}


#endif
