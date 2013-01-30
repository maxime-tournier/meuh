#ifndef TEST_INL
#define TEST_INL

#include <iostream>

template<class T, class U>
void foo<T, U>::bar() {
  fuu();
}

template<class T, class U>
void foo<T, U>::fuu() {
  std::cout << "fuu !" << std::endl;
}

template<class T, class U>
foo<T, U>::~foo() { }

#endif
