#ifndef CORE_CURIOUS_H
#define CORE_CURIOUS_H

namespace core {

  template<class Derived>
  class curious {
  protected:
    Derived& derived() { return static_cast<Derived&>(*this); }
    const Derived& derived() const { return static_cast<Derived&>(*this); }
  };

}


#endif
