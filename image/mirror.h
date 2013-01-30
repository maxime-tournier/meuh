#ifndef IMAGE_MIRROR_H
#define IMAGE_MIRROR_H

namespace image {

  // extends an image with mirror
  template<class ValueType>
  struct mirror {
    const math::matrix<ValueType>& source;

    typedef ValueType value_type;
    
    value_type operator()(int i, int j) const {
      if( i < 0 ) i = -i;
      if( j < 0 ) j = -j;

      if( i > source.rows() ) i = 2 * source.rows() - i - 1;
      if( j > source.cols() ) j = 2 * source.cols() - j - 1;
      
      return source(i, j);
    }
    
  };

}


#endif
