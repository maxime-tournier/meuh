#include "iter.h"

namespace math {

  iter::iter(natural bound, real epsilon)
    : bound(bound), epsilon(epsilon) { }


  namespace impl {
    wrap<real> operator,(real value, wrap<real> ) {
      return {value};
    }

  }

}
