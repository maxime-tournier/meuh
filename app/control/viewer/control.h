
#ifndef GUI_VIEWER_CONTROL_H
#define GUI_VIEWER_CONTROL_H

#include <math/vec.h>
#include <gui/viewer/fancy.h>

#include <tool/mocap.h>
#include <tool/angular_limits.h>
#include <tool/pga.h>
#include <tool/markers.h>

#include <QGLViewer/qglviewer.h>
#include <gui/frame.h>

#include <vector>
#include <memory>

#include <mocap/ik/handle.h>

#include <gl/texture.h>
#include <math/lie/pga.h>

#include <QTime>
#include <QMutex>

#include <phys/system.h>
#include <gui/toolbox.h>
#include <gui/push.h>

#include <math/random.h>

#include <phys/system.h>
#include <control/system.h>

#include <phys/constraint/bilateral.h>

#include <gui/model.h>
#include <geo/plane.h>

#include <control/obj/collection.h>
#include <control/pd.h>

#include <list>

#include <tool/character.h>
#include <tool/track.h>

#include <gui/playbar.h>

#include <core/actions.h>
#include <gui/menu.h>


#include <tool/model.h>
#include <tool/rigid.h>
#include <tool/picker.h>

#include <script/engine.h>
#include <script/api.h>

#include <tool/trajectory.h>
#include <tool/viewer.h>


namespace gui {
  
  namespace viewer {

    class control : public tool::viewer  {
      QWidget* parent;
      std::set<gui::frame* > frames;
      // std::vector<mocap::ik::handle> handles;

      // std::unique_ptr<phys::skel::pack> tracking, learning;
      std::unique_ptr<tool::mocap> tracking, learning;
      std::unique_ptr<qglviewer::Camera> light;
    public:
      typedef tool::character::reduced object_type;
      // typedef character::ragdoll object;
    private:
      std::unique_ptr<object_type> obj;
      
      typedef tool::track::engine< object_type::conf_type > tracker_type;
      tracker_type tracker;
      
      Q_OBJECT

      phys::rigid::config pose;
     
      // struct ik_handle { 
      // 	math::natural index;
      // 	math::vec3 local;
	
      // 	const phys::constraint::bilateral* constraint;
      // 	math::real weight;
      // 	bool tracking;
      // };


      std::unique_ptr<tool::rigid> box;

      typedef std::map< Qt::MouseButton, std::map< Qt::KeyboardModifiers, std::function<void(QMouseEvent*) > > > mouse_events;
      mouse_events on_mouse_press, on_mouse_release, on_mouse_move;
      
      struct panic : std::exception { };
      
      void popup(QWidget* , const QString& title);

      // index map: pack => tracking
      std::map<math::natural, math::natural> tracking_map(const tool::mocap& pack) const;

    public:
      gui::menu character_menu;
      
      // std::map< gui::frame*, ik_handle > handles;
    private:
      
      struct {
	std::unique_ptr<gui::playbar> playbar;
	std::unique_ptr<tool::anim::control> control;
      } _time;
       

      std::map<std::string, tool::model::full> models;
      
      math::real laziness, reflexes;
      math::real reference;
      
      // global
      math::vec handle_forces() const;

      // void ik_constraints_obj(phys::system& system, ::control::obj::collection& , math::real );
      void ik_constraints(tool::track::engine<object_type>&, math::real);
      
      // void ik_constraints(phys::system& system, phys::dof::metric& , math::real);
      // void com_constraints(phys::system& system, ::control::system&, math::real);
      void contact_constraints(phys::system& system, math::real);
      void limit_constraints(phys::system&, math::real);
      void fixed_constraints(phys::system&, math::real);

      void balance_objective(::control::system&);
      void reference_objective(::control::system&);
      void contact_objective(::control::system&, const phys::system& , const phys::dof* );

      void safety_objective(phys::dof::metric&,  math::real);
      void safety_objective_obj(::control::system&,  math::real);

      void setup_actions(core::actions& actions,
			 tool::track::chunk_base< object_type::conf_type >* chunk,
			 tool::track::target_base* target);


      void track(::control::system& , const phys::dof*, 
		 const math::mat&, const math::vec& , math::real weight = 1.0);

      std::unique_ptr< tool::limits_viewer > limits_viewer;
      
      struct {
	std::list< std::unique_ptr<phys::constraint::friction> > tangent;
	std::list< std::unique_ptr<phys::constraint::unilateral> > normal;
	std::list< std::unique_ptr<phys::constraint::cone> > cone;
	
	std::list<math::natural> dofs;

	std::unique_ptr< math::vec3 > center;
	math::real mu;
      } contacts;
      
      struct {
	std::list< phys::constraint::unilateral > constraint;
	std::unique_ptr< tool::angular_limits::result_type > data;
      } limits;

      void setup_limits();

      // forces sum
      phys::dof::momentum impulses(const phys::system& system, math::real dt) ;

      void setup(phys::system& system, ::control::system& ,math::real dt);

      // ik constraint
      math::mat handle_matrix() const;
      math::vec handle_values(math::real dt) const;
      
