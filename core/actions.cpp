#include "actions.h"

namespace core {

  std::string hook(const std::string& name) { return std::string("hook_") + name; }
  bool is_hook(const std::string& name) { return name.substr(0, 5) == "hook_"; }
  

}
