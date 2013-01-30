#ifndef PHYS_RIGID_ABSOLUTE_H
#define PHYS_RIGID_ABSOLUTE_H

#include <phys/rigid/config.h>
#include <phys/rigid/skeleton.h>

#include <math/func/compose.h>
#include <math/func/lie/translation.h>
#include <math/func/lie.h>
#include <math/func/coord.h>


namespace phys {
  namespace rigid {

    struct absolute {
      
      typedef config domain;
      typedef config range;
      
      mutable range res;

      const rigid::skeleton& skeleton;
      absolute( const rigid::skeleton& );
      
      absolute(const absolute& ) = default;
      absolute(absolute&& ) = default;

      const range& operator()(const domain& g) const;

      typedef math::func::compose< math::func::lie::right<math::SE3>,
				   math::func::coordinate< domain > > chunk_type;
    
      typedef math::func::compose< math::func::lie::product<math::SE3>,
				   math::func::tuple_join< chunk_type, chunk_type > > element_type;
      


      struct push {
	const rigid::skeleton& skeleton;
	domain at;
	
	typedef math::func::vector< math::func::traits<element_type>::push > dimpl_type;
	dimpl_type dimpl;
	
	mutable  math::lie::group<range>::algebra res;

	push(const absolute&, const domain& );
	const math::lie::group<range>::algebra& operator()(const math::lie::group<domain>::algebra& ) const;

      };

      struct pull {
	const rigid::skeleton& skeleton;
	
	math::lie::group<domain>::coalgebra zero;
	domain at;
	
	typedef std::tuple<math::SE3, math::SE3> rigid_pair;

	// rigid_pair -> SE3
	typedef math::func::compose< math::func::lie::right<math::SE3>,
				     math::func::tuple_get<0, rigid_pair> > chunk0_type;
	
	typedef math::func::compose< math::func::lie::right<math::SE3>,
				     math::func::tuple_get<1, rigid_pair> > chunk1_type;

      
	// rigid_pair, rigid_pair -> SE3
	typedef math::func::compose< math::func::lie::product<math::SE3>,
				     math::func::tuple_join< chunk0_type, chunk1_type > > element_type;
      
	
	typedef math::func::vector< math::func::traits<element_type>::pull > dTimpl_type;
	dTimpl_type dTimpl;
	
	mutable math::lie::group<domain>::coalgebra res;

	pull(const absolute&, const domain& );
	const math::lie::group<domain>::coalgebra& operator()(const math::lie::group<range>::coalgebra& ) const;

      };


      // math::T<range> diff(const math::T<domain>& dg) const;
      // math::coT<domain> diffT(const math::pullT<absolute>& fg) const;
      
    };
    
  }
}

// namespace math {
//   namespace func {

//     template<class> struct tangent;

//     template<>
//     struct tangent<phys::rigid::absolute> {
      
//       typedef phys::rigid::absolute of_type;
//       typedef of_type::domain at_type;

      
//       typedef Lie<of_type::domain>::algebra domain;
//       typedef Lie<of_type::range>::algebra range;

//       const of_type of;
//       const at_type at;

//       tangent(const of_type& of, const at_type& at);
//       range operator()(const domain& x) const;

//     };

//   }
// }


#endif
