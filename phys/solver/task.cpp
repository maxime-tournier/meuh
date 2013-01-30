#include <sparse/vector.h>

#include "task.h"

#include <math/vec.h>


namespace phys {
  namespace solver {

    task::task(dof::velocity& result) 
      : velocity(result), 
	lambda(0),
	mu(0)
    { }
    
  }
}
