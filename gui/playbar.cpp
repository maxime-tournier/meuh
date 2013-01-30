#include "playbar.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QSlider>

#include <core/log.h>
#include <core/macro/here.h>

#include <core/share.h>
#include <gui/fix.h>

namespace gui {
  static core::logger log;


  

  playbar::playbar(tool::anim::control* control) 
    : control(control) {

    auto play_icon = core::share("/icon/media-playback-start.png");
    auto stop_icon = core::share("/icon/media-playback-stop.png");

    play_pause = new QPushButton(QIcon( QString(play_icon.c_str()) ), "");
    stop = new QPushButton(QIcon( QString(stop_icon.c_str()) ), "");

    play_pause->setCheckable( true );

    // play_pause->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    // stop->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed);
    
    QWidget* wid = new QWidget;
    QLayout* wid_lay = new QHBoxLayout;

    wid_lay->addWidget(play_pause);
    wid_lay->addWidget(stop);

    wid->setLayout( wid_lay );
    wid->setSizePolicy( QSizePolicy::Minimum, QSizePolicy::Minimum );

    QLayout* vert = new QVBoxLayout;
    slider = new QSlider( Qt::Horizontal );
    slider->setMaximum(5000);
    
    vert->addWidget( slider );
    vert->addWidget( wid );
 
    setLayout( vert );
    
    connect();
    setSizePolicy(  QSizePolicy::Minimum, QSizePolicy::Minimum );


  }


  void playbar::toggle_play_pause(bool value) {
    
    // playing
    if( value ) {
      auto icon = core::share("/icon/media-playback-pause.png");
      play_pause->setIcon( QIcon(icon.c_str()) );
      control->play();
      play_pause->setDown(true);
    } else {
      auto icon = core::share("/icon/media-playback-start.png");
      play_pause->setIcon( QIcon(icon.c_str()) );
      control->pause();
      play_pause->setDown(false);
    }
    
  }
  
  void playbar::connect() {

    QObject::connect(play_pause, SIGNAL(toggled( bool ) ),
		     this, SLOT( toggle_play_pause( bool ) ) );

    QObject::connect(control, SIGNAL(playback( bool ) ),
		     play_pause, SLOT( setChecked( bool ) ) );
    
    QObject::connect(stop, SIGNAL(clicked()),
		     control, SLOT( stop() ) );

    QObject::connect(control->time(), SIGNAL(changed( double )),
		     this, SLOT(move(double )));
  
    QObject::connect( slider, SIGNAL(sliderPressed()),
		      this, SLOT(push()));

    QObject::connect( slider, SIGNAL(sliderReleased()),
		      this, SLOT(pop()));

    QObject::connect( slider, SIGNAL(sliderMoved(int)),
		      this, SLOT(seek(int)));

   
    
  }

  // 
  void playbar::seek(int value) {
    math::real ratio = value / math::real( slider->maximum());

    control->go( control->start() + ratio * control->length() );
  }


  void playbar::move( math::real t ) {
    if(! control->length() ) return;
    
    math::real ratio = (t - control->start()) / control->length();
    // log(macro_here, "ratio:", ratio);
    
    // TODO this should NOT trigger valueCHanged
    slider->setValue( ratio * slider->maximum() );
    

  }

  void playbar::push() {
    was_playing = control->playing();
    control->pause();
  }

  void playbar::pop() {
    control->go( control->time()->get() );
    if( was_playing ) control->play();
  }

}
