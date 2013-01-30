#ifndef SPARSE_MAP_H
#define SPARSE_MAP_H

#include <map>

#include <sparse/insert.h>

namespace sparse {


  // template<class Key, class Res, class Content, class F>
  // void map(std::map<Key, Res>& res, const std::map<Key, Content>& data, const F& f = F() ) {
  //   res.clear();
      
  //   auto it = res.begin();

  //   core::each(data, [&]( Key k, const Content& c ) {
  // 	it = ++sparse::insert(res, k, f(c), it );
  //     });
    
  // }
  
  // // !!! you need the first template parameter for return type !!!
  // template< class Res, class Key, class Content, class F>
  // std::map<Key, Res> map(const std::map<Key, Content>& data, const F& f = F() ) {
  //   std::map<Key, Res> res;
    
  //   map(res, data, f);
    
  //   return std::move(res);      
  // }
  
  
}


#endif

