#ifndef SPARSE_INIT_H
#define SPARSE_INIT_H

namespace phys {
  namespace sparse {

    template<class V>
    struct default_construct {
      typedef V result_type;
      
      template<class A>
      V operator()(const A& ) const { return V(); } 
    };
    
    template<class Value, class KeyIterator>
    boost::transform_iterator<default_construct<Value>, KeyIterator> init_iterator(const KeyIterator& key) {
      return boost::make_transform_iterator(key, default_construct<Value>() );
    }

    

  }
}



#endif
