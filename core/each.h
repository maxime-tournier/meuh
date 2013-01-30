#ifndef CORE_EACH_H
#define CORE_EACH_H

// TODO include range for concept check

#include <core/edit.h>
#include <core/range.h>

namespace core {
  
  template<class Range, class Action>
  void each(Range r, const Action& action) {
    for(; !range::empty(r); range::pop(r)) {
      edit(range::get(r))( action );
    }
  }
  
}


#endif
