#ifndef TOOL_FRICTION_H
#define TOOL_FRICTION_H

#include <math/types.h>
#include <vector>

#include <phys/contacts.h>

#include <phys/constraint/unilateral.h>
#include <phys/constraint/friction.h>

#include <coll/primitive.h>

namespace tool {

  // delicious copypasta
  class friction : public phys::contacts< coll::hit > {
    
    typedef phys::constraint::unilateral normal_type;
    std::vector< normal_type > normal;

    normal_type::matrix N;
    normal_type::vector n;

    typedef phys::constraint::friction tangent_type;
    std::vector< tangent_type > tangent;

    tangent_type::matrix T;
    
  public:
    
    // builds a world basis - normal is first vector
    static math::mat33 basis(const math::vec3& normal);
    
    // discretized cone tangent directions
    static math::mat discrete(math::natural n = 2);
    
    // planar cone basis
    static math::mat22 cone(math::real mu = 1);
    
    // // applies constraint matrix 
    // void write(constraint_type& out,
    // 	       const phys::dof::vector& v) const;
    
    
    void update(const std::list< coll::hit >& hits);

    void clear();


    struct helper {
      
      const friction& data;
      const sparse::offset< phys::dof::key > dofs;

      const math::natural n;
      const math::real mu;
      const math::mat K, D;
      
      // initial variable/constraints
      struct {
	math::natural vars, cons;
      } offset;
      
      helper(const friction& data,
	     math::natural n = 2,
	     math::real mu = 1.0); 
      
      // additional constraint/variables
      math::natural cons() const;
      math::natural vars() const;
      
      // constraint matrix: adds constraint output given dofs
      void apply(math::vec& out, const math::vec& in) const;

      // constraint transpose matrix: adds dofs output given lagrange multipliers
      void applyT(math::vec& out, const math::vec& in) const;
      
    };

  };

}


#endif
