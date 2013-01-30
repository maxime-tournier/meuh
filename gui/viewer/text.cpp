#include "text.h"

#include <QGLViewer/qglviewer.h>
#include <gl/matrix.h>
#include <gl/translate.h>
#include <gl/color.h>
#include <gl/blend.h>

#include <QGLWidget>
#include <QFontMetrics>

#include <core/debug.h>

namespace gui {
  namespace viewer {


    math::vec3 at(const math::vec3& pos, 
		  const qglviewer::Camera* cam, 
		  const QString& string,
		  const QFont& font) {
      qglviewer::Vec at(pos.x(), pos.y(), pos.z());
      qglviewer::Vec screen = cam->projectedCoordinatesOf(at);

      QFontMetrics fm(font);
      int width = fm.width(string);
      int height = fm.height();
	
      qglviewer::Vec correct = screen;
      correct.x -= width / 2;
      correct.y += height / 2;
	
      qglviewer::Vec world = cam->unprojectedCoordinatesOf(correct);
      return math::vec3( world.x, world.y, world.z);
    }
    
    text::text(const QString& string,
	       const QFont& font ) 
      : string(string),
	font(font)
    {
      
    }
      
    void text::draw(QGLWidget* wid) const {

      QFontMetrics fm(font);
      
      const QSize size = fm.size(Qt::TextSingleLine,  string );
      const int margin = size.height() * 0.9;
      
      const int x = wid->width() - size.width() - margin;
      const int y = wid->height() - margin;

      using namespace gl;

      // shadow: bourrin
      blend()([&] {
	  const math::vec3 dark = math::vec3::Constant(0.0);
	  color( transp(dark) );
	  const int off = 2;

	  wid->renderText(x + off , y + off, string, font );
	  wid->renderText(x + off , y - off, string, font );
	  wid->renderText(x - off , y +off , string, font );
	  wid->renderText(x - off , y -off, string, font );

	});
      
      // actual text
      color( white() );
      wid->renderText(x, y, string, font );
    }

    void text::draw(QGLViewer* wid, const math::vec3& center) const {
      gl::matrix::push();
      gl::translate(at(center, wid->camera(), string, font));
      wid->renderText(0, 0, 0, string, font );
      gl::matrix::pop();
    }

  }
}
