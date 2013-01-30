
#include <phys/lagrange.h>
#include <phys/solver/graph.h>

#include <math/vec.h>

#include <phys/sparse/base.h>
#include <phys/system/dof.h>

using namespace math;
using namespace phys;


static const vec3 gravity(0, -9.8, 0);

// struct particle : lagrange::base<vec3> {
  
//   const real mass;
  
//   range operator()(const domain& q ) const {
//     return mass * ( euclid::dot(q.at(), gravity) + 0.5 * euclid::squared_norm(q.body()) );
//   }
  
// };


// namespace lagrange {
  
//   template<>
//   struct map< particle > {
    
//     struct potential : function::base< T<vec3>, {
      
      
      
//     };

//   };


// }


int main() {

 
  // auto start = lagrange::start<vec3>();
  
  // auto q =  function::get<0>( start );
  // auto v =  function::get<1>( start );

  // auto potential = function::dot_with( ) << q;
  // auto kinetic = function::squared_norm<vec3>() << v;

  // auto lagrange = potential + kinetic;
  
}
