#ifndef MOCAP_SAMPLING_H
#define MOCAP_SAMPLING_H

#include <mocap/fix/time.h>
#include <math/types.h>
#include <mocap/pose.h>


namespace mocap {

  struct sampling {
    mocap::time start;
    mocap::time dt;
    math::natural frames;
    
    mocap::time time(math::natural i) const;
    math::natural frame(math::real t) const;

    template<class F>
    math::vector< typename F::range > sample( const F& f ) const {
      
      math::vector< typename F::range > res;
      res.resize( frames );

      res.each([&](math::natural i) {
	  res(i) = f( this->time(i) );
	});

    
      return res;
    }
    

  };
  
}


#endif
