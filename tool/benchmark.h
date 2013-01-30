#ifndef TOOL_BENCHMARK_H
#define TOOL_BENCHMARK_H

#include <map>
#include <string>

#include <phys/solver/any.h>
#include <core/callback.h>

namespace tool {

  namespace bench {


    typedef std::map<std::string, 
		     phys::solver::any> solver_set;
    
    typedef std::function< void(const phys::solver::task& task) > solver_type;
    
    solver_type solver(const std::string& name, const solver_set& set, core::callback after = core::callback());
    

  }

}



#endif
