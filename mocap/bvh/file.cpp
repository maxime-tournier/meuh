#include "file.h"

#include <core/io.h>

namespace mocap {

  namespace bvh {
    
    file::file(const std::string& filename) {
      std::string storage = core::io::load::ascii(filename);
      parse( storage.begin(), storage.end() );
    }

  }
}
