#ifndef TOOL_RIGID_H
#define TOOL_RIGID_H

#include <phys/object.h>

#include <geo/mesh.h>
#include <gl/mesh.h>
#include <coll/primitive.h>

#include <phys/rigid/body.h>

#include <math/se3.h>

#include <core/callback.h>

#include <math/tuple.h>
#include <math/vec.h>
#include <math/so3.h>
#include <math/covector.h>

#include <geo/aabb.h>

namespace tool {


  struct rigid : phys::object< std::tuple<math::SO3, math::vec3>  > {
    
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW;

    typedef phys::object< config_type > base;
    
    struct geometry_type {
      std::unique_ptr< geo::mesh > mesh;
    } geometry;
    
    struct visual_type {
      std::unique_ptr<gl::mesh> mesh;
    } visual;
    
    struct collision_type {
      std::unique_ptr<coll::primitive> primitive;
      geo::aabb aabb;
    } collision;
    
    // rigid body properties (mass ... )
    phys::rigid::body body;
    
    void mesh(const geo::mesh& mesh);
    core::callback update;

    void draw() const;

    // momentum_type map(const math::vec3& local, const math::force& f) const;

    math::SE3 frame() const;
    void frame(const math::SE3&);

    rigid();

    rigid(const rigid& ) = delete;
    rigid(rigid&& ) = default;
    ~rigid();

    struct map {
      math::vec3 local;

      math::vec3 operator()(const config_type& ) const;

      struct push {
	config_type at;
	math::vec3 local;

	push(const map& , const config_type& );
	
	math::vec3 operator()(const velocity_type& ) const;
      };


      struct pull {
	config_type at;
	math::vec3 local;

	pull(const map& , const config_type& );
	
	momentum_type operator()(const math::force& ) const;
      };


    };

  };




}


#endif
