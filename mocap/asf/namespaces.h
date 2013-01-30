#ifndef MOCAP_ASF_NAMESPACES_H
#define MOCAP_ASF_NAMESPACES_H


namespace boost {
  namespace spirit {
    namespace qi  { }
    namespace ascii { }
  }

  namespace phoenix { }

}


namespace mocap {
  
  namespace asf {
    namespace qi = boost::spirit::qi;
    namespace ascii = boost::spirit::ascii;
    namespace phoenix = boost::phoenix;
  }  
}




#endif
