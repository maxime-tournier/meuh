#include "idealized.h"

#include <math/matrix.h>
#include <core/stl.h>

#include <phys/solver/friction.h>
#include <phys/solver/bilateral.h>

#include <iostream>

namespace phys {

  namespace solver {
    
    constraint::bilateral::matrix make_matrix( const solver::friction& friction) {
      constraint::bilateral::matrix res;
      core::each(friction.pack.matrix, [&](const constraint::friction* f,
					   const constraint::friction::row& row) {
		   res[ new constraint::bilateral( f->dim ) ] = row;
		 });

      return res;
    }


    constraint::bilateral::pack idealized::pack() const { return { matrix, values }; }

    idealized::idealized(const solver::friction& friction, math::real eps ) 
      : friction(friction),
	matrix( make_matrix(friction) ),
	values(),
	brutal(pack(), friction.bilateral, eps ) {
	// si(pack(), friction.bilateral ) {
      
    }
    

    dof::velocity idealized::operator()(const dof::momentum& f) const {
      // std::cout << "solving idealized" << std::endl;
      // std::cout << "idealized values: " << brutal.cache.keys.dense(values).transpose() << std::endl;
      return brutal( f );
      // return si(f);
    }

    idealized::~idealized() {
      core::each(matrix, [&](const constraint::bilateral* d, core::unused) {
	  delete d;
	});
    }
    
  }
}
