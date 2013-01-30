#ifndef CORE_CONTAINS_H
#define CORE_CONTAINS_H

namespace core {

  template<class Container>
  bool contains(const Container& container, const typename Container::value_type& value) {
    return container.find(value) != container.end();
  }

}


#endif
