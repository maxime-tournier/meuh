#include "type.h"

namespace core {

  template<>
  std::string type<int>::name() { return "int";}

  template<>
  std::string type<void>::name(){ return "void"; }

  template<>
  std::string type<char>::name(){ return "char"; }

  template<>
  std::string type<double>::name(){ return "double"; }


}
