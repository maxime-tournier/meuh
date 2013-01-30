#include "asf.h"

#include <mocap/asf/file.h>
#include <core/io.h>

namespace tool {

  template<class What, class Iterator>
  static What parse(Iterator first, Iterator last) {
    typename What::template parser<Iterator> parser;
  
    using namespace mocap::asf;

    What res;
    skipper<Iterator> skipper;
    bool ok = qi::phrase_parse( first, last, parser, skipper, res);
  
    struct error : std::exception { };
    if(!ok || first != last ) { throw error(); }
  
    return res;
  }


  static phys::rigid::skeleton make_skeleton(const std::string& filename) {
    const std::string storage = core::io::load::ascii( filename );
    
    auto file = parse<mocap::asf::file>(storage.begin(), storage.end());
    
    file.debug();
    

    phys::rigid::skeleton::topology_type top;

    return {top};
  }
  
  
  asf::asf(const std::string& filename) 
    : filename(filename),
      skeleton( make_skeleton(filename)  )
  {

  }


}
