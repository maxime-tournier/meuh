#ifndef CONTROL_SOLVER_NUMBERING_H
#define CONTROL_SOLVER_NUMBERING_H



#include <control/actuator.h>
#include <control/feature.h>

#include <sparse/linear/numbering.h>

namespace control {

  struct system;

  namespace solver {

    struct numbering {
      
      numbering(const system& );

      const sparse::linear::numbering< control::actuator > actuator;
      const sparse::linear::numbering< control::feature > feature;

    };
    

  }

}


#endif
