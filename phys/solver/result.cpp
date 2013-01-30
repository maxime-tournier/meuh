#include "result.h"

#include <math/vec.h>

namespace phys {
  namespace solver {

    result::result() : lambda(0) { }

    void result::update_lambda(lambda_type data) const {
      if(!lambda) return;

      data.insert(lambda->begin(), lambda->end());
      *lambda = data;
    }

  }
}
