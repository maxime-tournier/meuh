#include <core/stl.h>
#include "ik_viewer.h"


// bli 
#include <gl/skeleton.h>
#include <mocap/curves.h>
#include <gui/viewer/text.h>

#include <math/vec.h>
#include <math/lie/pga.h>

#include <phys/skel/absolute.h>

#include <math/func/jacobian.h>
#include <phys/skel/pga.h>
#include <math/func/SE3.h>

#include <core/macro/here.h>
#include <core/macro/debug.h>

#include <gl/common.h>
#include <gl/line.h>
#include <math/func/coord.h>

#include <QMouseEvent>
#include <math/SE3.h>

#include <math/algo/pgs.h>
#include <math/lcp.h>

namespace gui {

  ik_viewer::ik_viewer(QWidget* parent) 
    : viewer::fancy(parent) { 
    
    ngeo = 5;
    ik = false;
  }
  
  void ik_viewer::init() {
    viewer::fancy::init();
    gl::skeleton::init();

    startAnimation();
    time.start();
  }

  void ik_viewer::drawWithNames() {
    if(! pack ) return;

    gl::skeleton( gl::skeleton::full ).draw_center( pack->skeleton.topology, 
						    math::body(pose, body) );
        
  }


  void ik_viewer::update_target(int x, int y) {
    qglviewer::Vec loc(x, y, dist);
    qglviewer::Vec tmp = camera()->unprojectedCoordinatesOf(loc);
    target = math::vec3(tmp.x, tmp.y, tmp.z);
  }
  
  void ik_viewer::postSelection(const QPoint& point) {
        
    
    // find the selectedPoint coordinates, using camera()->pointUnderPixel().
    bool found;
    qglviewer::Vec selectedPoint = camera()->pointUnderPixel(point, found);
    
    
    ik = found && (selectedName() >= 0) && (selectedName() < pose.rows() ) ;
    
    if( ik ) {

      // local selection coords
      dist = camera()->projectedCoordinatesOf(selectedPoint).z;
      update_target(point.x(),  point.y());

      selected = pose( selectedName() ).inverse() ( target );

    }
  }



  void ik_viewer::mouseMoveEvent(QMouseEvent *e)
  {
    if (ik && (e->buttons() &  Qt::LeftButton )){
      update_target(e->x(), e->y());
    } else  {
      QGLViewer::mouseMoveEvent(e);
    }
  }

  void ik_viewer::mouseReleaseEvent(QMouseEvent *e)
  {
    if (ik && (e->button() == Qt::LeftButton) ){
      ik = false;
    } else {
      QGLViewer::mouseMoveEvent(e);
    }
  }
  

  void ik_viewer::keyReleaseEvent(QKeyEvent* e)
  {
    // Defines the Alt+R shortcut.
    if (ik && (e->key() == Qt::Key_Shift) )
      {
	ik = false;

      } else {
    
      QGLViewer::keyReleaseEvent(e);
    }
   
  }


  void ik_viewer::keyPressEvent(QKeyEvent* e)
  {
    if (e->key() == Qt::Key_R )
      {
	reset();
      } else {
      
      QGLViewer::keyPressEvent(e);
    }
   
  }


  void clamp(math::vec& data, math::real max = 1) {
    data.each([&](math::natural i) {
	if( data(i) > max ) { data(i) = max; }
	if( data(i) <  -max ) {  data(i) = -max; }

      });
  }




  math::mat constraint_matrix(const math::vec& coords, math::real max = 1) {
    using namespace math;
    std::vector<natural> up, down;
    
    const natural n = coords.rows();
    
    coords.each([&](natural i) {
	if( coords(i) >=  max ){  
	  up.push_back(i);
	}
	
	if( coords(i) <=  -max ) { 
	  down.push_back(i);
	}
      });

    const natural m = up.size() + down.size();
    mat res;

    if( ! m ) return res;
    res.resize(m, n);
    
    for(natural i = 0; i < up.size(); ++i) {
      res.row(i) = -vec::Unit(n, up[i]).transpose();
    }

    for(natural i = 0; i < down.size(); ++i) {
      res.row(up.size() + i) = vec::Unit(n, down[i]).transpose();
    }
    
    mat true_res(res.rows(), 6 + res.cols());
    true_res << mat::Zero(res.rows(), 6), res;
    
    return true_res;
  }


  



