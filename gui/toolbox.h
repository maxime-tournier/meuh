#ifndef GUI_TOOLBOX_H
#define GUI_TOOLBOX_H

#include <functional>
#include <string>

struct QToolBox;
struct QWidget;
struct QDockWidget;

#include <QObject>

namespace gui {

  class toolbox : public QObject {
    Q_OBJECT
    
    QDockWidget* dock_;
    QToolBox* toolbox_;
      
  public:
    toolbox(const std::string& name, QWidget* parent = 0);
    
    QDockWidget* widget() const;
      
    toolbox& add(const std::string& name, QWidget* wid);
    toolbox& add(const std::string& name, const std::function<void () >& );
      
    void show(bool); 
    
  signals:
    void shown( bool );    
  };
  
}

#endif
