#include "octree.h"

namespace geo {

  
  static std::string conv_base(unsigned long v, long base)
  {
    std::string digits = "0123456789abcdef";
    std::string result;

    if((base < 2) || (base > 16)) {
      result = "Error: base out of range.";
    }
    else {
      do {
	result = digits[v % base] + result;
	v /= base;
      }
      while(v);
    }
    return result;

  }


  
  static constexpr math::natural width = sizeof( octree::id_type ) * 8;
  static constexpr octree::id_type max_value = 1 << octree::max_depth;

  static constexpr math::natural junk = width % octree::max_depth;
  
  octree::id_type octree::id(const math::vec3& pos) const {
    
    const math::vec3 relative = 
      (pos - aabb.lower).cwiseProduct( aabb.dim().cwiseInverse() )
      .cwiseMin( math::vec3::Constant( 1 - math::epsilon ) ) 
      .cwiseMax( math::vec3::Zero() ) ;
      
    const id_type res = 
        id_type( relative.x() * max_value ) << (width - max_depth)
      | id_type( relative.y() * max_value ) << (width - 2 * max_depth)
      | id_type( relative.z() * max_value ) << (width - 3 * max_depth);
    
    return res;
  }

  // http://dmytry.com/texts/collision_detection_using_z_order_curve_aka_Morton_order.html
  octree::id_type octree::morton(id_type id) {
    id_type res = 0;

    const id_type mask = 1;

    for(math::natural i = 0; i < 3; ++i) {
      id_type chunk =  (id << i * max_depth) >> (width - max_depth);

      for(math::natural k = 0; k < max_depth; ++k) {
	res |= (chunk & mask) << ( junk + k * 3 + (2 - i) );
	chunk >>= 1;
      }
    }

    return res;
  }
  

  aabb octree::cell(id_type id, math::natural depth) const {
    
    math::vec3 relative;
    
    const id_type max = 1 << depth;
    
    for(math::natural i = 0; i < 3; ++i ) {
      id_type chunk =  (id << i * max_depth) >> (width - max_depth);
      chunk >>= (max_depth - depth);
      relative(i) = math::real(chunk) / max; 
    }
    
    const math::vec3 radius = aabb.radius() / (1 << depth);
    
    geo::aabb res;
    res.lower = aabb.lower + relative.cwiseProduct( aabb.dim() );
    res.upper = res.lower + 2 * radius;

    return res;
  }
  
  static constexpr octree::id_type full = (1 << octree::max_depth) - 1;
  
  octree::id_type octree::neighbor(id_type id, offset_type off) {
    
    id_type res = 0;

    for(math::natural i = 0; i < 3; ++i) {
      id_type chunk = (id << i * max_depth) >> (width - max_depth);
      
      const int delta = (&off.x)[i];
      
      chunk = chunk + delta * (1 << (max_depth - off.depth));
      
      if( int(chunk) < 0) chunk = 0;
      if( chunk > full ) chunk = full; 
      
      res |= chunk << (width - (i + 1) * max_depth);
    }    
    return res;
  }
  
  octree::id_type octree::clamp(id_type id, math::natural depth)  {
    id_type res = 0;

    const math::natural shift = max_depth - depth;
      
    for(math::natural i = 0; i < 3; ++i) {
      id_type chunk = (id << i * max_depth) >> (width - max_depth);
      
      chunk = (chunk >> shift) << shift;
      
      res |= chunk << (width - (i + 1) * max_depth);
    }
    
    return res;
  }


  octree::offset_type octree::offset(id_type src, id_type dst, math::natural depth) {
    offset_type res;
    
    res.depth = depth;

    const math::natural shift = max_depth - depth;
    
    for(math::natural i = 0; i < 3; ++i) {
      id_type src_chunk = (src << i * max_depth) >> (width - max_depth);
      src_chunk = (src_chunk >> shift);
      
      id_type dst_chunk = (dst << i * max_depth) >> (width - max_depth);
      dst_chunk = (dst_chunk >> shift);
      
      (&res.x)[i] = dst_chunk - src_chunk;
    }
    
    return res;
  }

  

    
    
}

