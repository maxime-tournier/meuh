#include <gl/obj/file.h>

#include <core/io.h>
#include <iostream>

int main(int argc, char** argv) {

  if(argc != 2) throw core::exception("need one argument");
  
  typedef std::string::const_iterator iterator;
  
  using namespace gl;

  const std::string& storage = core::io::load::ascii( argv[1] );
  iterator first = storage.begin(), last = storage.end();
    
  obj::file res(first, last);

  std::cout << "success !" << std::endl;

  return 0;
}
