#include "string.h"

#include <core/use.h>

namespace core {

  namespace impl {
     
    void arg(std::string& res, size_t start, const std::string& pattern) {
      assert( res.find(pattern, start) == std::string::npos );
      core::use(res, start, pattern);
    }

  }


  std::string indent(unsigned int level) { return {"\t", level}; }
  
}
