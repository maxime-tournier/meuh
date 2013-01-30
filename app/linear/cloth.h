#ifndef CLOTH_H
#define CLOTH_H

#include <core/graph.h>
#include <vector>

#include <phys/object.h>
#include <phys/system.h>
#include <phys/constraint/bilateral.h>

#include <geo/mesh.h>

struct cloth {
  
  typedef phys::object<math::vec3> obj_type;
  std::vector< obj_type > dofs;
  
  std::vector< phys::constraint::bilateral > constraints;

  struct edge_type {
    math::real rest;
    phys::constraint::bilateral::key key;
    bool acyclic;
  };
    
  typedef core::graph<boost::undirectedS, math::natural, edge_type> mesh_type;
  mesh_type mesh;
  
  math::natural width, height;
  math::natural size() const;

  math::real total_mass;
  math::real scale;
  
  math::natural color;

  cloth(math::natural w, math::natural h, math::real scale = 1.0);
  ~cloth();
  
  void mass(phys::system& );
  void bilateral(phys::system& );

  phys::dof::force force() const;

  geo::mesh geometry;
  void draw() const;
  
  math::real particle_mass() const;

  void reset();

  std::set<phys::constraint::bilateral::key> in_tree;

  typedef math::vector< math::vec3 > delta_type;
  math::real error2(const delta_type& delta = delta_type());
  
};



#endif
