#ifndef MATH_FUNC_FAIL_H
#define MATH_FUNC_FAIL_H

#include <math/error.h>
#include <math/types.h>

namespace math {
  namespace func {

    template<class Domain, class Range>
    struct error {
      
      std::string message;
      
      typedef Domain domain;
      typedef Range range;
      
      error( const std::string& message = std::string() ) : message(message) { }
      
      template<class Of, class At>
      error(const Of& of, const At& ) : message(of.message) { }
      
      
      range operator()(const domain& ) const {
	throw math::error( message );
      }

      typedef error< typename math::lie::group<domain>::algebra,
		     typename math::lie::group<range>::algebra > push;
    
      typedef error< typename math::lie::group<range>::coalgebra,
		     typename math::lie::group<domain>::coalgebra > pull;

    };

  }
}


#endif
