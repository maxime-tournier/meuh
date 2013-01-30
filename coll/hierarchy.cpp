#define _GLIBCXX_PARALLEL 1	// for parallel sort !

#include "hierarchy.h"

#include <set>
#include <core/stl.h>
#include <mutex>

namespace coll {


    hierarchy::hierarchy(const geo::aabb& bbox, math::natural count) {
      octree.aabb = bbox;
      if( count ) occupation.reserve(count);
    }


    std::vector< hierarchy::test_type > hierarchy::test() {

      auto comp = [](const occupation_type& a, const occupation_type& b) -> bool {
	return 
	std::make_pair( geo::octree::morton( a.id ), a.depth ) <
	std::make_pair( geo::octree::morton( b.id ), b.depth );
      };

      if( occupation.empty() ) return {} ;
    
      std::sort(occupation.begin(), occupation.end(), comp);
      
      std::vector< test_type > tmp;
      tmp.reserve( occupation.size() );
      
      for(math::natural i = 0, n = occupation.size() ; i < n - 1; ++i) {
	occupation_type& current = occupation[i];
	
	// next is a child cell
	for(math::natural k = i + 1; k < n && 
	      octree.clamp( occupation[ k ].id, current.depth) == current.id; ++k) {
	  
	  // add hit to the list
	  tmp.push_back( std::make_pair( std::min(current.group, occupation[k].group ),
					 std::max(current.group, occupation[k].group )  ) );
	}
      }
      std::sort( tmp.begin(), tmp.end() );
	
      auto end = std::unique( tmp.begin(), tmp.end() );
	
      return {tmp.begin(), end};
    }


    hit::list hierarchy::hits() {
      auto tests = test();
      
      hit::list res;

      std::mutex mutex;
      core::each(tests, [&](const coll::group* first, const coll::group* second) {
      // std::for_each(tests.begin(), tests.end(), [&](const test_type& t) {
      // 	  auto first = t.first;
      // 	  auto second = t.second;

	  assert( first != second );
	  auto h = first->collide( *second );
	
	  res.splice(res.end(), h);

	});
      
      return res;
    }
      

    void hierarchy::add(const geo::aabb& bbox, const coll::group* g) {
      if( bbox.empty() ) return;
      
      // highest depth containing bbox, termwise
      // at depth k, octree cells have radius r / 2^k
      math::vec3 k = octree.aabb.radius().cwiseProduct( bbox.radius().cwiseInverse() ).array().log() / std::log(2);
      
      // min coeff: box of size immediately greater
      // max coeff: best fit for vertices, but need to fill in
      math::natural depth = k.minCoeff();
      
      // which cells are occupied by the 
      // TODO avoid heap allocs with vector + sort + unique
      std::set< geo::octree::id_type > cells;
      
      bbox.each([&](const math::vec3& v) {
	  cells.insert( octree.clamp( octree.id(v), depth ) );
	});
      
      core::each( cells, [&](geo::octree::id_type id) {
	  occupation_type occ;

	  occ.id = id;
	  occ.depth = depth;
	  occ.group = g;

	  occupation.push_back( occ );
	});
    }

  }
