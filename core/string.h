#ifndef CORE_STRING_H
#define CORE_STRING_H

#include <string>
#include <sstream>
#include <cassert>

// #include <iostream>
#include <iosfwd>

namespace core {

  template<class A>
  std::string string(const A& a) {
    std::stringstream s;
    s << a;
    return s.str();
  }
  
  namespace {
    const char tab = '\t';
  }

  std::string indent(unsigned int level = 1);


  namespace impl {

    
    void arg(std::string& res, size_t start, const std::string& pattern);

    template<class H, class ... T>
    void arg(std::string& res, size_t start, const std::string& pattern,
	     H&& h, T&& ... t) {
      const size_t pos = res.find(pattern, start);
      assert( pos != std::string::npos );

      res.replace(pos, pattern.size(), string( std::forward<H>(h) ));
      
      arg(res, pos + pattern.size(), pattern, std::forward<T>(t)... );
    }


    
  }
  
  // Qt-style string replacement
  template<class ... Args>
  std::string arg(std::string res,
		  Args&& ... args) {
    impl::arg(res, 0, "%%", std::forward<Args>(args)...);
    return res;
  }
  
}


#endif
