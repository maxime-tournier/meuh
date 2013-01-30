#include "mesh.h"

#include <QApplication>

#include <geo/mesh.h>

#include <geo/obj.h>
#include <geo/obj/file.h>

#include <gl/init.h>
#include <gl/texture.h>
#include <core/resource.h>

namespace gui {
  namespace viewer {


    gl::texture chess;
    


    QPixmap load_texture(const std::string& relative) {
      return QPixmap(core::resource::path(relative).c_str());
    }

    void mesh::draw() {
      if(!mesh_ ) return;
      chess.bind();
      mesh_->draw();

      chess.unbind();

    }

    void mesh::init () { 
      gl::init();
      
      chess.gen( bindTexture(load_texture("gui/texture/chess-light.png"), chess.target()) ).nearest().unbind();
      
      setSceneRadius(400);
    }

    void mesh::load(const QString& path) {
      
      mesh_.reset( new gl::mesh( geo::obj::mesh(path.toStdString())));
      
    }



    int mesh::run(int argc, char** argv) {
      QApplication app( argc, argv );
      
      if( argc < 2 ) return -1;
      
      viewer::mesh view;
      view.load( argv[1] );

      view.show();
      return app.exec();

    }



  }
}

