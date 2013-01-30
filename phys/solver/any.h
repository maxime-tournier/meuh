#ifndef PHYS_SOLVER_ANY_H
#define PHYS_SOLVER_ANY_H

#include <memory>
#include <phys/dof.h>

// #include <boost/any.hpp>

#include <phys/solver/result.h>
#include <phys/error.h>

#include <core/safe_bool.h>

namespace phys {
  namespace solver {

    struct task;

    struct any : core::safe_bool<any> {

      bool boolean() const;
      
      struct base {
	virtual ~base();
	virtual void solve(const task& ) const = 0;
      };
      
      template<class Solver>
      struct wrap : base {
	const Solver solver;
	
	wrap(const Solver& s) : solver(s) { }
	wrap(Solver&& s) : solver(std::move(s)) { }
	
	wrap(wrap&& ) = default;
	wrap(const wrap& ) = default;
	

	void solve(const task& t) const {
	  solver.solve(t);
	}
      };

      std::unique_ptr< base > impl;
      
      template<class Solver>
      void set(Solver&& s) {
	impl.reset( new wrap< typename std::decay<Solver>::type >( std::forward<Solver>(s)) );
      }
      
      any() { }

      any(const any& ) = delete;
      any(any&& ) = default;
      

      template<class Solver>
      any(Solver&& solver) {
	set(std::forward<Solver>(solver));	
      }
      
      
      template<class Solver>
      any& operator=(Solver&& solver) {
	set( std::forward<Solver>(solver) );
	return *this;
      }
      
      
      void solve(const task& t) const { assert(impl); impl->solve(t); }
      

      template<class Solver>
      const Solver& as() const {
	return static_cast< const wrap<Solver>* >(impl.get())->solver;
      }

      template<class Solver>
      Solver& as() {
	return static_cast<wrap<Solver>* >(impl.get())->solver;
      }
      
    };
    
  }
}



#endif
