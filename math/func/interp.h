#ifndef MATH_FUNC_INTERP_H
#define MATH_FUNC_INTERP_H

#include <map>
#include <list>
#include <math/func/spline.h>
#include <math/func/lie/subgroup.h>
#include <math/vector.h>

#include <core/stl.h>

namespace math {
  namespace func {
    
    namespace interp {

      
      template<class N>
      struct Nodes;

      namespace nodes {
    
	template<class N>
	math::vector< std::pair< typename Nodes<N>::domain, typename Nodes<N>::range > > 
	closest(const N& n,  const typename Nodes<N>::domain& u,  natural i) {
	  return Nodes<N>::closest(n, u, i); 
	}

      }
    
      
      namespace impl {
	
	template<class Map>
	static std::list<typename Map::const_iterator> closest(const Map& map, math::real t) {
	  assert(map.size() >= 2);
    
	  std::list<typename Map::const_iterator> res;

	  auto up = map.upper_bound(t);

	  if(up == map.begin()) {
	    // avant le 1er
	    res.push_front(up);
	    ++up;
	    res.push_back(up);
	  }
    
	  else if(up == map.end()) {
	    // apres le dernier
	    --up;
	    res.push_back(up);
	    --up;
	    res.push_front(up);
	  }
	  else {
	    res.push_back(up);
	    --up;
	    res.push_front(up);
	  }      
    
	  return res;
	}


	template<class Map>
	static std::list<typename Map::const_iterator> grow(const Map& map, 
							    math::natural n,
							    std::list<typename Map::const_iterator> res) {
	  assert(map.size() >= (res.size() + 2*n) );
    
	  // front
	  for(math::natural i = 0; i < n; ++i) {
	    auto front = res.front();
      
	    if( front != map.begin() ) {
	      --front;
	    }
	    res.push_front(front);
	  };

	  // back
	  for(math::natural i = 0; i < n; ++i) {
	    auto back = res.back();
      
	    ++back;
	    if( back == map.end() ) {
	      --back;
	    }
	    res.push_back(back);
	  };

	  return res;
	}
	
      }

      template<class Map>
      static math::vector< std::pair<math::real, typename Map::mapped_type> >  
      closest(const Map& map, math::real t, math::natural n = 2) {
	  
	assert( n % 2 == 0);
	  
	auto list = impl::grow(map, (n - 2)/2, impl::closest(map, t));

	math::vector< std::pair<math::real, typename Map::mapped_type > > res;
	res.resize( n );

	natural off = 0;
	core::each( list, [&](typename Map::const_iterator it) {
	    res(off++) = *it;
	  });

	return res;
      }







      
      template<class N>
      class spline {

      public:
	typedef typename Nodes<N>::domain domain;
	typedef typename Nodes<N>::range range;

	const N& nodes;
	const math::lie::group<range> lie;

	spline(const N& n, const math::lie::group<range>& lie) 
	  : nodes(n),
	    lie(lie) { }
      
	// TODO improve ?
	range operator()(const domain& x) const {
	  return patch(x)(x);
	}
	
	// T<range> diff(const T<domain>& dx) const {
	//   return patch( dx.at() ).diff( dx );
	// }
      private:
	
	typedef std::tuple< domain, range > node_type;
	typedef typename ::tuple::pow< node_type, 4 >::type nodes_type;
      
	typedef func::spline::coeffs::catmull_rom<domain> coeffs_type;
	typedef func::spline::patch<range, coeffs_type > patch_type;

	// return a spline patch around x
	patch_type patch(const domain& x) const  {
	  auto closest = interp::closest(nodes, x, 4);
	  using namespace std;
	  
	  return patch_type( std::make_tuple( closest(0).second,
					      closest(1).second,
					      closest(2).second,
					      closest(3).second ),
			     coeffs_type( closest(1).first,
					  closest(2).first ) ,
			     lie);
	}

      public:

	struct push : base< typename traits<patch_type>::push >  {
	  
	  push( const spline& of, const domain& at) 
	    : push::base(d(of.patch(at))(at)) { }
	  
	};
	
      private:

      
      };
      
      
      

      template<class U, class G, class Comp, class Alloc>
      struct Nodes< std::map<U, G, Comp, Alloc> > { 
	typedef std::map<U, G, Comp, Alloc> type;

	typedef U domain;
	typedef G range;

	typedef std::pair<domain, range> node_type;
    
    
	static math::vector< node_type >
	closest(const type& n, const domain& x, natural i) {
	  assert( i % 2 == 0 );
	  assert( ! n.empty()  );
      
	  typedef std::pair<domain, range> node_type;
      
	  typedef std::list<node_type, Eigen::aligned_allocator<node_type> > tmp_type;
	  tmp_type tmp;
      
	  typedef typename type::const_iterator iter_type;
	  typedef typename type::const_reverse_iterator rev_iter_type;
 
	  iter_type start = n.upper_bound(x); 
	  
	  if ( start == n.end() ) {
	    start = n.lower_bound(x);
	  }

	  assert( start != n.end() );
	  assert( start != n.begin() );
	  
	  int todo;

	  // right
	  todo = i / 2;
	  // try to add nodes on the right
	  for(iter_type up = start; up != n.end() && todo > 0; ++up, --todo) {
	    tmp.push_back( *up );
	  }
	  // can't add right: duplicate last node
	  for( int c  = 0; c < todo; ++c ) { 
	    node_type virtual_node = *n.rbegin();
	    virtual_node.first += c;
	    tmp.push_back( virtual_node );
	  }

	  // left
	  todo = i / 2;
	  for(rev_iter_type low(start); low != n.rend() && todo > 0; ++low, --todo) {
	    tmp.push_front( *low );
	  }
	  // cant add left: duplicate first node
	  for( int c = 0; c < todo; ++c ) { 
	    node_type virtual_node = *n.begin();
	    virtual_node.first -= c;
	    tmp.push_front( virtual_node );
	  }
	  
	  assert( tmp.size() == i);

	  // conversion
	  math::vector< node_type > res;
	  res.resize( i );

	  natural off = 0;
	  using namespace core;
	  core::each( tmp, [&](const node_type& p) {
	      res(off++) = p;
	    });
      
	  return res;
	}
    

    
      };
      
    }

    
    // template<class N>
    // interp::spline<N> curve(const N& nodes, const Lie<typename interp::Nodes<N>::range >& lie =
    // 			    Lie<typename interp::Nodes<N>::range >() ) { return {nodes, lie}; }


    template<class Map>
    interp::spline< Map > curve( const Map& map,
				 const math::lie::group<typename Map::mapped_type>& lie = 
				 math::lie::group<typename Map::mapped_type>()){
      return {map, lie};
    }
    
  }
}

#endif
