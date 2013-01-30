#include "console.h"

#include <iostream>


#include <signal.h>

#include <core/log.h>

#include <script/meuh.h>

#include <core/sig.h>
#include <core/callback.h>
#include <script/api.h>

#include <script/readline.h>
#include <future>
#include <core/debug.h>

namespace script {
 

  void console::operator()() {
    
    core::log("console starting");
    
    readline::init();

    try {
      while( true ) {
	exec( readline::get("> ") );
      }
    }
    catch( readline::eof ) {
      sema.notify();
    }
    
    core::log("console exiting");
  }
  
  void console::exit() {
    readline::abort();
    sema.wait();
  }
  
}
