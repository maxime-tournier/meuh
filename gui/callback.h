#ifndef GUI_CALLBACK_H
#define GUI_CALLBACK_H

#include <core/callback.h>
#include <QObject>

namespace gui {

  // qt slot wrapper
  class callback : public QObject { 
    Q_OBJECT;
  public:
    callback();

    core::callback action;
			 
  public slots:
    void trigger();
  };


}


#endif