  void ik_viewer::animate() {
        
    if(!pack) return;

    using namespace math;
    
  
    
    using func::operator<<;
    auto f = phys::skel::pga( pga );
    auto g = phys::skel::absolute(pack->skeleton.topology) << f;
    
    auto E = f.dom();
    auto G = f.rng();
    auto e = E.alg();
    
    // jacobian
    const mat Jack = func::J(g , E , G) ( param );
    
    mat R;
    R.resize(Jack.rows(), Jack.cols());
    R.each_col([&](natural j) {
	R.col(j) = pack->skeleton.Mc( Jack.col(j).eval() );
      });
    
    // mass 
    const mat M = Jack.transpose() * R;
      // + mat::Identity(Jack.cols(), Jack.cols());
    // const mat M = Jack.transpose() * Jack;
    
    auto inv = M.lu();
    auto Minv = [&](const vec& x) -> vec { 
      return inv.solve( x );
    };
    
    const natural n = pack->skeleton.size;

    // forces
    vec force = vec::Zero(6*n);
  
    if(ik) {
      natural dof = selectedName();
      vec3 point = pose(dof)( selected );
      vec3 diff = target - point;
      
      math::real k = 2;
      math::force fi = k * diff.transpose();
      
      func::absolute<> map(selected);
      
      const SE3& at = pose(dof);
      math::wrench wi = map( math::body( map, at, fi) ).body();
      force.segment<6>(dof * 6) = wi.transpose();
    }
    


    vec inertia = pack->skeleton.Mc( e.coords(bparam) );
    inertia.resize(6 * n);
    body.each([&](natural i ) {
	twist chunk = inertia.segment<6>(6*i);
	angular(chunk) = angular(body(i)).cross( angular(chunk ) );
	linear(chunk) = angular(body(i)).cross( linear(chunk ) );
	inertia.segment<6>(6*i) = chunk;
      });
    

    real dt = 1; 
    
    // ext force
    vec ext = Jack.transpose() * ( dt *force );

    // 1st order + inertia
    ext += M * e.coords(bparam) - dt * inertia;
    

    // // constraints
    mat J = constraint_matrix( f.coords(param), 0.02);
    vec v;
    if( !J.empty() ) {

      mat R; 
      R.resize(ext.rows(), J.rows());
      R.each_col([&](natural j) {
	  R.col(j) = Minv(J.row(j).transpose());
	});

      mat K = J*R;
      vec b =  -J*Minv(ext);


      math::lcp lcp(K, b);
      vec lambda = algo::pgs(lcp, math::epsilon).solve( algo::stop().it(2 * K.rows()) );
      
      // spring
      // impulse.tail(ngeo) -= dt * f.coords(param);
    
      // dynamics
      v = Minv( ext + J.transpose() * lambda);
    } else {
      v = Minv( ext );
    }
    
    bparam = e.elem(v);
    
    // integration
    using lie::operator*;
    param = param * E.exp(e.scal(dt, bparam));
    
    
    auto dpose = g( math::body(param, bparam) );
    
    pose = dpose.at();
    body = dpose.body();
    
  
  }



  void ik_viewer::draw() {
    using namespace gl;
    
    if(!pack) return;

    
    auto content = [&]() {
      // none.bind();
      color( white() );
      skeleton( skeleton::full ).draw_center( pack->skeleton.topology, 
					      math::body(pose, body) );
    };

    viewer::fancy::draw( content );

    if( ik ) {
      color(yellow());
      line_width(5);
      disable(GL_LIGHTING );
      line( pose(selectedName()) ( selected  ), target );
       
      enable(GL_LIGHTING );
    }
    
    
  }
  
  void ik_viewer::reset() { 
    if( !pack ) return;

    using math::func::operator<<;
    auto f = phys::skel::pga( pga );
    auto g = phys::skel::absolute(pack->skeleton.topology) << f;

    std::get<0>(param) = pack->curves.joint( mocap::start( pack->mocap.clip.data ) ) (0 );
    std::get<1>(param) = math::vec::Zero( ngeo );
    
    bparam = math::lie::of(param).alg().zero();

    pose = g(param);
    body = math::lie::of(pose).alg().zero();
    
  }




  std::vector<math::natural> to_remove(const phys::skel::pack& pack) {
    std::vector<math::natural> res;
    
    std::vector<std::string> names = { "rtoes", "ltoes", 
				       "lfingers", "rfingers",
				       "lthumb", "rthumb",
				       "lhand", "rhand" };
    
    for(math::natural i = 0; i < names.size(); ++i) {
      const math::natural p = pack.find( names[i] )->get().id;
      res.push_back( p );
    }
    
    return res;
  }

  
  
  void ik_viewer::load(const QString& filename ) {
    
    pack.reset( new phys::skel::pack(filename.toStdString()) ); 
    
    using namespace math;
    
    auto data = pack->mocap.clip.sampling.sample( pack->curves.joint );
    
    
    data.each([&](natural i) { 
	if( i == 0 ) return;

	core::each( to_remove(*pack), [&](natural j) {
	    data(i)(j) = data(0)(j);
	  });

	data(i).each([&](natural j) {
	    data(i)(j) = SE3::rotation( data(i)(j).rotation() );
	  });
	
      });
    
    pga.compute(data, ngeo, algo::stop().eps(1e-10).it(20), 
		lie::of( data(0) ) );
    
   
    reset();
  }


}
 
