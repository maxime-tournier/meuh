#include "meuh.h"

#include <core/meuh.h>
#include <core/share.h>

#include <core/thread/queue.h>
#include <core/log.h>

#include <memory>
#include <thread>



namespace script {
  
  static std::unique_ptr< core::thread::atomic<lua>  > instance; 
  
  void path( const std::string& dir ){ 
    exec( core::arg("package.path = '%%/?.lua;'..package.path", dir) );
  }

  void app(const std::string& name) {
    path( core::meuh::root() + "/app/" + name );
    exec("require '" + name + "'");
  }
  
  // static core::thread::queue< std::string > queue;

  // static std::unique_ptr<std::thread> worker;

  
  the_type the() {
    if( !instance ) { 
      instance.reset( new core::thread::atomic<lua>  );

      the()->libs();
      path( core::share("/script" ));
      require( "lib" );
      exec( "lib.load('%%')", core::meuh::root() + "/lib/libscript.so" );

      // // avanti !
      // worker.reset( new std::thread( [] {
      // 	    while( true ) {
      // 	      queue.wait( [&]( std::string&& cmd ) {
      // 		  the()->exec( cmd );
      // 		  // core::log( cmd );
      // 		});
      // 	    } 
      // 	  }) 
      // 	);
      
    }
    
    return (*instance)();
  }
  
  
  void exec(const std::string& cmd) {
    the()->exec( cmd );
    // auto& hurr = the().get();
    // hurr.exec(cmd);
  }


  // void async(std::string cmd) {
  //   queue.push( std::move(cmd) );
  // }
  
  
  void require( const std::string& module) {
    exec( core::arg("%% = require '%%'", module, module) );
  }
  

}

