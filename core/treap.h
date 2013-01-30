#ifndef CORE_TREAP_H
#define CORE_TREAP_H

#include <vector>
#include <core/rand.h>
#include <cassert>

#include <core/log.h>

namespace core {

  template<class T>
  class treap {
    typedef unsigned int index_type;
    static constexpr index_type none = -1;
    
 
    struct node_type {
      T value;
      index_type left, right, parent;
      index_type priority;
      
      node_type(const T& value) 
      : value(value),
	left(none),
	right(none),
	parent(none),
	priority(  core::rand() ) {
      }
    };
    
    index_type root;
    
    typedef std::vector<node_type> storage_type;
    storage_type storage;
    
     
    node_type& node(index_type i) { 
      assert( i != none );
      assert( i < storage.size() );
      return storage[i]; 
    }
    
    const node_type& node(index_type i) const { 
      assert( i != none );
      assert( i < storage.size() );

      return storage[i]; 
    }

    bool leaf(index_type i) const { 
      const node_type& n = node(i);
      return n.left == none && n.right == none;
    }

    node_type& push(const T& value) {
      storage.push_back( {value} );
      return storage.back();
    }
    
    // performs one node rotation to move given node up
    void rotate(index_type i) {
      if( i == root ) return;
      
      node_type& n = node(i);
      node_type& p = node(n.parent);
      
      index_type pi = n.parent;
      
      // right rotation
      if( p.left == i ) {

	// move n.right to p.left
	p.left = n.right;
	if( n.right != none ) node(n.right).parent = pi; 
	
	// n.right becomes p
	n.right = pi;
	n.parent = p.parent;
	p.parent = i;
	
      } else if(p.right == i) {

	p.right = n.left;
	
	if( n.left != none ) node(n.left).parent = pi; 
	
	n.left = pi;
	n.parent = p.parent;
	p.parent = i;
	
      } else assert( false );
      
      if( n.parent == none ) root = i;
      else {
	node_type& pp = node(n.parent);
	if( pp.left == pi ) pp.left = i;
	else if ( pp.right == pi ) pp.right = i;
	else assert( false );
	
      }
    }


  public:
    
    treap(index_type n = 0) : root(none) {
      storage.reserve( n );
    }
    
    void reserve(index_type n) {
      storage.reserve(n);
    }
    
    void insert(const T& value) {
      
      if( root == none ) { 
	root = 0;
	push( value );
	
	return;
      }
      
      index_type curr = root;
      
      bool inserted = false;
      index_type index = storage.size();
      
      while( !inserted ) {
	node_type& c = node(curr);

	if( value == c.value ) return; // TODO throw ?
	
	index_type& branch = value < c.value ? 
	  c.left : c.right;
	
	if( branch == none ) {
	  branch = index;
	  push(value).parent = curr;
	  inserted = true;
	} else {
	  curr = branch;
	}
      }
      
      // here we inserted the node at its binary-search place, we must
      // now enforce priority-heap structure

      // inserted node
      curr = index;
      node_type& n = node(curr);
      
      while(curr != root && n.priority > node(n.parent).priority ) {
	rotate( curr );
      }
      
    }
    
    template<class ... Args>
    void insert(const T& value, Args&& ... args) {
      insert(value);
      insert(std::forward<Args>(args)...);
    }




    template<class F>
    void each( const F& f ) { each_under(f, root); }

    template<class F>
    void each( const F& f ) const { each_under(f, root); }

    T* find( const T& value ) {
      
      index_type curr = root;
      while( curr != none ) {
	node_type& c = node(curr);
	
	if( value == c.value ) return &c.value;
	
	curr = value < c.value ? c.left : c.right;
      }
      
      return 0;
    }

    const T* find( const T& value ) const {
       
      index_type curr = root;
      while( curr != none ) {
	node_type& c = node(curr);
	 
	if( value == c.value ) return &c.value;
	
	curr = value < c.value ? c.left : c.right;
      }
      
      return 0;
    }
    
     
  protected:
    
    bool empty() const { return root == none; }

    template<class F>
    void each_under(const F& f, index_type i) {
      if(i == none) return;
      
      node_type& n = node(i);
      each_under(f, n.left);
      f(n.value);
      each_under(f, n.right);
    }
    
    template<class F>
    void each_under(const F& f, index_type i) const {
      if(i == none) return;

      const node_type& n = node(i);
      each_under(f, n.left);
      f(n.value);
      each_under(f, n.right);
    }
    
  };




}


#endif
