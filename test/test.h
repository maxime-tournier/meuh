#ifndef TEST_H
#define TEST_H


template<class T, class U>
struct foo {
  
  virtual ~foo();
  virtual void bar();
  
  void fuu();   // not exported !
  
  int baz;
};



#endif


