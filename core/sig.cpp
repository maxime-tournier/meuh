#include <core/sig.h>

#include <signal.h>
#include <map>
#include <cassert>
#include <stdlib.h>

#include <core/log.h>

namespace core {

  
  namespace sig {
    static std::map<int, core::callback> handlers;

    static struct sigaction psa;
    static struct sigaction old;
    

    static void (*muchel)(int) = 0;
    
    static void sig_handler(int signo ) {
      auto it = handlers.find( signo );
      if( it == handlers.end() ) core::log("DURR");

      it->second();
    }

    static struct initizalizer {
      initizalizer() {
	psa.sa_handler = sig_handler;
      }
    } init;
        
    callback& handler(int signo) {
      auto it = handlers.find( signo );
      
      if( it == handlers.end () ){
	sigaction(signo, &psa, &old);
	
	core::callback cb = [&,signo] {
	  signal(SIGINT, SIG_DFL);
	  raise(SIGINT);
	};
	
	it = handlers.insert(it, std::make_pair(signo, cb));
      }
      
      return it->second;
    }


  }

}
