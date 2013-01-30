
#include <core/edit.h>
#include <core/pair.h>

#include <core/stl.h>
#include <sparse/matrix.h>

#include "engine.h"

#include <phys/engine.h>
#include <sparse/zero.h>
#include <sparse/zip.h>
#include <phys/error.h>



namespace phys {
  
  namespace op {
    
    add::add(const dof::velocity& v,
	     const dof::momentum& p) : v(v), p(p) { }
    
  }

  dof::momentum engine::momentum(const dof::momentum& f, math::real dt) const {
    
    dof::momentum res(f.keys(), math::vec() );
    pool.apply( op::momentum{res, f, dt} );
    
    return res;
  }
  
  void engine::correct(const dof::velocity& c) const {
    pool.apply( op::correct{c} );
  }
  
  void engine::integrate(const dof::velocity& v, 
			 const dof::momentum& p,
			 math::real dt) const {
    pool.apply( op::integrate{v, p, dt} );
  }

  void engine::set(const dof::velocity& v, 
		   const dof::momentum& p) const {
    pool.apply( op::set{v, p} );
  }

  void engine::add(const dof::velocity& v, 
		   const dof::momentum& p) const {
    pool.apply( op::add{v, p} );
  }


  
  void engine::clear() {
    pool.clear();
  }


}
