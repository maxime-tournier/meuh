#ifndef IMAGE_GAUSSIAN_BLUR_H
#define IMAGE_GAUSSIAN_BLUR_H

namespace image {

  template<class Value, int Radius>
  struct gaussian_kernel {
    
    math::natural radius() const { return Radius; }
    static const math::natural sum = Radius * Radius;
    
    math::real operator()(int i, int j) const {
      // TODO P
    }
    
  };

}



#endif
