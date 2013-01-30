#ifndef PHYS_SOLVER_RESULT_H
#define PHYS_SOLVER_RESULT_H

#include <phys/constraint/bilateral.h>

namespace phys {
  namespace solver {

    struct result {

      result();
      
      typedef constraint::bilateral::vector lambda_type;
      lambda_type* lambda;
      
      void update_lambda(lambda_type ) const;
      
    };


  }
}


#endif
