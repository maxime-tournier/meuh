#include "alpha_viewer.h"

namespace gui {



  void alpha_viewer::alpha( double d) {
    alpha_ = d;
  }


  void alpha_viewer::value( int i ) {
    double d = start() + length() * double(i) / (resolution_ - 1);
    if ( d > end() ) d = end();
    if ( d < start() ) d = end();
     
    alpha( d );
  }


  void alpha_viewer::resolution(unsigned int r) { resolution_ = r; }


  alpha_viewer::alpha_viewer( QWidget* parent) 
    : QGLViewer(parent),
      alpha_(0),
      start_(0),
      end_(1),
      resolution_(100)
  {
    
  }
  
  double alpha_viewer::alpha() const { return alpha_; }


  double alpha_viewer::length() const { return end() - start(); }

  double alpha_viewer::start() const { return start_; }
  double alpha_viewer::end() const { return end_; }

  void alpha_viewer::start( double s ) { start_ = s; }
  void alpha_viewer::end( double s ) { end_ = s; }
}
