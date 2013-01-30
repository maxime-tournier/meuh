#include "file.h"

#include <core/io.h>

namespace geo {
  namespace obj {

    file::file(const std::string& filename) {
      const std::string storage = core::io::load::ascii( filename );
      auto it = storage.begin();
      parse(it, storage.end());
      
    }

  }
}
