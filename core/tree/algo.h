#ifndef CORE_TREE_ALGO_H
#define CORE_TREE_ALGO_H

#include <core/tree/node.h>
#include <algorithm>
#include <list>

// TODO break in parts
namespace core { 
  namespace tree {
    
    namespace algo {

      template<class Action>
      struct ref {
	const Action& action;

	template<class A>
	void operator()(node<A>* n) const { action(n); }
      };
      
      
    }

    template<class Action>
    algo::ref<Action> ref(const Action& action) { return {action}; }
    
    
    namespace algo {
      template<class Action>
      struct children {
	
	const Action action;
	
	template<class A>
	void operator()(node<A>* n) const {
	  for(node<A>* c = n->first(); c; c = c->next()) {
	    action(c);
	  }
	}

      };

      template<class Action>
      struct safe_children {
	
	const Action action;
	
	template<class A>
	void operator()(node<A>* n) const {
	  std::list<node<A>*> backup;
	  for(node<A>* c = n->first(); c; c = c->next()) {
	    backup.push_back(c);
	  }
	  
	  std::for_each(backup.begin(), backup.end(), action);
	}

      };



    }

    template<class Action>
    algo::children<Action> children(const Action& action) { return {action}; }

    template<class Action>
    algo::safe_children<Action> safe_children(const Action& action) { 
      return {action}; 
    }
    
    
    namespace algo {
      template<class Action>
      struct prefix {
	
	const Action action;
	
	template<class A>
	void operator()(node<A>* n) const {
	  action(n);
	  n->children( tree::ref( *this ) );
	}
	
      };


      template<class Action>
      struct postfix {
	
	const Action action;
	
	template<class A>
	void operator()(node<A>* n) const {
	  n->children( tree::ref( *this ) );
	  action(n);
	}
	
      };
      
    }
    
    template<class Action>
    algo::postfix<Action> postfix(const Action& action) { return {action}; }

    template<class Action>
    algo::prefix<Action> prefix(const Action& action) { return {action}; }
    
    // TODO breadth-first

    namespace algo {

      template<class Function>
      struct transform {
	
	const Function f;
	
	template<class A>
	auto operator()(node<A>* n) const -> decltype( f( n ) )
	{
	  auto res = f(n);
	  
	  for(node<A>* c = n->first(); c; c = c->next() ) {
	    res->push( operator()(c) );
	  }
	
	  return res;
	}
      
      };

    }
    
    template<class F>
    algo::transform<F> transform(const F& f) { return {f}; }

    
    template<class A>
    node<A>* copy(node<A>* n) { 
      return transform( [](node<A>* n) { 
	  return new node<A>( n->get()  ); 
	} )( n );
      
    }
    
    namespace algo {

      template<class Predicate>
      struct find {
	const Predicate pred;

	template<class A>
	node<A>* operator()(node<A>* n) const {
	  if( pred(n) ) return n;
	  
	  for(node<A>* c = n->first(); c; c = c->next() ) {
	    node<A>* res = (*this)(c);
	    if( res ) return res;
	  }
	  
	  return 0;
	}

      };

      template<class Predicate, class Output>
      struct collect {
	mutable Output out;
	const Predicate pred;
	
	template<class A>
	void operator()(node<A>* n) const {
	  if( pred(n) ) {
	    ++out;
	    *out = n;
	  }
	}

      };
      
    }

    template<class Predicate>
    algo::find<Predicate> find(const Predicate& pred) { return {pred}; }
    
    template<class Predicate, class Output>
    algo::collect<Predicate, Output> collect(Output out,
					     const Predicate& pred) {
      return {out, pred};
    }


    
    template<class A>
    unsigned int size(node<A>* n) {
      if( !n) return 0;

      unsigned int res = 1;

      n->children( [&res](node<A>* c) {
	  res += size(c);
	} );
      
      return res;
    }

    template<class Print>
    struct pretty {
      std::ostream& out;
      const Print& print;
      const unsigned int depth;
      
      pretty(std::ostream& out, const Print& print, unsigned int depth = 0) 
	: out(out), print(print), depth(depth) { }
      
      template<class A>
      void operator()(node<A>* n) const {
	for(unsigned int i = 0; i < depth; ++i) {
	  out << "-";
	}
	out << " ";
	
	print(n);

	out << '\n';

	n->children( pretty(out, print, depth + 1) );
      }
    };

    template<class Print>
    pretty<Print> pretty_print(std::ostream& out, const Print& print) {
      return pretty<Print>(out, print);
    }
        
    
    namespace algo {

      template<class Action>
      struct ancestors {

	const Action action;

	template<class A>
	void operator()(node<A>* n) const {
	  if( n->root() ) return;

	  action( n->parent() );
	  (*this)( n->parent() );
	};

      };
      
    }


    template<class Action>
    algo::ancestors<Action> ancestors(const Action& action) { return {action}; }
    
  }
}

#endif
