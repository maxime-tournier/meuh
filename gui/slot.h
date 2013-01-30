#ifndef GUI_SLOT_H
#define GUI_SLOT_H

#include <QObject>
#include <functional>
#include <cassert>

namespace gui {
  
  // TODO BLOAT

  // fugly crap
  struct slot_base : public QObject {
    Q_OBJECT
  public:
    struct unimplemented : std::exception { }; 

    slot_base(QObject* parent = 0) : QObject(parent) { }
    
  public slots:

    virtual void get( double )  { throw unimplemented(); } 
    virtual void get( unsigned int )  { throw unimplemented(); } 
    virtual void get( int )  { throw unimplemented(); } 
    virtual void get() { throw unimplemented(); }
    
  };

  template<class T = void>
  struct slot : public slot_base {

    slot(QObject* parent = 0) : slot_base(parent) { }

    void get( T v) { 
      assert( action );
      action( v );
    }
    
    std::function< void( T ) > action;
  };
  
  template<>
  struct slot<void> : public slot_base {
    
    slot(QObject* parent = 0) : slot_base(parent) { }

    void get() { 
      assert( action );
      action();
    }
    
    std::function< void() > action;
  };

}


#endif
