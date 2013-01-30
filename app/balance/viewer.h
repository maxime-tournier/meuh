#ifndef VIEWER_H
#define VIEWER_H

#include <tool/simulator.h>
#include <phys/rigid/skeleton.h>

#include <tool/character.h>
#include <tool/reduced.h>

#include <tool/mocap.h>
#include <tool/pga.h>

#include <coll/hierarchy.h>
#include <coll/primitive.h>
#include <tool/contacts.h>

#include <core/shared.h>
#include <core/callback.h>

#include <gui/frame.h>

#include <phys/step/symplectic.h>

#include <tool/control.h>
#include <tool/ground.h>
#include <tool/tweak.h>


struct viewer : tool::simulator {
  
  viewer();

  
  struct reduced_type : tool::reduced< math::vec > {
    
    reduced_type(const tool::mocap& mocap, math::natural n,
		 const tool::pga::exclude_type& exclude = tool::pga::exclude_type() );
    tool::pga pga;

    // math::vec3 H, L;

    math::real damping;
    lagrangian_type interaction;
  };

  typedef reduced_type character_type;  
  std::unique_ptr<character_type> character;
  math::vec3 char_color;
  
  void draw();
  void init();

  void setup();
  void reset();

  void straighten();
  
  core::shared<core::callback> post_draw, pre_animate;
  
  core::callback pick_cb;

  struct {
    math::vec3 local;
    math::real stiff;
  } pick;

  tool::ground ground;
  tool::contacts contacts;

  void collisions();
  
  phys::step::control control(const math::mat& M,
			      const math::mat& Minv);
  
  math::real laziness;
  math::real smoothness;

  struct com_type {
    gui::frame target;
    math::real stiffness;
  } com;

  struct am_type {
    math::real stiffness;
  } am;

  struct zmp_type {
    math::real stiffness;
  } zmp;


  struct reference_type {
    math::real stiffness;
  } reference;

  math::real head;
  math::real grab;
  math::natural steps;
  
  struct support_type {
    std::unique_ptr<math::vec3> center;
    math::real weight;
  } support;


  math::iter iter;
  
  phys::constraint::bilateral::vector lambda;
  phys::constraint::unilateral::vector mu;

  enum solver_type {
    uncontrolled = 0,
    fake = 1,
    full = 2
  };

  struct {
    bool hints;
    bool gravity;
    bool pull;
    bool draw_light;
    bool bretelles;
    bool fancy;
  } flag;



  solver_type solver;

  std::unique_ptr<tool::control> features;
  
  tool::pga::exclude_type exclude;

  void spin(math::real x = 1.0);

  // TODO move outside ?
  tool::tweak tweak;

  // fancy drawing
  std::unique_ptr< qglviewer::Camera > light;
};

#endif
