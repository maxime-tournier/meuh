#ifndef SCRIPT_MEUH_H
#define SCRIPT_MEUH_H

#include <core/string.h>
#include <core/thread/atomic.h>

#include <script/lua.h>

// #include <script/call.h>

// convenience functions for use with meuh


namespace script {

  // returns an engine singleton. thread safe.
  typedef core::thread::atomic<lua>::handle<lua> the_type;
  the_type the();
  
  // executes a command 
  void exec(const std::string& );

  // convenience
  template<class H, class ... T>
  void exec(const std::string& what, 
	    H&& h, T&& ... t) {
    exec( core::arg(what, std::forward<H>(h),
		    std::forward<T>(t)...) );
  }

  
  // // convenience
  // template<class H, class ... T>
  // void async(std::string what, 
  // 	     H&& h, T&& ... t) {
  //   async( core::arg(what, std::forward<H>(h),
  // 		     std::forward<T>(t)...) );
  // }
  
  
  // requires a lua module under the same global name
  void require(const std::string& name);

  // adds a directory to package.path
  void path(const std::string& dir);
  
  // convenience: adds an app path to package.path
  void app(const std::string& name);
  
  // calls a lua function (you need to give return type as first
  // template arg)
  template<class Ret, class ... Args>
  Ret call(const std::string& name, 
	   Args&& ... args) {
    the_type lock = the();
    
    // if this triggers an error, you probably need to include <script/call.h>
    return call<Ret>(*lock, name, std::forward<Args>(args)...);
  }
  
}


#endif
