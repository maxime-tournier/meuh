#ifndef SCRIPT_HISTORY_H
#define SCRIPT_HISTORY_H

#include <script/api.h>

namespace script {

  class history {
    script::api api;
  public:
    history();
    
    void add(const std::string& line);
    
  };

}





#endif
