#include "nan.h"

#include <cmath>

namespace math {

  bool nan(const real& x) { return std::isnan(x); }

}
