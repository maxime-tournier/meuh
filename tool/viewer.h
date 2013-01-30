#ifndef TOOL_VIEWER_H
#define TOOL_VIEWER_H

#include <gl/common.h>
#include <QGLViewer/qglviewer.h>

#include <script/lua.h>
#include <script/api.h>

#include <tool/picker.h>
#include <tool/gl.h>

#include <mutex>
#include <set>

namespace tool {


  // simple viewer + script + some useful stuff
  class viewer : public QGLViewer {
    Q_OBJECT;
    
  public:
    ~viewer();
    viewer(QWidget* parent = 0);
    
    void keyPressEvent(QKeyEvent* e);
    
    void postSelection(const QPoint& point);
    void drawWithNames();
    
    void text(const std::string& value);
    void arrow(const math::vec3& from, 
	       const math::vec3& to,
	       math::real radius = 1,
	       unsigned int subdiv = 12);

    void snapshot(const std::string& where);
    void snapshot_next();
    
    virtual math::real time() const;			
	
  public slots:
    void snapshot_frame(bool value);
    void resize(unsigned int, unsigned int);

  protected:
    script::api api;
    tool::gl gl_api;
    
    tool::picker picker;

    virtual void initializeGL();

    void postDraw();
    
    void saveAnimation();
    void restoreAnimation();

    // loads and bind a texture TODO add format
    GLuint texture(const std::string& relative, GLenum target = GL_TEXTURE_2D);
    
  private:
    QSize size_hint;
    std::mutex mutex;
    std::list< core::callback > todo;

    std::function< void( const math::vec3& ) > held;
    
    bool animation_state;						   
  private slots:
    void flush(bool);
    
  public:
    virtual QSize sizeHint() const;
    void push(const core::callback& );
    
  };


  // TODO duplicate from gui::convert :-/
  math::vec3 convert(const qglviewer::Vec& );
  qglviewer::Vec convert(const math::vec3& );

  
  // TODO obselete ?
  int console_app(int argc, char** argv, std::function< viewer*() > make_viewer);
  
};



#endif
