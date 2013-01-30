#ifndef CONTROL_FEATURE_H
#define CONTROL_FEATURE_H

#include <map>
#include <math/types.h>

#include <phys/dof.h>

namespace control {

  struct feature {
    const math::natural dim;
    
    feature( math::natural n );

    typedef const feature* key;
    typedef std::map<key, math::vec> vector;
        
    // dof
    typedef std::map<phys::dof::key, math::mat> row;
    typedef std::map<key, row> matrix;

  };


}


namespace sparse {
  namespace linear {

    math::natural dim(control::feature::key f);
    
  }
}

#endif
