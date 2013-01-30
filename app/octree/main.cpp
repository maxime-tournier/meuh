
#include <math/lie.h>
#include <math/tuple.h>

#include <tool/rigid.h>
#include <math/so3.h>
#include <math/vec.h>

#include <geo/octree.h>

#include <tool/simulator.h>
#include <gui/frame.h>
#include <QApplication>

#include <gl/vbo.h>
#include <gl/cube.h>

#include <script/console.h>
#include <script/api.h>

#include <coll/hierarchy.h>
#include <tool/unilateral.h>

#include <core/stl.h>

#include <coll/primitive.h>

#include <geo/obj.h>
#include <core/share.h>

#include <phys/constant.h>

#include <math/func/tuple.h>
#include <math/func/vector.h>
#include <math/func/covector.h>
#include <math/func/coord.h>

#include <math/func/euclid.h>
#include <math/func/gradient.h>
#include <math/func/tuple.h>
#include <math/func/constant.h>

#include <phys/solver/simple.h>
#include <phys/solver/pgs.h>
#include <phys/solver/cholesky.h>
#include <phys/solver/task.h>

#include <gl/sphere.h>
#include <gl/frame.h>
#include <gl/blend.h>
#include <gl/quad.h>

#include <sparse/diag.h>
#include <sparse/transp.h>

#include <math/qp.h>
#include <math/opt/bokhoven.h>


#include <script/meuh.h>

#include <math/random.h>
#include <core/timer.h>

#include <iomanip>

#include <gui/app.h>
#include <core/semaphore.h>



struct viewer : tool::simulator  {
 
  gui::frame* frame, *frame2;
  
  gl::vbo box;

  core::semaphore sema;
  std::mutex mutex;

  tool::unilateral contact;
  
  typedef std::tuple< math::vec3, math::SO3 > dof_type;
  typedef tool::rigid obj_type;

  std::vector< std::unique_ptr<obj_type> > objs;

  gl::vbo sphere;
  geo::aabb aabb;
  
  std::vector<coll::group> groups;

  core::callback on_draw;

  coll::plane ground;
  std::vector<coll::plane> walls;
  
  std::function< void(phys::dof::force& )> pick_force;
  math::vec3 mouse_pos, picked_pos;

  enum solver_type { MINRES = 1, PGS } type;
  math::iter iter;

  phys::dof::velocity last;
  
  math::real height;

  struct {
    bool walls;
    bool warm;
    bool correction;
  } use;



  template<class F>
  void each_obj( const F& f ) {
    core::each( objs, [&]( const std::unique_ptr<obj_type>& obj) {
	f( *obj );
      });
  }


  math::natural add_obj() {
    saveAnimation();
    stopAnimation();
    
    obj_type* obj = new obj_type;
    objs.push_back( std::unique_ptr<obj_type>( obj ) );

    // init state
    auto rand = math::random<math::vec3>::uniform(-1, 1);
    std::get<1>(obj->p() ) = rand().normalized().transpose();
    std::get<0>(obj->p() ) = rand().normalized().transpose();

    std::get<1>(obj->g() ) = height * math::vec3::UnitY();

    // collision model
    auto s = new coll::sphere();
    s->geo.radius = 1.0;
    s->data = obj;
    
    obj->collision.primitive.reset( s );

    data.engine.add( obj );

    // mass
    data.system.mass( obj ).setIdentity();
    data.system.resp( obj ).setIdentity();
    
    // collision update
    obj->update = [&,obj,s] {
      s->geo.center = obj->frame().translation();

      obj->collision.aabb.clear();
      obj->collision.aabb += s->geo.center + s->geo.radius * math::vec3::Ones();
      obj->collision.aabb += s->geo.center - s->geo.radius * math::vec3::Ones();
	  
      aabb += obj->collision.aabb;
    };

    // contact mapping
    contact.map(obj, [obj](const math::vec3& pos, const coll::primitive*) {
	const math::vec3 local = obj->frame().inverse()(pos);
	return tool::rigid::map{ local };
      });
	
    groups.push_back( coll::group() );
    groups.back().primitives.push_back( s );
    

    // picking
    tool::pickable p;
    p.draw = [&,obj] {
      gl::frame(obj->frame())( [&] {
	  sphere.draw();
	});
    };
    
    p.pick = [&,obj](const math::vec3& at) -> math::vec3 {
      mouse_pos = at;
      picked_pos = at;
      
      using namespace math;
      const vec3 local = obj->frame().inverse()(at);
      
      pick_force = [&,local](phys::dof::force& f) {

	auto map = obj_type::map{local};

	// haxxor ! 
	picked_pos = map(obj->g() );
	
	using namespace func;
	auto diff = cst<obj_type::config_type>(mouse_pos) - map;
	
	real stiffness = 1e2;
	auto energy = (0.5 * stiffness) * norm2( diff );
	
	twist storage;
	math::euclid::get(storage, grad(energy)(obj->g() ));
	f(obj) -= storage;
      };
      
      return at;
    };

    p.move = [&] (const math::vec3& pos) { mouse_pos = pos; };
    p.end = [&] { pick_force = 0; };

    picker.add( p );
    restoreAnimation();
    return objs.size();
  }
  
