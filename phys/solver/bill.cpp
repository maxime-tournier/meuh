#include "bill.h"

#include <sparse/keep.h>
#include <phys/system.h>

namespace phys {
  namespace solver {
    
    // template<class C>
    // static std::set<C> sort(const std::vector<C>& data) {
    //   return std::set<C>(data.begin(), data.end());
    // }
    
    
    // bill::bill(const phys::system& system,
    // 	       math::real epsilon)	
    //   : full( ref(linear), 
    // 	      sparse::keep(system.constraint.bilateral.matrix,
    // 			   sort(linear.setup(system)) ),
    // 	      epsilon) {
      
    // }

    // bill::bill(const phys::system& system,
    // 	       math::real epsilon,
    // 	       const constraint::bilateral::weight& w)	
    //   : full( ref(linear), 
    // 	      sparse::keep(system.constraint.bilateral.matrix,
    // 			   sort(linear.setup(system, linear.mst(w))) ),
    // 	      epsilon) {
      
    // }
    
    
    // void bill::solve(const solver::task& task) const {
    //   full.solve(task);
    // }


    // bill::bill(bill&& other) 
    //   : linear(std::move(other.linear)),
    // 	full( ref(linear),
    // 	      other.full.matrix,
    // 	      other.full.epsilon)  {
      
    // }	      
    
  }
} 
