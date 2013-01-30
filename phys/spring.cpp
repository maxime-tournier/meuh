#include "spring.h"

#include <math/random.h>

#include <Eigen/Geometry>


namespace phys {
  
  using namespace math;

  spring::spring(real stiffness, real rest)
    : stiffness( stiffness ),
      rest( rest )  {
    assert( stiffness > 0 );
    assert( rest >= 0 );
  }


  real spring::V(const vec3& first, const vec3& second) const {
    const real el = elongation(first, second);
    return stiffness / 2 * el * el;
  }
  

  vec6 spring::dV(const vec3& first, const vec3& second) const {
    const vec3 diff = first - second;
    const real theta = diff.norm();
    
    // undefined gradient
    if( theta < math::epsilon ) return vec6::Zero();
    
    const real elongation = theta - rest;
    
    const vec3 u = diff / theta;
    
    vec6 res;
    res.head<3>() =  stiffness * elongation * u;
    res.tail<3>() = - stiffness * elongation * u;
    
    return res;
  }

  
  mat66 spring::d2V(const vec3& first, const vec3& second) const {
    const vec3 diff = first - second;
    const real theta = diff.norm();
    
    const real elongation = theta - rest;
    
    const vec3 u = diff / theta;

    mat36 J;

    J.leftCols<3>() = mat33::Identity();
    J.rightCols<3>() = - mat33::Identity();

    mat66 res;
    const real alpha = rest / theta;
    
    res = (stiffness * stiffness) * J.transpose() * ( mat33::Identity() * (1 - alpha) + alpha * u * u.transpose() ) * J;
    
    return res;
  }

  auto rand = math::random< vec3 >::uniform();

  void spring::JTDJ(mat36& J, vec3& D, const vec3& first, const vec3& second) const {
    const vec3 diff = first - second;
    const real theta = diff.norm();
    
    const real elongation = theta - rest;
    
    const vec3 u = diff / theta;
    const real alpha = elongation / theta;
   
    vec3 tmp = rand();
    
    const vec3 v = (tmp - u * u.dot(tmp)).normalized();
    const vec3 w = u.cross(v);

    mat33 Q;
    Q << u, v, w;
    
    real stiff_root = std::sqrt( stiffness );
    
    // set result
    J << Q.transpose(), - Q.transpose();
    D = stiffness * vec3(1, alpha, alpha);
    
  }

 
 
  real spring::elongation(const vec3& first, const vec3& second) const {
    const vec3 diff = first - second;
    const real theta = diff.norm();

    return theta - rest;
  }

};
