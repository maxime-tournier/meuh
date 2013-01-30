#ifndef GEO_OBJ_NAMESPACES_H
#define GEO_OBJ_NAMESPACES_H

namespace boost {
  namespace spirit { 

    namespace qi { }
    
  }

  namespace phoenix {

  }


}


namespace geo {

  namespace obj {
    namespace spirit = boost::spirit;
    namespace phoenix = boost::phoenix;
    
    namespace qi = spirit::qi;
    
  }

}

#endif
