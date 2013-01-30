#ifndef GUI_TWEAK_H
#define GUI_TWEAK_H

#include <gui/slot.h>
#include <memory>


struct QDoubleSpinBox;
struct QSpinBox;
struct QCheckBox;

namespace gui {
  
  template<class T>
  struct tweaker;

  
  template<>
  class tweaker<double> { 
    std::unique_ptr<QDoubleSpinBox> box;
    const double init;
      
  public:
    ~tweaker();
    tweaker(double init);
      
    QWidget* widget();
    gui::slot<double> slot;
  };


  template<>
  class tweaker<unsigned int> {
    std::unique_ptr<QSpinBox> box;
    const double init;
    
  public:
    ~tweaker();
    tweaker(unsigned int init );

    QWidget* widget();
    gui::slot<int> slot;

  };

  template<>
  class tweaker<bool> {
    std::unique_ptr<QCheckBox> box;
    const bool init;
    
  public:
    ~tweaker();
    tweaker(bool init );

    QWidget* widget();
    gui::slot<int> slot;

  };


  template<class T>
  tweaker<T>* tweak(T& var, T value)  {
    var = value;
    
    tweaker<T>* res = new tweaker<T>(value);
    res->slot.action = [&var](const T& val) { var = val; };

    return res;
  }

  template<class T, class OnChange>
  tweaker<T>* tweak(T& var, T value, const OnChange& on_change )  {
    
    auto res = tweak(var, value);
    auto action = res->slot.action;
    res->slot.action = [action,on_change](const T& val) { action(val); on_change(val); };
    
    return res;
  }

}



#endif