      bool reset_needed;

      // tool::markers ik() const;
           
      math::real dt, kappa, epsilon, gamma, lazy, strength;
    
      math::natural unactuated;

    public:
      struct com_type {
	gui::frame* handle;
	math::real weight;
	
	bool orient_control;
	bool automatic;

	bool tracking;

	const phys::constraint::bilateral pos, orient;
	com_type();
	

      } com;
      
      ::control::pd<math::vec3> pd;
    private:
      void com_add();
      void com_del();
      void com_auto();
     
      // math::mat com_matrix() const;
      // math::vec com_values(math::real) const;
      
      // math::mat orient_matrix() const;
      // math::vec orient_values(math::real) const;
      
      // void keyframe_pos(gui::frame* f, const math::func::any<math::real, math::vec3 >& c);
      // void keyframe_full(gui::frame* f, 
      // 			 const math::func::any<math::real, math::vec3 >& pos,
      // 			 const math::func::any<math::real, math::SO3 >& orient);

      // void keyframe_ik(gui::frame* f, const ik_handle& h); 
      // void keyframe_com(gui::frame* f);

      void keyframe_start();
     	
      math::real keyframe_speed;

      void start_pose();

      void reset_object(math::natural n);
      void reset_tracker();
      
      math::lie::pga< tool::pga::inner > pga;
      math::lie::pga< math::vector<math::vec3> > euler_pca;
      
      QMutex mut_reset;

      // returns 99% variance geodesic count
      math::natural learn(const tool::mocap& );
      
      std::list< core::callback > to_draw;

      gui::push push;
      
      
      struct {
	bool push_mode;
	bool enforce_limits;
	bool inertia;
	bool contacts;
	bool show_hints;
	bool balance;
	bool bretelles;
	bool momentum;
	bool gravity;
	bool control;
	bool relax;
	bool travelling;
	bool show_momenta;
	bool fixed;
	bool friction;
	bool euler;
	bool tracking;
	bool correct;
	bool nappe;
	bool trace;
      } flags;


      struct {
	math::real am;
	math::real com_height;
      } weight;

      math::real stance;

      math::real keep_contact;

      math::random<math::real>::generator random_angle;
      
      // gui::model model;
      void setup_models();
            
      geo::mesh adjust(const phys::rigid::body& , const geo::mesh& , math::real factor = 1.0) const;

      geo::plane ground;
      
      std::function< void () > collisions;

      tool::picker picker;

      typedef std::vector< std::pair<math::real, math::real> > kinetic_type;
      kinetic_type kinetic;
      void write_kinetic();

      typedef std::vector< std::pair<math::real, math::vec3> > angular_type;
      angular_type angular;
      void write_angular();

      typedef std::vector< std::pair<math::real, math::vec3> > linear_type;
      linear_type linear;
      void write_linear();



      

      phys::dof::momentum unilateral;

      void apply_push(math::natural dof, const math::vec3& local, bool random = true);

      // void snapshot(const std::string& where);

    public:
      gui::toolbox animation, controller, params;
      
      struct handle {
	tool::track::feature<math::vec3>* feature;
	tool::track::flex<math::vec3>* target;
	tool::track::settings* settings;
      };

      math::real time() const;

      handle add_handle(const math::func::any< phys::rigid::config, math::vec3 >&, 
			const std::string& name);
      
      handle add_handle(math::natural index, const math::vec3& local);
      handle add_com();

      void lookat();

      math::natural ngeo;
    
      EIGEN_MAKE_ALIGNED_OPERATOR_NEW
      public:
      control(QWidget* parent = 0);
      ~control();

      void init();

      void draw();
      void drawWithNames();
      void postSelection(const QPoint& point);
    
      // void add_handle(mocap::skeleton*);
      void animate();
	
      // void mouseMoveEvent(QMouseEvent *e);

      bool handle_mouse_event(const mouse_events&, QMouseEvent* );

      void mouseReleaseEvent(QMouseEvent *e);
      void mousePressEvent(QMouseEvent *e);

      // void keyReleaseEvent(QKeyEvent* e);
      void keyPressEvent(QKeyEvent* e);

      void make_ball();
      void shoot_ball(const math::vec3& from, 
		      const math::vec3& to, 
		      math::real vel = 1.0);

      // plays tracking file
      void playback(math::real t);

      enum {
	normal,
	menu,
	shoot
      } selection_mode;

      script::api app;
      
      std::string selected;
      
      // init script engine
      void init_script();
	
		   
    public slots:
      void reset();
      
      void track( const QString& );
      math::natural learn( const QString& );

      void load( const QString& );

  
      void walk();
      
      // void save_one_snapshot(bool);

    signals:
      void walk_needed();
      void load_needed( const QString& );
    private:

      // struct {
      // 	std::string string;
      // 	math::natural frames;
      // } _text;

      std::list< tool::trajectory > trajectories;
      std::list< tool::trajectory >::iterator edited;
      
      math::vector<math::SO3> euler_offsets;
      
    };
 
    void toggle(core::actions& actions, const std::string& on, const std::string& off);
   
  }
}

#endif
