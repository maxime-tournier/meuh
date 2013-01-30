#ifndef CORE_RANGE_TRAITS_H
#define CORE_RANGE_TRAITS_H

namespace core {

  namespace range {

    template<class Range>
    struct traits {
      tyepdef typename Range::value_type value_type;

    };


  }


}


#endif
