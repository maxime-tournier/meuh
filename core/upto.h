#ifndef CORE_UPTO_H
#define CORE_UPTO_H

namespace core {

  template<class Action>
  void upto(unsigned int n, const Action& act) {
    for(unsigned int i = 0; i < n; ++i) {
      act(i);
    }
  }
  
}


#endif
