#include "fancy.h"

#include <gl/reflect.h>
#include <gl/init.h>
#include <gl/common.h>
#include <gl/quad.h>
#include <gl/blend.h>
#include <gl/fbo.h>
#include <gl/rbo.h>
#include <gl/texture.h>
#include <gl/light_map.h>

#include <gl/shader.h>
#include <gl/program.h>
#include <gl/uniform.h>

#include <core/io.h>
#include <core/resource.h>

#include <gl/outline.h>
#include <gui/convert.h>

namespace gui {

  namespace fancy {

    gl::fbo fbo(2048, 2048); 
    gl::texture dep(GL_TEXTURE1);
   
    gl::shader::fragment diffuse, specular, light_map, cell, fog;
    struct {
      gl::shader::vertex vert;
      gl::shader::fragment frag;
    } main;
  
    gl::program prog;

    gl::outline outline;

  
    static QPixmap load_texture(const std::string& relative) {
      return QPixmap(core::resource::path(relative).c_str());
    }
    
    void init(QGLViewer* wid, qglviewer::Camera* light) {
      
      gl::init();
      gl::enable( GL_NORMALIZE );
      
      gl::enable( GL_TEXTURE_2D );
       
      
      math::vec3 background = 0.14 * gl::white();
      gl::clear_color(background);
    
      gl::fog::mode(GL_EXP2);
      gl::fog::color(background);
      gl::fog::density(0.00150);
        
      gl::shade::smooth();
      
      gl::fog::enable();
      
      glActiveTexture(GL_TEXTURE1 );
      gl::enable( GL_TEXTURE_2D );
    
      dep.gen().bind().linear().compare().clamp();
      dep.send( gl::texture::data::depth().width( fbo.width()).height( fbo.height() ) ).unbind();

     
      fbo.gen().bind().attach_depth( dep );
      glDrawBuffer(GL_NONE);
      glReadBuffer(GL_NONE);
      fbo.check().unbind();

      diffuse.init("diffuse.frag");
      specular.init("specular.frag");
      light_map.init("light_map.frag");
      cell.init("cell.frag");
      fog.init("fog.frag");

      // TODO main.vert/frag should not lie in share/
      main.vert.init("main.vert");
      main.frag.init("main.frag");
      
      prog.gen().attach(diffuse, 
			specular, 
			light_map, 
			cell,
			fog,
			main.vert, 
			main.frag).link();
      
      
      prog.enable().uniform("light_map") = dep;
      prog.enable().uniform("scaling") = math::vec3(1, -1, 1);
      prog.enable().uniform("tex") = 0;
      prog.disable();
      
      outline.init();
      glActiveTexture(GL_TEXTURE0 );
     

      wid->setManipulatedFrame( light->frame() );
      wid->setSceneRadius( 500 );

      light->setPosition( qglviewer::Vec(0, 100, 0) );
      light->setSceneRadius( 40.0 );
      light->setZClippingCoefficient( 2 );
      light->setAspectRatio( 1.0 );

    }


    void cast(qglviewer::Camera* camera,
	      qglviewer::Camera* light,
	      const std::function< void ( void ) >& content ) {
      gl::matrix::push();
      
      // TODO move this outside
      // light->setSceneCenter( camera->revolveAroundPoint() );
      light->setFOVToFitScene();
      gl::light::get(0).position( gui::convert( light->position() ));

      using namespace gl;
      
      
      // 1ere passe: depuis la lumiere dans le fbo
      light->loadProjectionMatrix();
      light->loadModelViewMatrix();

      fbo.render( [&]() {
      	  gl::clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      	  cull::enable();
      	  cull::face(GL_FRONT);
      	  content();
      	  cull::face(GL_BACK);
      	  cull::disable();
      	});
      
      gl::clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      gl::matrix::pop();
    }

    

    void draw(qglviewer::Camera* camera,
	      qglviewer::Camera* light,
	      const std::function< void ( void ) >& content ) {
      gl::matrix::push();
      
      // 2eme passe: point de vue normal
      camera->loadProjectionMatrix();
      camera->loadModelViewMatrix();
      
      struct { math::mat44 cam, light; }  mat;
      
      light->getModelViewProjectionMatrix(mat.light.data());
      camera->getModelViewMatrix(mat.cam.data());
      
      gl::matrix::mode(GL_TEXTURE);
      gl::matrix::push();
      gl::light_map::texture_matrix(dep, mat.cam, mat.light.data());
      
      prog.draw( content );
      
      // outline.draw( content );
      gl::matrix::mode(GL_MODELVIEW);
      gl::matrix::pop();

      gl::matrix::mode(GL_TEXTURE);
      gl::matrix::pop();

      gl::matrix::mode(GL_MODELVIEW);
    }

  }



  namespace viewer { 
  
    // fancy::fancy(QWidget* parent) : QGLViewer(parent) { }

    // void fancy::spot(const math::vec3& target) {
    //   qglviewer::Vec tmp(target.x(), target.y(), target.z());
      
    //   light->lookAt( tmp );
    // }
    

    // void fancy::init() {
    //   gui::fancy::init(this);
      
    //   light.reset( new qglviewer::Camera );
    //   light->setPosition( qglviewer::Vec(0, 100, 0) );
    //   light->setSceneRadius( 40 );
    //   light->setZClippingCoefficient(2);
    //   light->setAspectRatio(1.0);
     
    // }


    // void fancy::fog(bool value) {
    //   if(value) {
    // 	gl::fog::enable();
    //   } else {
    // 	gl::fog::disable();
    //   }
    // }

    




    // void fancy::draw(const content_type& content) const {
    //   gui::fancy::draw(camera(), light.get(), content);
    // }



  }
 
}
