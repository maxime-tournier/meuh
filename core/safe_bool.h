#ifndef CORE_SAFE_BOOL_H
#define CORE_SAFE_BOOL_H

namespace core {

  // derive this class in a curious way to get safe boolean
  // conversions

  // taken from http://www.artima.com/cppsource/safebool3.html
  template<class U>
  struct safe_bool {
    typedef void (safe_bool::*bool_type)() const;
    void this_type_does_not_support_comparisons() const {}

    operator bool_type() const {
      return static_cast< const U*>(this)->boolean() ?
	&safe_bool::this_type_does_not_support_comparisons : 0;
    }

  };

  template<class U, class V>
  bool operator==(const safe_bool<U>& lhs, const safe_bool<V>& ) {
    lhs.this_type_does_not_support_comparisons();
    return false;
  };

  template<class U, class V>
  bool operator!=(const safe_bool<U>& lhs, const safe_bool<V>& ) {
    lhs.this_type_does_not_support_comparisons();
    return false;
  };

}

#endif
