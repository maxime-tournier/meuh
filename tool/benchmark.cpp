#include "benchmark.h"


#include <script/meuh.h>
#include <core/stl.h>

#include <core/debug.h>

namespace tool {
  namespace bench {

  
    solver_type solver(const std::string& name, const solver_set& set, 
		       core::callback after) {
      script::require("plot");

      return [&] (const phys::solver::task& task) {
	script::exec("%% = plot.new()", name);
	script::exec("%%.set.logscale = 'y'", name);
	
	core::each(set, [&](const std::string& sname, const phys::solver::any& solver) {
	    
	    core::log("solver", sname);
	    script::exec("table.insert(%%.title, '%%')", name, sname );
	    solver.solve( task );
	    
	    if(after) after();
	  });
	
      };
      
    }
    
  }
}
