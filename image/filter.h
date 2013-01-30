#ifndef IMAGE_FILTER_H
#define IMAGE_FILTER_H

namespace image {


  template<class Image, class Filter>
  void filter(Image& dest, const Filter& f) {
    
    dest.each([&](math::natural i, math::natural j) {
	dest(i, j) = f(i, j);
      });
    
  }

}


#endif
