#ifndef ALPHA_VIEWER_H
#define ALPHA_VIEWER_H


#include <gui/frame.h>
#include <QGLViewer/qglviewer.h>

namespace gui {

  class alpha_viewer : public QGLViewer {
    Q_OBJECT

    double alpha_;
    double start_;
    double end_;

    unsigned int resolution_;


  public:
    alpha_viewer(QWidget* parent = 0);
    double alpha() const;

    double start() const;
    double end() const;

    void start( double );
    void end( double );

    double length() const;

    void resolution(unsigned int r);

  public slots:
    void alpha( double );	
    void value( int );	

  };

}

#endif
