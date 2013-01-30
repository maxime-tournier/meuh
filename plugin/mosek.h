#ifndef PLUGIN_MOSEK_H
#define PLUGIN_MOSEK_H

#include <map>

#include <math/types.h>
#include <math/vec.h>
#include <math/mosek.h>

#include <plugin/base.h>
#include <core/exception.h>

namespace plugin {

  struct mosek : math::mosek, plugin::base {
    
  };

}



#endif
