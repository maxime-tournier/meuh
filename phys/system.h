#ifndef PHYS_SYSTEM_H
#define PHYS_SYSTEM_H

#include <sparse/matrix.h>
#include <sparse/vector.h>

#include <map>

#include <phys/constraint/bilateral.h>
#include <phys/constraint/unilateral.h>
#include <phys/constraint/friction.h>
#include <phys/constraint/cone.h>

#include <phys/dof.h>

#include <list>
#include <memory>

namespace phys {

  struct system {
    
    dof::mass mass, resp;
    
    struct constraint_type {
      
      struct bilateral_type {
	std::list< std::unique_ptr< const constraint::bilateral > > storage;
	
	constraint::bilateral::matrix matrix;
	constraint::bilateral::vector values, corrections;

	constraint::bilateral::vector damping;

	constraint::bilateral::pack pack() const;
      } bilateral;

      struct unilateral_type {
	std::list< std::unique_ptr< const constraint::unilateral > > storage;
	
	constraint::unilateral::matrix matrix;
	constraint::unilateral::vector values;
	
	constraint::unilateral::vector damping;

	constraint::unilateral::pack pack() const;
	
      } unilateral;
      
      struct friction_type {
	std::list< std::unique_ptr< const constraint::friction > > storage;

	constraint::friction::matrix matrix;
	constraint::friction::bounds bounds;

	constraint::friction::vector damping;
	
	constraint::friction::pack pack() const;
	
      } friction;

      phys::constraint::cone::set cones;
      
      std::map< phys::constraint::cone::key,
		phys::constraint::unilateral::key > discrete;
      
      void check() const;

      void discretize(math::natural n);

      // test
      phys::constraint::unilateral::vector rhs(const phys::dof::velocity& ) const;

    } constraint;
    

    phys::dof::vector damping, stiffness;
    
 
    void check() const;

    void clear();
   
    
  };


}



#endif
