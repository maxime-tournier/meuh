#include "numbering.h"

#include <sparse/keys.h>
#include <control/system.h>

#include <math/matrix.h>

namespace control {
  namespace solver {


    numbering::numbering(const control::system& system) 
      : actuator( sparse::keys( system.actuator.matrix ) ),
	feature( sparse::keys( system.feature.matrix) ) {
      
    }


  }
}
