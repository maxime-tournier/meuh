#ifndef CONTROL_SYSTEM_H
#define CONTROL_SYSTEM_H

#include <control/actuator.h>
#include <control/feature.h>
#include <list>
#include <memory>

namespace control {

  struct system {

    struct actuator_type {
      control::actuator::matrix matrix;
      control::actuator::vector bounds;
      
      control::actuator::pack pack() const;
      
      control::actuator::vector clamp(const control::actuator::vector& ) const;

      void clear();
      void check() const;
    } actuator;
    

    struct feature_type {
      std::list< std::unique_ptr< const control::feature > > storage;
      
      control::feature::matrix matrix;
      control::feature::vector values;
      
      void clear();
      void check() const;
      
    } feature;



    void clear(); 
    void check() const;

  };

}



#endif
