#ifndef SCRIPT_READLINE_H
#define SCRIPT_READLINE_H

#include <string>
#include <script/history.h>

namespace script {


  struct readline {
    static script::history history;
    
    // thrown when readline exits
    struct eof { };

    static void init();
    static std::string get(const std::string& prompt) throw( eof );
    static void abort();
    
  };



}


#endif
