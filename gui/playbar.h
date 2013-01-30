#ifndef GUI_PLAYBAR_H
#define GUI_PLAYBAR_H

#include <QWidget>


#include <memory>

#include <tool/anim.h>

struct QPushButton;
struct QSlider;

namespace gui {


  class playbar : public QWidget {
    Q_OBJECT;

    tool::anim::control* control;
    QPushButton* play_pause, *stop;
    QSlider* slider;
    void connect();
    
    bool was_playing;

  public:
    
    playbar(tool::anim::control* control);

  public slots:
      
  private slots:
    void move( double time );
    
    void push();
    void pop();
    
    void seek(int value);

    void toggle_play_pause(bool );
  };
  
}



#endif
