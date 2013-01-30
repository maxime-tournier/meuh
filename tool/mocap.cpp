#include "mocap.h"

#include <mocap/bvh.h>
#include <mocap/bvh/file.h>

#include <tool/bio/nasa.h>
#include <mocap/offset.h>

#include <math/func/compose.h>
#include <math/func/ref.h>

#include <phys/rigid/relative.h>

namespace tool {

  static bio::nasa make_info( ::mocap::skeleton* skel ) {
    
    bio::nasa::real_map lengths;
    skel->each([&](::mocap::skeleton* j) {
	lengths[ j->get().name ] = ::mocap::length(j);
      });

    return bio::nasa(lengths);
  }


  mocap::mocap(const std::string& filename)
    : filename(filename),
      clip(::mocap::bvh::clip( ::mocap::bvh::file(filename) ) ),
      curves(clip), 
      adaptor(clip.skeleton.get(), make_info( clip.skeleton.get() ) ) {

  }
  
  const phys::rigid::skeleton& mocap::skeleton() const { return adaptor.skeleton; }
  
  
  mocap::curve_type mocap::absolute() const {
    using namespace math::func;
    
    return ref(adaptor) << curves.global();
  }

  mocap::curve_type mocap::relative() const {
    using namespace math::func;
    return phys::rigid::relative{skeleton()} << ref(adaptor) << curves.global();
  }


}
