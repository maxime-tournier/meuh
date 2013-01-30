#include "absolute.h"

#include <math/func/lie.h>
#include <math/func/lie/translation.h>
#include <math/func/tuple.h>
#include <math/func/coord.h>

#include <core/debug.h>
#include <meta/debug.h>

namespace phys {
  namespace rigid {

    absolute::absolute(const rigid::skeleton& skeleton) 
      : skeleton(skeleton) { 
      res.resize( skeleton.size() );
    }

    const absolute::range& absolute::operator()(const domain& g) const {
      assert(g.rows() == skeleton.size());

      res = g;     
      
      skeleton.prefix([&](math::natural i) {
	  
	  if(i == skeleton.root) {
	    res(i) = g(i);
	  } else {
	    
	    auto e = skeleton.in_edge(i);
	    auto p = skeleton.parent(i);
	  
	    const joint& j = skeleton.topology[e];
	    
	    using namespace math::func;
	    typedef coordinate<math::vector< math::SE3 > > coord;
	    
	    auto f = lie::prod( lie::R(j.frame(p)) << coord(p) ,
				lie::R(j.frame(i).inverse()) << coord(i));

	    res(i) = f(res);
	  }
	});
      
      return res;
      
    }

    static absolute::push::dimpl_type make_dimpl(const rigid::skeleton& skeleton,
						 const absolute::domain& at) {
      assert(at.size() == skeleton.size() );

      absolute::push::dimpl_type::data_type data(at.size());
      
      absolute::domain res = at;
      
      using namespace math::func;
      typedef coordinate< absolute::domain > coord;

      skeleton.prefix([&](math::natural i) {
	  
	  if(i == skeleton.root) {
	    // HACK to get identity
	    data.set(i, d(lie::prod( lie::R(math::SE3::identity()) << coord(i) ,
				     lie::R(at(i).inverse()) << coord(i)))(at) );
	  } else {
	    
	    auto e = skeleton.in_edge(i);
	    auto p = skeleton.parent(i);

	    const joint& j = skeleton.topology[e];
	    auto fun = lie::prod( lie::R(j.frame(p)) << coord(p),
				  lie::R(j.frame(i).inverse()) << coord(i));

	    // the pushforward is evaluated on absolute-parent, relative-child
	    data.set(i, d(fun)( res ));
	    
	    res(i) = fun(res);
	  }
	});

      return {data};
    }


    static absolute::pull::dTimpl_type make_dTimpl(const rigid::skeleton& skeleton,
						   const absolute::domain& at) {
      assert(at.size() == skeleton.size() );

      absolute::pull::dTimpl_type::data_type data(at.size());
      
      using namespace math::func;
      typedef coordinate< absolute::domain > coord;

      tuple_get<0, absolute::pull::rigid_pair> get_first;
      tuple_get<1, absolute::pull::rigid_pair> get_second;

      absolute::domain res = at;

      skeleton.prefix([&](math::natural i) {
	  
	  if(i == skeleton.root) {
	    // HACK to get identity
	    absolute::pull::rigid_pair tmp(at(i), at(i));
	    data.set(i, dT(lie::prod( lie::R(math::SE3::identity()) << get_first ,
				      lie::R(at(i).inverse()) << get_second))(tmp));
	  } else {
	    
	    auto e = skeleton.in_edge(i);
	    auto p = skeleton.parent(i);
	    
	    const joint& j = skeleton.topology[e];
	    
	    absolute::pull::rigid_pair tmp(res(p), res(i));

	    auto fun = lie::prod( lie::R(j.frame(p)) << get_first,
				  lie::R(j.frame(i).inverse()) << get_second);

	    // the pullback is evaluated on absolute-parent, relative-child
	    data.set(i, dT(fun) (tmp));
	    
	    res(i) = fun(tmp);
	  }
	});

      return {data};
    }
    

    absolute::push::push(const absolute& of, const domain& at) 
      : skeleton(of.skeleton),
	at( at ),
	dimpl(make_dimpl(of.skeleton, at))
    {
      res.resize(skeleton.size());
    }    

    const math::lie::group<absolute::range>::algebra& absolute::push::operator()(const math::lie::group<domain>::algebra& v) const {
      assert( skeleton.size() == v.rows() );
      
      res = v;
      
      skeleton.prefix([&](math::natural i ) {
	  if( i == skeleton.root ) {
	    res(i) = v(i);
	  } else {
	    res(i) = dimpl.data(i)(res);
	  }
	  
	});
      
      return res;
    }

    
    absolute::pull::pull(const absolute& of, const domain& at) 
      : skeleton(of.skeleton),
	zero(of.skeleton.lie().coalg().zero() ),
	at(at),
	dTimpl(make_dTimpl(of.skeleton, at))
    {
      
      res.resize(skeleton.size());
    }


