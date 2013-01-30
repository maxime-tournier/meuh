#ifndef SPARSE_STORAGE_H
#define SPARSE_STORAGE_H

#include <unordered_map>

namespace sparse {

  template<class Key, class Value>
  struct storage {
    typedef Key key_type;
    typedef Value value_type;
    
    typedef std::equal_to<key_type> pred_type;
    typedef std::hash<key_type> hash_type;

    typedef std::unordered_map<key_type,
			       value_type,
			       hash_type,
			       pred_type> type;

    ~storage() { 
      typename storage::dont_instanciate* _; 		// don't instanciate this class
    }
    
  };


}


#endif
 
