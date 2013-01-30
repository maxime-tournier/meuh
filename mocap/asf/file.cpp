#include "file.h"

#include <core/log.h>
#include <core/stl.h>


namespace mocap {
  namespace asf {

    void file::debug() const {
      core::log("version:", version);
      core::log("name:", name);

      units.debug();
      core::log("documentation:", documentation);
      root.debug();

      core::log("bonedata:");
      core::each(bonedata, [&](const asf::bone& b) {
	  b.debug();
	});
      
      hierarchy.debug();
    }

  }
}
