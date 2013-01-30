#ifndef SCRIPT_COMMAND_H
#define SCRIPT_COMMAND_H

#include <core/string.h>

namespace script {

  // small helpers to build script commands
  std::string cmd() { return std::string(); }

  template<class ... Args>
  std::string cmd(const std::string& head, Args&& ... args) {
    return head + cmd(std::forward<Args>(args)...);
  };

  template<class Head, class ... Args>
  std::string cmd(const Head& head, Args&& ... args) {
    return core::string(head) + cmd(std::forward<Args>(args)...);
  };
  
  template<class ... Args>
  std::string str(Args&&...args) {
    return std::string("'") + cmd(std::forward<Args>(args)...) + std::string("'");
  }
  
}



#endif
