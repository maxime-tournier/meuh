
#include <iostream>


#include <math/vec.h> 
#include <math/SO3.h>
#include <math/SE3.h> 
#include <math/quaternion.h>  
#include <math/dual_quaternion.h> 
#include <math/matrix.h>

#include <math/lie.h>

#include <math/func/any.h>
#include <math/func/lie.h>
#include <math/func/polynomial.h>
#include <math/func/constant.h>
#include <math/func/euclid.h>
#include <math/func/hermite.h>
#include <math/func/spline.h>
#include <math/func/interp.h>

int main() {
   
  using namespace math;

  vec3 a, b; 
  Euclid<vec3> E;

  a = E.zero();
  b = E.sum( a, b );

  Lie<SO3> G;

  SO3 g = G.id();
  
  func::any< SO3, SO3 > f;
  func::id< real > x;

  func::any<real, real> poly = func::val(1.0) + (x ^ 2);

  std::cout << poly(0) << "  " << poly(1) << std::endl;

  vec zob(10); 

  Euclid<vec> Rn(10);
  
  zob = Rn.zero();

  typedef std::map<real, SO3> nodes;
  nodes p;

  p[0] = SO3::identity();
  p[1] = SO3::identity();
  p[2] = SO3::identity();
  p[3] = SO3::identity(); 

  auto mitch = std::make_tuple(g, g, g, g);
  
  func::spline::coeffs::catmull_rom<> test;
  func::spline::patch<SO3, func::spline::coeffs::catmull_rom<> > testuz(mitch, test);

  SO3 bob = func::curve(p)( 0.0 );

  // todo test splines
}
 
 
