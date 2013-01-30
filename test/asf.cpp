

#include <tool/asf.h>

 
int main(int argc, char** argv) {
  
  if(argc == 1) return 0;
  
  std::string filename = argv[1];
  
  tool::asf taiste(filename);
  
  return 1;
} 