  void init() {
    height = 15;

    type = PGS;
   
    iter.bound = -1;		// psh-psh !
    iter.epsilon = math::epsilon;
    
    frame = new gui::frame;
    frame2 = new gui::frame;

    box.send( gl::cube() );
    
    use.warm = true;
    use.correction = false;
    
    contact.clear();
    on_draw = 0;
    
    ground.geo.set( -5 * math::vec3::UnitY(),
		    math::vec3::UnitY() );
    
    using namespace math;
    real width = 5.5;

    for(natural i = 0; i < 4; ++i) {
      walls.push_back( coll::plane() );
      
      auto normal = i % 2 ? vec3::UnitX() : vec3::UnitZ();
      real sign = i < 2 ? 1 : -1;      
      walls.back().geo.set((-width * sign) * normal, sign * normal);
    }
    
    use.walls = true;
    use.correction = false;
    
    sphere.send( gl::sphere(1.0, 1) );
    reset();

    script::exec("octree.post_init()");

    setSceneRadius( 50 );
    setAnimationPeriod( 0 );
    startAnimation();
    
  }


  void reset() {
    saveAnimation();
    stopAnimation();
    
    aabb.clear();
    
    t = 0;
    on_draw = 0;
    pick_force = 0;
    
    contact.clear();
    objs.clear();
    groups.clear();
    data.clear();
    picker.clear();

    last = phys::dof::velocity();

    player.animator = [&]( math::real t, math::real dt) { 
      using namespace phys;
      using namespace math;
      using namespace func;
    
      coll::hierarchy hierachy(aabb, objs.size());
      
      natural i = 0;
      each_obj( [&](obj_type& obj) {
	  hierachy.add(obj.collision.aabb, &groups[i]);
	  ++i;
	});

      coll::group group;
      
      group.primitives.push_back(&ground);

      if(use.walls) {
	core::each(walls, [&](const coll::plane& p) {
	    group.primitives.push_back(&p);
	  });
      }

      hierachy.add(aabb, &group);

      on_draw = 0;

      auto hits = hierachy.hits();
      contact.update( hits );

      // core::log("contacts:", hits.size() );

      dof::force f;
      
      i = 0;
      each_obj( [&](obj_type& obj) {
	  // potential forces
	  {
	    const math::real mass = 1.0;

	    // translation part of rigid config: element 1 in tuple
	    auto center = tuple_get<1, tool::rigid::config_type >();
	
	    // gravity: Eg = m.g.h
	    auto height = coord<vec3>(1) << center;
	    auto gravity = (mass * phys::constant::g) * height;

	    // potential energy: E = Eg + Es
	    auto potential = gravity;
      
	    // force = -grad E
	    auto force = grad( potential )( obj.g() );
	
	    twist fi;
	    // write result in sparse container
	    math::euclid::get( fi, force );
	    f( &obj ) -= fi;
	  }      
	});
      
      if( pick_force ) pick_force(f);
      

      auto solve = [&](const dof::momentum& p, 
		       const constraint::unilateral::vector& b,
		       math::real eps) {
	dof::velocity v;
	if( !contact.geometry.empty() ) {
	  natural m = data.system.mass.rows().dim();
	  natural n = contact.geometry.rows().dim();
	  natural sub = std::ceil( std::log(m + n));
	
	  switch( type ) {
	  
	  case PGS: {
	  
	    using namespace phys;
	    solver::task task(v);
	  
	    task.momentum = p;
	  
	    struct trivial {
	      void solve(const solver::task& t) const {
		t.velocity = t.momentum;
	      }
	    };
	  
	    auto pgs = solver::make_pgs(trivial(), contact.geometry);
	  
	    pgs.iter = iter;
	    pgs.iter.bound *= sub;
	    pgs.damping = eps;
	    
	    task.unilateral = b;

	    pgs.solve( task );
	  
	  
	  } break;
	  
	  case MINRES: {
	    math::vec storage_p;
      
	    constraint::unilateral::vector storage_j(contact.geometry.rows(), math::vec() );
	    constraint::unilateral::vector mu(contact.geometry.rows(), math::vec());
	    
	    dof::momentum storage_jT( data.system.mass.rows(), math::vec() );
	    dof::velocity vel(data.system.mass.rows(), math::vec() );
	  
	    auto Q = [&](const math::vec& vv) -> const math::vec& {
	  
	      // haxxor !
	      return vv;
	  
	      // dof::velocity vel(data.system.mass.rows(), vv);
	      // sparse::dense( storage_p, data.system.mass.rows(), sparse::diag( data.system.mass) * vel);
	  
	      // return storage_p;
	    };
      
	    
	    auto J =[&](const math::vec& vv) -> sparse::vec_chunk {
	      
	      vel.dense() = vv;
	      sparse::prod(storage_j, contact.geometry, vel);
	      
	      return storage_j.dense();
	    };
	    
	    auto JT =[&](const math::vec& lambda) -> sparse::vec_chunk {
	      mu.dense() = lambda;
	      sparse::prod(storage_jT, sparse::transp(contact.geometry), mu);
	      
	      return storage_jT.dense();
	    };
      
	    // const auto qp = math::lol::simple(Q, J, JT);
	    // const auto qp = math::qp(Q, J, JT);
	    // const auto qp = math::qp(Q, J, JT);
	    
	    auto qp = math::opt::qp::make_bokhoven(Q, J, JT);

	    qp.eps = eps;
	    
      	    math::vec dense_p = sparse::dense( data.system.mass.rows(), p );
	    math::vec dense_b = sparse::dense( contact.geometry.rows(), b);
	    
	    math::vec warm;

	    if( use.warm ) warm = sparse::dense(data.system.mass.rows(), last);
	  
	    core::timer timer;
	    v = sparse::dense( data.system.mass.rows(), qp.solve(-dense_p, dense_b,  iter, warm ) );
	    math::real time = timer.start();
	    
	    last = v;
	    // core::log("solve time:", time, "per sec:", 1/time);
	  } break;
	  }
	} else {
	  solver::task task(v);
	  task.momentum = p;
	  phys::solver::simple(data.system).solve(task);
	}

	return v;
      };

      dof::momentum p = data.engine.momentum(f, dt);
      dof::velocity v = solve(p, constraint::unilateral::vector(), 0);
      p = data.system.mass * v;
      
      mutex.lock();
      
      data.integrate(v, dt);
      
      // damped solve
      if(use.correction) {
	data.engine.correct(solve(dof::momentum(), 
				  contact.corrections, 0.5));
      }
      mutex.unlock();
     


      // update collision models
      aabb.clear();
      each_obj( [&](obj_type& obj) {
	  obj.update();	 
	});
    };
    
    script::exec("hook( simulator.post_reset )");
    restoreAnimation();
  }

