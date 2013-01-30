#ifndef IMAGE_CONVOL_H
#define IMAGE_CONVOL_H

namespace image {


  template<class Source, class Kernel>
  struct convolution {

    const Source source;
    const Kernel kernel;

    typedef typename Source::value_type value_type;
    const math::Euclid< value_type > euclid;
    
    value_type operator()(int i, int j) const {
      value_type res = E.zero();
      
      const int radius = kernel.radius();
      for(int ki = -radius; ki <= radius; ++ki) {
	for(int kj = -radius; kj <= radius; ++kj) {
	  res = E.sum(res, E.scal(kernel(ki, kj), source( i + ki, j + kj) ) );
	}
      }
      
      return res;
    }
    
  };
  

  template<class Source, class Kernel>
  convolution<Source, Kernel> convol(const Source& src, const Kernel& kern) {
    return {src, kern, math::Euclid< typename Source::value_type >() };
  }
  

}


#endif
