#include "markers.h"


#include <math/func/index.h>
#include <math/func/vector.h>
#include <math/func/SE3.h>

#include <math/func/compose.h> 

namespace tool {

  //  using namespace math;

  //   Lie< markers::domain > markers::dmn() const {
  //     return lie;
  //   }


  //   Lie< markers::range > markers::rng() const {
  //     return Lie<range>(handles.size());
  //   }
  
  //   static func::vector< func::absolute<> > points( const std::map<natural, vec3>& handles) {
  //     assert(!handles.empty() );
      
  //     func::vector< func::absolute<> > res;
  //     res.resize(handles.size());

  //     natural j = 0;
  //     core::each(handles, [&](natural, const vec3& local) {
  // 	  res(j++).local = local;
  // 	});
      
  //     return std::move(res);
  //   }
    
  //   static std::set<natural> indices(const std::map<natural, vec3>& handles) {
  //     std::set<natural> res;

  //     core::each(handles, [&](natural i, core::unused ) {
  // 	  res.insert(i);
  // 	});
      
  //     return res;
  //   }


  //   markers::range markers::operator()(const domain& g) const {
      
  //     using math::func::operator<<;
  //     return ( points(handles) <<  func::index::select< SE3 >( indices(handles) ))(g);
  //   }

    
  //   math::T<markers::range> markers::operator()(const math::T<domain>& dg) const {
      
  //     using math::func::operator<<;
  //     return ( points(handles) <<  func::index::select< SE3 >( indices(handles) ))(dg);
  //   } 
  
  // markers::markers(const math::Lie< domain >& lie) : lie(lie) { }
  

}
