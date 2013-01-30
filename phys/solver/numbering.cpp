#include "numbering.h"

#include <sparse/keys.h>
#include <phys/system.h>

#include <math/matrix.h>

namespace phys {
  namespace solver {

    numbering::numbering(const phys::system& system) 
      : dofs( sparse::keys(system.mass) ),
	bilateral( sparse::keys(system.constraint.bilateral.matrix) ),
	unilateral( sparse::keys(system.constraint.unilateral.matrix) ),
	friction(  sparse::keys(system.constraint.friction.matrix) ) {
    }

  }
}