    const math::lie::group<absolute::domain>::coalgebra& absolute::pull::operator()(const math::lie::group<range>::coalgebra& f) const {
      assert(f.size() == zero.size());
      
      // absolute forces
      res = f;
      
      skeleton.postfix([&](math::natural i) {
	  
	  if(i == skeleton.root) {
	    // f(i) is already in res(i)
	  } else {
	    auto p = skeleton.parent(i);
	    
	    // res(i) is the total absolute force on i
	    auto pulled = dTimpl.data(i) ( res(i) );
	    
	    // adds corresponding force to parent
	    res(p) += std::get<0>( pulled );

	    // res(i) becomes the net relative force
	    res(i) = std::get<1>( pulled );
	  }
	});
      
      return res;
    }



    // math::T<absolute::range> absolute::diff(const math::T<domain>& dg) const {
    //   assert(dg.at().rows() == skeleton.size());

    //   range at;
    //   at.resize( skeleton.size() );
      
    //   math::Lie<range>::algebra body;
    //   body.resize( skeleton.size() );
      
      
    //   skeleton.prefix([&](math::natural i) {

    // 	  if(i == skeleton.root) {
    // 	    at(i) = dg.at()(i);
    // 	    body(i) = dg.body()(i);
    // 	  } else {
    // 	    auto e = skeleton.in_edge(i); 
    // 	    auto p = boost::source(e, skeleton.topology );
	    
    // 	    const joint& j = skeleton.topology[e];
	    
    // 	    using namespace math::func;
    // 	    auto f =  lie::prod( lie::R(j.coords(p)),
    // 				 lie::R(j.coords(i).inverse() ) );
    // 	    auto i_th = coord<math::SE3>(i);
	    
    // 	    auto res_i = f.diff( math::tuple::merge( math::body(at(p), body(p)),
    // 						     i_th.diff(dg)) );
    // 	    at(i) = res_i.at();
    // 	    body(i) = res_i.body();
    // 	  }
    // 	});

    //   return math::body( std::move(at), std::move(body) );
    // }
    

    // math::coT<absolute::domain> absolute::diffT(const math::pullT<absolute>& fg) const {
    //   assert(fg.at().rows() == skeleton.size());

    //   using namespace math;
    
    //   // net absolute forces
    //   auto fg_body = fg.body();
    //   // 
    //   const Lie<domain> lie = skeleton.lie();
    //   Lie<domain>::coalgebra body = lie.coalg().zero();
      
    //   skeleton.postfix([&](math::natural i) {
    // 	  if( i == skeleton.root ) return;
	  
    // 	  auto e = skeleton.in_edge(i);
	  
    // 	  const joint& j = skeleton.joint(e);

    // 	  // map force on center, to force on joint
    // 	  body(i) = math::lie::adT( j.coords(i) ) ( fg_body(i) );
	  
    // 	  // transmit force to centered parent 
    // 	  const natural p = boost::source(e, skeleton.topology );
	  
    // 	  // centered parent joint to centered child joint
    // 	  const SE3 p2c = j.coords(p) * fg.at()(i) *  j.coords(i).inverse();
	  
    // 	  fg_body(p) += math::lie::adT( p2c.inverse() ) ( fg_body(i) );
    // 	});

      
    //   // root
    //   body( skeleton.root ) = fg_body( skeleton.root );
      
    //   return math::cobody( fg.at(), std::move(body) );
     
    // }
  }
}


// namespace math {
//   namespace func {


//     tangent< phys::rigid::absolute >::tangent(const of_type& of, const at_type& at) 
//       : of(of), at(at) {

//     }

//     tangent< phys::rigid::absolute >::range tangent< phys::rigid::absolute >::operator()(const domain& body) const {
//       range res;
//       res.resize( of.skeleton.size() );
      
//       of.skeleton.prefix([&](math::natural i) {
	  
// 	  if(i == skeleton.root) {
// 	    res(i) = body(i);
// 	  } else {
// 	    auto e = skeleton.in_edge(i); 
// 	    auto p = boost::source(e, skeleton.topology );
	    
// 	    const joint& j = skeleton.topology[e];
	    
	    

// 	    using namespace math::func;
// 	    auto f =  lie::prod( lie::R(j.coords(p)),
// 				 lie::R(j.coords(i).inverse() ) );
// 	    auto i_th = coord<math::SE3>(i);
	    
// 	    auto res_i = f ( math::tuple::merge( math::body(at(p), body(p)),
// 						 i_th(dg)) );
// 	    at(i) = res_i.at();
// 	    body(i) = res_i.body();
// 	  }
// 	});


//     }

//   }
// }
