#ifndef TUPLE_OSTREAM_H
#define TUPLE_OSTREAM_H

#include <core/tuple/apply.h>

namespace core {
  namespace tuple {

    
    struct to_ostream {
      std::ostream& out;

      template<class A, class ... Args> 
      void operator()(const A& a, const Args& ... args ) const {
	out << a << ", ";
	operator()( args... );
      }

      template<class A>
      void operator()(const A& a) const { 
	out << a;
      };

      void operator()() const { }

    };

    template<class ... Args>
    std::ostream& operator<<(std::ostream& out, const std::tuple<Args...>& args) {
      out << "( "; 
      apply( to_ostream{ out }, args );
      return out << " )" ;
    }


  }
}

#endif
