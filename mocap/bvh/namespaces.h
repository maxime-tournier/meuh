#ifndef MOCAP_BVH_NAMESPACES_H
#define MOCAP_BVH_NAMESPACES_H

namespace boost {
  namespace spirit {
    namespace qi  { }
    namespace ascii { }
  }

  namespace phoenix { }

}


namespace mocap {
  
  namespace bvh {
    namespace qi = boost::spirit::qi;
    namespace ascii = boost::spirit::ascii;
    namespace phoenix = boost::phoenix;
  }  
}




#endif
