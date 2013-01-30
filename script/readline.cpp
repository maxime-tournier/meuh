#include "readline.h"

#include <core/log.h>


#include <readline/readline.h>
#include <script/history.h>

#include <stdexcept>

#include <core/sig.h>
#include <signal.h>

#include <unistd.h>
#include <core/debug.h>
#include <core/use.h>

namespace script {

  script::history readline::history;

  // control pipe
  static int fd[2];

  // modified getc function: we select on two streams (input and
  // control)
  static int get_char(FILE* file) {
    fd_set r;

    FD_ZERO(&r);
    
    FD_SET( fileno(file), &r);
    FD_SET( fd[0], &r);

    // wait till there is input in stream or control
    int ret = -1;
    
    while( ret < 0 ) {
      ret = select(FD_SETSIZE, &r, 0, 0, 0);
    }
    if( (ret > 0) && !( FD_ISSET(fd[0], &r) )) {
      char c;
      read(fileno(file), &c, 1);

      return c;
    }
    return -1;
  }
  

  void readline::init() {
    macro_debug("readline init");
    if( pipe(fd) < 0 ) throw std::logic_error("pipe error");

    rl_getc_function = get_char;

    // manually install signal handler
    rl_catch_signals = false;
    core::push_back(core::sig::handler(SIGINT), [&] {
  	rl_cleanup_after_signal();
      }); 
  }

 

  std::string readline::get(const std::string& prompt) throw( eof ) {
    const char* line = ::readline( prompt.c_str() );
    
    if( line ) history.add(line);
    else {
      macro_debug("readline cleanup");
       
      // cleanup
      close( fd[0] );
      close( fd[1] ); 
      
      throw eof{};
    }
    
    return line;
  }


  void readline::abort() {
    macro_debug("readline abort");
    // simply send stuff on control stream
    char c;
    write(fd[1], &c, 1);
    core::use(c);
  }

}
