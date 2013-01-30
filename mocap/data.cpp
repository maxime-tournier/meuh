#include "data.h"

#include <mocap/error.h>
#include <core/stl.h>

namespace mocap {
  
  time start(const data& data) {
    if(data.empty()) throw error("empty data");
    return data.begin()->first;
  }


  time end(const data& data) {
    if(data.empty()) throw error("empty data");
    return data.rbegin()->first;
  }


  time length(const data& data) {
    return end(data) - start(data);
  }

  
  math::vector<pose> raw(const data& data) {
    using namespace math;
    
    math::vector<pose> res;
    res.resize( data.size());

    auto frame = core::all(data);
    res.each([&](natural i) {
	using namespace core::range;
	
	assert(! empty(frame) );

	res(i) = get(frame).second;
	pop(frame);
      });

    return res;
  }


}
