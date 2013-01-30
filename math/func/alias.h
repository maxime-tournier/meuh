#ifndef MATH_FUNC_ALIAS_H
#define MATH_FUNC_ALIAS_H

#include <core/call.h>

#define macro_func_alias(child, ...)					\
  typedef __VA_ARGS__ parent;						\
  									\
  typename parent::range operator()(const typename parent::domain& x)	\
    const { return parent::operator()(x); }				\
    									\
    math::T<typename parent::range> diff(const math::T<typename parent::domain>& dx) const { \
      return parent::diff(dx); }					\
    									\
    math::coT<typename parent::domain> diffT(const math::pullT<child>& fx) const { \
      return parent::diffT( pullT<parent>(*this, fx.at(), fx.body())); \
    }									\
    

#endif
