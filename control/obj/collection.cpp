#include "collection.h"

#include <math/vec.h>

namespace control {
  namespace obj {

    collection::collection() : dim_(0) { }

    math::natural collection::dim() const { return dim_; }


    collection& collection::add(objective* obj) {
      dim_ += obj->dim();
      objs_.push_back( std::unique_ptr<objective>(obj) );
      return *this;
    }

    math::vec collection::operator()(const phys::dof::velocity& v,
				     const phys::dof::momentum& a) const {
      math::vec res; res.resize( dim());

      math::natural off = 0;
      core::each(objs_, [&](const std::unique_ptr<objective>& obj) {
	  math::natural n = obj->dim();
	  res.segment(off, n) = (*obj)(v, a);
	  off += n;
	});
      return res;
    }
    
  }
}
