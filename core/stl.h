#ifndef CORE_STL_H
#define CORE_STL_H

#include <core/pair.h>
#include <core/range/iterator.h>
#include <core/each.h>

namespace std {

  template<class, class> class vector;
  template<class, class> class list;
  template<class, class, class, class> class map;
  template<class, class, class, class> class multimap;
  template<class, class, class> class set;
  
}

namespace core {
  
  
  // iterator pairs
  template<class Container>
  std::pair<typename Container::iterator,
	    typename Container::iterator> all(Container& cont) { 
    return {cont.begin(), cont.end()}; 
  }

  template<class Container>
  std::pair<typename Container::const_iterator,
	    typename Container::const_iterator> all(const Container& cont) {
    return {cont.begin(), cont.end()}; 
  }


  template<class Container>
  std::pair<typename Container::reverse_iterator,
	    typename Container::reverse_iterator> reverse(Container& cont) {
    return {cont.rbegin(), cont.rend()};
  };


  template<class Container>
  std::pair<typename Container::const_reverse_iterator,
	    typename Container::const_reverse_iterator> reverse(const Container& cont) {
    return {cont.rbegin(), cont.rend()};
  };



  // convenience
  template<class C, class A, class Action>
  void each(const std::vector<C, A>& vec, const Action& action) {
    each(all(vec), action);
  }

  template<class C, class A, class Action>
  void each(std::vector<C, A>& vec, const Action& action) {
    each(all(vec), action);
  }
  

  template<class C, class A, class Action>
  void each(const std::list<C, A>& list, const Action& action) {
    each(all(list), action);
  }
  
  template<class C, class A, class Action>
  void each(std::list<C, A>& list, const Action& action) {
    each(all(list), action);
  }


  
  template<class Key, class Data, class Compare, class Alloc, class Action>
  void each(const std::map<Key, Data, Compare, Alloc>& map, const Action& action) {
    each(all(map), action);
  }

  template<class Key, class Data, class Compare, class Alloc, class Action>
  void each(std::map<Key, Data, Compare, Alloc>& map, const Action& action) {
    each(all(map), action);
  }

  // multimap
  template<class Key, class Data, class Compare, class Alloc, class Action>
  void each(const std::multimap<Key, Data, Compare, Alloc>& map, const Action& action) {
    each(all(map), action );
  }


  template<class Key, class Compare, class Alloc, class Action>
  void each(const std::set<Key, Compare, Alloc>& set, const Action& action) {
    each(all(set), action);
  }




}



#endif
 
