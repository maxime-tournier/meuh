#include "io.h"

namespace core {

  namespace io {

    not_found::not_found(const std::string& filename)  
      : error( filename + " not found" ) { 
    
    };

    namespace load {

      std::string ascii(const std::string& filename) {
	using namespace std;
	std::string res;
	
	ifstream in( filename ); 
	if( !in.is_open() )  throw not_found( filename ); 
	
	in.unsetf(ios::skipws); // No white space skipping!
	copy( istream_iterator<char>(in), 
	      istream_iterator<char>(), 
	      back_inserter(res));
	
	return res;
	
      };

    }
  }
}
