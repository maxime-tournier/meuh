#ifndef LINEAR_H
#define LINEAR_H


#include <tool/simulator.h>
#include <math/vec.h>
#include <phys/object.h>
#include <core/graph.h>
#include <geo/mesh.h>


#include <phys/solver/linear.h>
#include <core/callback.h>
#include <gui/frame.h>



#include <phys/solver/any.h>

#include <app/meuh/solver.h>



struct linear : tool::simulator {
  void init();
  void draw();
    
  void setup(math::natural n);
  void setup(math::natural w, math::natural h);
  
  virtual void factor( bool );

  virtual void setup();
  virtual void constraints();
  
  virtual phys::dof::velocity solve(const phys::dof::momentum& net);
  
  phys::dof::momentum momentum(math::real t, math::real dt);
  phys::dof::force force(math::real t, math::real dt);
  phys::dof::velocity correction(const phys::dof::velocity& );

  void integrate(const phys::dof::velocity&, math::real);
  void correct(const phys::dof::velocity& );

  typedef phys::object<math::vec3> obj_type;
  std::vector< obj_type > obj;
  
  math::natural width, height;
  math::real total_mass, particle_mass;
  math::real stiffness;
  
  phys::dof::velocity velocity();

  void fix( const phys::dof::velocity& ,
	    const phys::dof::velocity& );

  struct edge_type {
    math::real rest;
    phys::constraint::bilateral::key key;
    bool acyclic;
  };
    
  typedef core::graph<boost::undirectedS, math::natural, edge_type> mesh_type;
  
  struct constraint_type { 
    mesh_type mesh;

    struct attach {
      phys::constraint::bilateral::key key;
      phys::dof::key dof;
      math::real rest;
    };
    
    std::map<gui::frame*, attach > fixed;
  } constraint;
  
  std::vector<gui::frame*> frame;

  struct {
    geo::mesh mesh;
  } display;
    

  struct Solver : ::Solver { 
  public:
    Solver();

    using ::Solver::dofs;
    using ::Solver::bilateral;

    using ::Solver::velocity;
    using ::Solver::delta;
    using ::Solver::momentum;
    using ::Solver::mu;
    using ::Solver::lambda;
  
    using ::Solver::system;
    using ::Solver::in_tree;

    bool minres;
  };
  
  Solver solver;
  
  linear(QWidget* parent = 0);
  core::callback length_constraints, pin_constraints;
  
  void compare();

  struct {
    math::real epsilon;
  } weight;
  
  struct {
    bool show_graph;
  } flags;

  script::api app;
  
  enum {
    cholesky,
    cg
  };
  int solver_kind;

  int graph_kind;

  // phys::engine engine;
  
  struct {
    math::vec3 hot, cold, cloth;
  } color;

  math::vec warm;

};




#endif
