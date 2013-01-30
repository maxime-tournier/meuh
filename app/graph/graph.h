#ifndef GUI_GRAPH_H
#define GUI_GRAPH_H

#include <gl/gl.h>
#include <memory>
#include <QGLViewer/qglviewer.h>

#include <math/types.h>
#include <phys/object.h>

#include <gui/frame.h>

#include <core/graph.h>
#include <geo/mesh.h>

#include <phys/solver/bilateral.h>
#include <phys/system.h>

#include <tool/viewer.h>



namespace gui {

  struct graph : tool::viewer {

    void init();
    void draw();
    void animate();
    
    void reset(math::natural n);
    void keyPressEvent(QKeyEvent* e);
    
    typedef phys::object<math::vec3 > obj_type;
    std::vector< obj_type > obj;

    math::natural width, height;

    std::vector<gui::frame*> frame;
    
    core::callback on_animate;
    
    struct edge_type {
      math::real rest;
      const phys::constraint::bilateral* constraint;
      bool acyclic;
    };
    
    typedef core::graph<math::natural, edge_type> mesh_type;

    struct constraint_type { 
      mesh_type mesh;
      phys::constraint::bilateral::vector lambda;
      
      struct attach {
	phys::constraint::bilateral::key key;
	math::natural dof;
      };
      
      std::map<gui::frame*, attach > fixed;
      
      std::map<phys::constraint::bilateral::key, 
	       core::callback > update;
      
    } constraint;

    phys::system system;

    math::real dt;
    
    typedef phys::solver::bilateral solver_type;
    std::unique_ptr<solver_type> solver;
    
    struct {
      geo::mesh mesh;
    } display;
    
  };


}



#endif
