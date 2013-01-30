#include "cg.h"

namespace math {
  namespace impl {

    void cg::init(const vec& rr) {
      r = rr;
      p = r;
      phi2 = r.squaredNorm();
      phi = std::sqrt( phi2 );
      k = 1;
    }

  }
}
