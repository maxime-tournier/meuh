#include "history.h"
#include <readline/history.h>

#include <script/meuh.h>
#include <core/log.h>

namespace script {

  
  history::history()
    : api("history") {

    api
      .fun("clear", [&] {
	  clear_history();
	})
      .fun("load", [&](const std::string& filename) {
	  read_history( filename.c_str() );
	})
      .fun("save", [&](const std::string& filename) {
	  write_history( filename.c_str() );
	});
    
  }

  
  void history::add(const std::string& line) {
    add_history(line.c_str());
  }


}
