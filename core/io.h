#ifndef CORE_IO_H
#define CORE_IO_H

#include <string>
#include <fstream>
#include <iterator>

#include <core/error.h>

// TODO stabilize API, make .cpp ?

namespace core {
 
  namespace io {
    
    // TODO refine
    struct not_found : error { 
      not_found(const std::string& filename);
    };
    
    
    namespace load {


      std::string ascii(const std::string& filename);
      
    }


    template<class Action>
    void write(const std::string& filename,
	       const Action& action) {
      std::ofstream file;
      file.open( filename );
      action(file);
      file.close();

      // TODO RAII
    }


  }

}


#endif
