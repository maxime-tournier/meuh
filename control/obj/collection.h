#ifndef CONTROL_OBJ_COLLECTION_H
#define CONTROL_OBJ_COLLECTION_H

#include <control/objective.h>
#include <memory>

namespace control {
  namespace obj {

    class collection : public objective {
      std::vector< std::unique_ptr<objective> > objs_;
      math::natural dim_;
    public:

      collection();
      
      collection& add( objective* );
      math::natural dim() const;
      
      math::vec operator()(const phys::dof::velocity& v,
			   const phys::dof::momentum& a) const;

    };

  }
}


#endif
