#ifndef CORE_TREE_NODE_H
#define CORE_TREE_NODE_H


#include <cassert>
#include <utility>
#include <core/error.h>

// TODO remove this once g++ gets aligment support
#include <Eigen/Core>

namespace core {
  namespace tree {

    // forward
    template<class> struct node;

    namespace algo {
      template<class Action> struct prefix;
      template<class Action> struct children;
      template<class Action> struct safe_children;
    }

    template<class Action> algo::prefix<Action> prefix(const Action& );
    template<class Action> algo::children<Action> children(const Action& );
    template<class Action> algo::safe_children<Action> safe_children(const Action& );
    
    
    // main tree structure: node
    template<class A>
    class node {
      A value_;
      
      node* next_;
      node* prev_;

      node* first_;
      node* last_;

      node* parent_;

    public:

      // simply forward arguments to 
      template<class ... Args>
      node(Args&&... args) 
	: value_( std::forward<Args>(args)... ),
	  next_(0),
	  prev_(0),
	  first_(0),
	  last_(0),
	  parent_(0)
      { }
   
      // node content
      const A& get() const { return value_; }
      A& set() {return  value_; }
      
      // convenience
      A* operator->() { return &value_;} 
      const A* operator->() const { return &value_;}

      // topology accessors

      // siblings
      node* next() const { return next_; }
      node* prev() const { return prev_; }
      
      // children
      node* first() const { return first_; }
      node* last() const { return last_; } 
      
      // parent
      node* parent() const{ return parent_; }
      

      // convenience
      bool leaf() const { 
	assert( first() || !last() );
	return !first(); 
      }

      bool root() const {
	assert( parent() || ( !next() && !prev() ) );
	return !parent();
      }

      // mutators
      // append child
      node* push( node * c) {
	assert( c->root() );

	if( leaf() ) {
	  first_ = c;
	} else {
	  assert( !last()->next() );
	  last()->next_ = c;
	  
	  assert( !c->prev() );
	  c->prev_ = last();  
	}

	last_ = c;
	c->parent_ = this;

	return c;
      }    
      
      // inserts a sibling after
      node* insert(node * s ) {
	if( root() ) throw error("cant insert on a root");
	if( !s->root() ) throw error("you shouly only insert roots");

	node* old_next = next();

	next_ = s;
	s->prev_ = this;
	s->parent_ = parent();
	
	if(old_next) {
	  old_next->prev_ = s;
	  s->next_ = old_next;
	} else {
	  parent()->last_ = s;
	}
	
	return s;
      }
      
      // reroots current tree on a subnode, preserving topology
      node* reroot( node* n ) {
	// TODO n must be a subnode
	if( n->root() ) return n;

	node* p = n->parent();
	n->detach();
	reroot( p );
	n->push( p );
	
	return n;
      }



      // TODO swap ?
      
      // detach node w/ offspring from hierarchy
      node* detach() {
	if( root() ) return this;

	// i have a parent
	if( next() ) {
	  next()->prev_ = prev();
	} else {
	  // i'm the last child
	  parent()->last_ = prev();
	}

	if( prev() ) {
	  prev()->next_ = next();
	} else {
	  parent()->first_ = next();
	}
	
	parent_ = 0;
	next_ = 0;
	prev_ = 0;
	
	return this;
      }

      ~node() { 
	detach();
	safe_children( [](node* c) { delete c; } )(this);
      }
      

      template<class Action>
      void exec(const Action& action) { action(this); }
    
      // convenience
      template<class Action>
      void each( const Action& action) { exec( prefix( action ) ); }
      
      template<class Action>
      void children( const Action& action){ exec( tree::children( action ) );}
      

    public:
      EIGEN_MAKE_ALIGNED_OPERATOR_NEW
    };
    
  }
}



#endif
