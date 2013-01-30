 
#include <core/io.h>
#include <mocap/bvh/file.h>

#include <mocap/skeleton.h>
#include <mocap/data.h>

#include <core/foreach.h>
#include <core/range/stl.h>


#include <iostream> 





int main(int argc, char* argv[] ) {
  
  using namespace std;
  
  if(argc != 2 ) return -11;
  
  string storage = core::io::load::ascii( argv[1] );
  mocap::bvh::file myfile(storage.begin(), storage.end() );
  
  std::cout << "success !" << std::endl;

  // mocap::skeleton* skel = skeleton(myfile);

  // core::tree::prefix( [](mocap::skeleton* s ) {
  //     std::cout << s->get().name << std::endl;
  //   })(skel);
  
  
  return 0;
}
 
 
