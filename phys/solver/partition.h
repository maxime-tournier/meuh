#ifndef PHYS_SOLVER_PARTITION_H
#define PHYS_SOLVER_PARTITION_H

#include <sparse/merge.h>
#include <sparse/split.h>

#include <core/each.h>

#include <phys/solver/result.h>

namespace phys {

  namespace solver {
    
  
    template<class S1, class S2>
    struct partition {

      const std::set<const phys::dof*> dofs1;
      
      const S1 first;	
      const S2 second;
      
      partition(const std::set<const phys::dof*>& dofs1,
		const S1& s1, const S2& s2) : dofs1(dofs1),
					      first(s1), second(s2) { 
	
      }

      template<class Range>
      static std::set<const phys::dof*> make_dofs(Range range) { 
	std::set<const phys::dof* > res;

	auto it = res.end();
	core::each(range, [&](const phys::dof* d) {
	    it = res.insert( it, d);
	  });
	
	return res;
      }


      template<class Range>
      partition(Range range, const S1& s1, const S2& s2) 
	: dofs1(make_dofs(range)),
	  first(s1),
	  second(s2) {
	
      }
      
      dof::velocity operator()(const dof::momentum& f, const result& res = result()) const {
	auto split = sparse::split(f, dofs1);
	return sparse::merge( first( split.first, res), 
			      second( split.second, res) );
      }
      
    };
  
    
  }

}


#endif
