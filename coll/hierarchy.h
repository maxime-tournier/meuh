#ifndef COLL_HIERARCHY_H
#define COLL_HIERARCHY_H

#include <geo/octree.h>
#include <coll/group.h>

#include <vector>

namespace coll {

    struct hierarchy {
      geo::octree octree;
      
      struct occupation_type {
	geo::octree::id_type id;
	math::natural depth;
	
	const coll::group* group;
      };
    
      std::vector< occupation_type > occupation;
      
    public:
      
      hierarchy(const geo::aabb& bbox, math::natural count= 0);
      
      // fills occupation list
      void add(const geo::aabb& bbox, const coll::group* );
      
      typedef std::pair< const coll::group*, 
			 const coll::group* > test_type;

      // get colliding groups
      std::vector< test_type > test();
      
      // convenience: get collision hits (calls @test)
      hit::list hits();

    };


}




#endif
