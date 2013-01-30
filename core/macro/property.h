#ifndef CORE_MACRO_PROPERTY_H
#define CORE_MACRO_PROPERTY_H

#include <core/macro/self.h>

#define macro_property(type, name)					\
  private: type name##_;  public: const type& name () const { return name##_; }	 self& name ( const type& value ) { name##_ = value; return *this; } 
 
#endif
