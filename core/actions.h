#ifndef CORE_ACTIONS_H
#define CORE_ACTIONS_H

#include <core/callback.h>
#include <string>
#include <map>


namespace core {

  typedef std::map< std::string, callback > actions;
 
  std::string hook(const std::string& name);
  bool is_hook(const std::string& name);
}


#endif
