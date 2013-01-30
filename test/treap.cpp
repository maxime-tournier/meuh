#include "test.h"

#include <core/treap.h>
#include <core/log.h>
#include <core/timer.h>

#include <set>
#include <unordered_set>

#include <core/unused.h>
#include <core/stl.h>


template<class C, class F>
void traverse(const C& c, const F& f) {
  core::each(core::all(c), f );  
}

template<class U, class F>
void traverse(const core::treap<U>& x, const F& f) {
  x.each( f );
}

struct value_type {
  int x;
  std::string lol;

  bool operator<(const value_type& other) const { return x < other.x; }
  bool operator==(const value_type& other) const { return x == other.x; }
};

value_type make_value(int i) {
  return {i, "lol"};
}


template<class C>
void test(C& c, int size, std::string name) {
  core::timer t;
  core::log(name);
  
  t.start();
  for(int i = 0; i < size; ++i) {
    c.insert( make_value(i) );
  }
  core::log("creation:", t.start());
  core::log();

  t.start();
  for(int i = 0; i < size; ++i) {
    c.find( make_value(i) );
  }
  core::log("find:   ", t.start());

  auto print = [&](int x) {
    std::cout << x << " ";
  };

  auto noop = [&](const value_type& ) {
    
  };
  
  t.start();
  traverse( c, noop );
  core::log("traverse:   ", t.start());

  core::log("===");
  
}

int main(int argc, char** argv) {
  
  int size = 10;

  if( argc > 1 ) size = std::stoi(argv[1]);

  core::treap< value_type > treap(size);
  std::set< value_type > set;
  std::unordered_set< value_type > unordered_set(size);
      
  test( treap, size, "treap");
  test( set, size, "set");
  // test( unordered_set, size, "unordered set");
  
  
}