  void draw(const geo::aabb& aabb) {
    using namespace gl;
    disable(GL_LIGHTING);
    color(white());
    matrix::push();
    translate( aabb.center() );
    scale( aabb.dim() );
    box.draw(GL_LINES);
    matrix::pop();
    enable(GL_LIGHTING);
  }

  
  
  void draw() {
    using namespace gl;

    // draw( aabb );
    if( on_draw ) on_draw();
    
    matrix::safe([&]{
	color( grey() );
	translate(ground.geo.origin());
	math::quat q;
	q.setFromTwoVectors(math::vec3::UnitZ(), ground.geo.normal() );
	rotate( q );

	scale( sceneRadius() );
	quad(GL_QUADS);
	
      });
    
    if(pick_force) {
      color( yellow() );
      disable(GL_LIGHTING);
      begin(GL_LINES, [&] {
	  vertex(picked_pos, mouse_pos);
	});
      enable(GL_LIGHTING);
    }

    // mutex.lock();
    each_obj( [&](const obj_type& obj) {
	
	cull::enable();
	cull::face( GL_BACK );
	blend()([&] {
	    // color( transp( grey() ) );
	    color( grey() );
	    gl::frame(obj.frame())( [&] {
		sphere.draw();
	      });
	    // draw( obj.collision.aabb );
	  });
	cull::disable();
      });
    // mutex.unlock();


  }


};


int main(int argc, char** argv ) {
 
  gui::app app(argc, argv);

  viewer* wid = new viewer;
      
  script::api api("octree");
  api
    .fun("add", [&] {
	return wid->add_obj();
      })
    .fun("solver", [&](int s) {
	wid->type = viewer::solver_type(s);
      })
    .fun("walls", [&](bool b) {
	wid->use.walls = b;
      })
    .fun("warm", [&](bool b) {
	wid->use.warm = b;
      })
    .fun("correct",[&](bool b) {
	wid->use.correction = b;
      })
    .fun("iter", [&](math::natural i) {
	wid->iter.bound = i;
      })
    .fun("eps", [&](math::real eps) {
	wid->iter.epsilon = eps;
      })
    .fun("height", [&](math::real h) {
	wid->height = h;
      })
    ;

  script::require("octree");
  wid->show(); 
  
  std::cout << std::fixed << std::setprecision(5);
  
  return app.console().exec();

}
