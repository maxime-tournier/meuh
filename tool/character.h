#ifndef TOOL_CHARACTER_H
#define TOOL_CHARACTER_H

#include <phys/object.h>
#include <phys/rigid/skeleton.h>

#include <math/tuple.h>
#include <math/vec.h>
#include <math/so3.h>
#include <math/se3.h>
#include <math/func/any.h>

#include <phys/state.h>
#include <phys/rigid/kinetic.h>
#include <phys/rigid/absolute.h>

#include <math/func/tangent.h>

namespace tool {
  
  
  template<class Config>
  struct character : phys::object<Config> {
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW;
    
    const phys::rigid::skeleton skeleton;
  
    character(const phys::rigid::skeleton& skeleton,
	      const math::lie::group<Config>& lie) 
      : phys::object<Config>(lie),
	skeleton(skeleton), 
        init(lie.id())
    { }
    
    // absolute skeleton config
    typedef math::vector< math::SE3 > absolute_type;

    // configuration space
    typedef Config config_type;
    
    // forward relative kinematics
    math::func::any<config_type, absolute_type> relative;
    
    math::func::any<config_type, absolute_type> absolute() const {
      assert( relative );
      return phys::rigid::absolute(skeleton) << relative;
    }
    
    config_type init;

    void reset() { 
      phys::object<config_type>::reset();
      this->g() = init;
    }

    typedef phys::state<config_type> state_type;
    typedef typename state_type::lagrangian_type lagrangian_type;
    
    lagrangian_type zero() const { return math::func::cst<typename state_type::type>(0.0); }
    
    lagrangian_type kinetic() const {
      using namespace math::func;
      return phys::rigid::kinetic(skeleton.topology) << tangent(absolute());
    }
    
  };

  

  

  // namespace character_old {

  //   struct damping {
  //     enum { kinematic = 0 , energetic = 1, joint = 2};

  //     static math::natural kind;
  //   };


  //   struct ragdoll : phys::object< std::tuple< math::SE3, math::vector<math::SO3> > >{
  //     typedef conf_type G;
      
  //     typedef phys::object< G > base;
      
  //     const phys::rigid::skeleton& skeleton;
      
  //     ragdoll(const phys::rigid::skeleton& skel);	

  //     // resets pos, zero vel/mom
  //     void reset(const G& at);

  //     // reduced config to absolute rigid config
  //     math::func::any<G, phys::rigid::config> joint() const;
      
  //     math::real damping, stiffness;

  //     math::real potential(const G& at) const;
  //   };



  //   struct reduced : phys::object< std::tuple< math::SO3, math::vec3, math::vec > > { 
      
  //     typedef phys::object< conf_type > base;
      
  //     const phys::rigid::skeleton& skeleton;
      
  //     // poses 
  //     typedef math::vector< math::SO3 > P;
  //     tool::pga pga;
      
  //     // absolute rigids
  //     typedef phys::rigid::config H;
      
  //     control::actuator actuator;
  //     control::actuator::vector act;
      
  //     typedef conf_type G;
      
  //     reduced(const phys::rigid::skeleton& skel, 
  // 	      const math::lie::pga<P>& pga,
  // 	      math::natural n);
      
      
        
  //     // reduced config to joint config
  //     math::func::any<G, H> joint() const;
      
  //     // resets pos, zero vel/mom
  //     void reset(const G& at);

  //     math::real damping, stiffness;

  //     math::real potential(const G& at) const;


  //     // hack
  //     math::vec3 AM;

  //     void save_rest();
  //     math::vec rest;

  //     G init;
      
  //     std::unique_ptr<reduced> copy(math::natural n) const;


  //     std::ofstream log;
  //   };
    


  //   // TODO optimize those two
  //   template<class Character, class F>
  //   math::func::any<typename Character::conf_type,
  // 		    typename F::range> absolute(const Character& c, const F& f) 
  //   {
  //     using math::func::operator<<;
  //     return f << phys::rigid::absolute{c.skeleton} << c.joint();
  //   };			
      

  //   template<class Character>
  //   math::func::any<typename Character::conf_type,
  // 		    phys::rigid::config> absolute(const Character& c) 
  //   {
  //     using math::func::operator<<;
  //     return phys::rigid::absolute{c.skeleton} << c.joint();
  //   };
  
  //   template<class Character>
  //   math::mat J(const Character& c, const typename Character::conf_type& at) {
  //     return math::func::J( absolute(c), c.lie, c.skeleton.lie() )(at);
  //   }

 
  //   template<class Character>
  //   const typename Character::momentum_type momentum(Character& c, const typename Character::conf_type& at,
  // 						     const typename Character::velocity_type& v) {
  //     auto dconf = absolute(c).diff( math::body(at, v) );
  //     auto Jv = dconf.body();
      
  //     c.skeleton.each([&](math::natural i) {
  // 	  math::angular(Jv(i)) = c.skeleton.body(i).inertia.asDiagonal() * math::angular(Jv(i));
  // 	  math::linear(Jv(i)) = c.skeleton.body(i).mass * math::linear(Jv(i));
  // 	});

  //     // rigid config lie
  //     auto zob = math::lie::of(dconf.at());
  //     typename Character::momentum_type res = math::pull( absolute(c), at, zob.coalg().elem( zob.alg().coords(Jv))).body();

  //     return res;
  //   }
    
    
  //   // mass tensor + implicit damping TODO move damping outside as its
  //   // not conservative
  //   template<class Character>
  //   math::mat mass(Character& c, const typename Character::conf_type& at, math::real dt) {
  //     using namespace math;

  //     // both rotational and translational
  //     math::real air_damping = 0;

  //     const mat J = character::J(c, at);
  //     const natural inner = c.dim - 6;
    

  //     // core::log("jacobian eigenvalues: ", math::svd(J).eigen().transpose() );

  //     // actually we compute (M + alpha I) J to get air_damping
  //     mat MJ; MJ.resize(J.rows(), J.cols());
      
  //     assert(J.rows() == 6 * c.skeleton.size() );

  //     c.skeleton.each([&](math::natural i) {
  // 	  MJ.block(6*i, 0, 3, J.cols()) = 
  // 	    (vec3::Constant(dt * air_damping)
  // 	     + c.skeleton.body(i).inertia).asDiagonal() * J.block(6*i, 0, 3, J.cols());

  // 	  MJ.block(6*i + 3, 0, 3, J.cols()) = (dt * air_damping + c.skeleton.body(i).mass) * J.block(6*i + 3, 0, 3, J.cols());
  // 	});
      
  //     mat res = J.transpose() * MJ;
      
      

  //     vec damping = vec::Zero(c.dim);
      
  //     // std::vector<math::natural> idx;
	
  //     // idx.push_back( c.skeleton.find("head"));
  //     // idx.push_back( c.skeleton.find("lhand") );
  //     // idx.push_back( c.skeleton.find("rhand") );
	
  //     // idx.push_back( c.skeleton.find("ltoes") );
  //     // idx.push_back( c.skeleton.find("rtoes") );
	
  //     // mat subJ; subJ.resize(3 * idx.size(), inner);

  //     // math::natural off = 0;
  //     // core::each(idx, [&](math::natural i) {
  //     //     subJ.block(3 * off++, 0, 3, inner)
  //     //       = J.block(6 * i + 3, 6, 3, inner);
  //     //   });
	
  //     // vec kinematic = subJ.colwise().squaredNorm().transpose() / (2 * subJ.rows() );
  //     // c.log << kinematic.minCoeff() << ' ' << kinematic.maxCoeff() << ' ';
      
  //     // vec energetic = res.diagonal().tail(inner) / c.skeleton.mass;
  //     // c.log << energetic.minCoeff() << ' ' << energetic.maxCoeff()<< '\n';

  //     // vec eigen_inv  = c.pga.data.eigen().head( inner ).cwiseInverse();
  //     switch( damping::kind ) {
  //     case damping::joint:  {
	 
  // 	mat Jjoint = math::func::J( c.joint(), c.lie, c.skeleton.lie() )(at);

  // 	res.bottomRightCorner(inner, inner) += (dt * c.damping) * 
  // 	  Jjoint.rightCols(inner).transpose() * Jjoint.rightCols(inner);

  //     } break;
	  
  //     case damping::energetic: 
  // 	res.diagonal().tail(inner) =  (1 + dt * c.damping) * res.diagonal().tail(inner);
  // 	break;

  //     default:		// uniform
  // 	res.diagonal().tail(inner) += vec::Constant(inner, c.damping * dt);
  //     }
	
  //     // uniform damping
  //     res.diagonal().tail(inner) += dt* res.diagonal().tail(inner);
	
      


  //     // switch( damping::kind ) {
  //     // 	 case damping::kinematic: 
  //     // case damping::energetic: 
  //     // 	res.diagonal().tail(inner) *= (c.damping / c.skeleton.mass) * dt;
  //     //  break;
  //     // }
    

  
  //     // // kinetic-levmar
  //     // if( character::damping::kind == character::damping::energetic ) {
  //     // 	res.diagonal().tail(inner) *= (1 + c.damping/c.skeleton.mass * dt); // (1 + c.damping * dt);
  //     // } 
      
  //     // // tikhonov
  //     // res.diagonal().tail(inner).array() += c.damping * dt;
     
  //     return res;
  //   }

    
    
  //   template<class Character>
  //   math::real L(const Character& c, 
  // 		 const typename Character::conf_type& at,
  // 		 const typename Character::velocity_type& body) {
  //     using namespace math;
	
  //     const auto abs = absolute(c);
  //     const auto v = abs.diff(math::body(at, body)).body();
  //     Lie<phys::rigid::config>::algebra Mv; Mv.resize(v.rows());
      
  //     c.skeleton.each([&](natural i) {
  // 	  math::angular( Mv(i) ) = c.skeleton.body(i).inertia.asDiagonal() * math::angular(v(i));
  //     	  math::linear( Mv(i) ) = c.skeleton.body(i).mass * math::linear(v(i));
  //     	});
      
  //     const real kinetic = 0.5 * euclid::dot(v, Mv);
  //     // const real tikhonov = 0.5 * c.damping * (euclid::norm2(std::get<1>(body)));

  //     // real levmar = 0;
      

  //     // Lie<phys::rigid::skeleton::config>::algebra dabs, Mdabs;
      
  //     // dabs.resize(v.rows());
  //     // Mdabs.resize(v.rows());
      
  //     // typename Character::velocity_type bodyi = c.lie.alg().zero();
      
  //     // for(natural i = 6; i < c.dof.dim; ++i) {
  //     // 	c.lie.alg().coord(bodyi, i) = c.lie.alg().coord(body, i);
	
  //     // 	dabs = std::move( abs( math::body(at, bodyi)).body() );
	
  //     // 	Mdabs.each([&](natural j) {
  //     // 	    math::linear( Mdabs(j) ) = c.skeleton.topology[j].mass * math::linear( dabs(j));
  //     // 	    math::angular( Mdabs(j) ) = c.skeleton.topology[j].inertia.asDiagonal() * math::angular(dabs(j));	
  //     // 	  });
	
  //     // 	// levmar += euclid::dot(dabs, Mdabs);
  //     // 	levmar += euclid::dot(dabs, Mdabs);
	
  //     // 	c.lie.alg().coord(bodyi, i) = 0;
  //     // }
      
  //     // levmar *= 0.5 * c.damping;

  //     const real total = kinetic // + tikhonov 
  // 	// + levmar
  // 	- c.potential(at);

  //     // const vec vv = c.lie.alg().conf(body);
  //     // const real full =  0.5 * vv.dot( mass(c, at) * vv);
  //     // core::log()("compare:", "full:", full, "fast:", fast);
  //     return total;
      
      
  //   }
    

    
  //   template<class Character>
  //   math::vec inertia(const Character& c, 
  // 		      const typename Character::conf_type& at,
  // 		      const typename Character::velocity_type& body,
  // 		      math::real dh) {
  //     using namespace math;
      
  //     vec res = vec::Zero( c.dim );
      
  //     // TODO parallelize this loop ?
  //     const auto lie = c.lie;
      
  //     const natural n = res.rows();
      
  //     core::parallel(0, n, [&](int start, int end) {
	
  // 	  typename Character::velocity_type dat = lie.alg().zero();
  // 	  typename Character::conf_type delta;
	
  // 	  for(int i = start; i < end; ++i) {
  // 	    lie.alg().coord(dat, i) = dh;
	  
  // 	    // central differences
  // 	    delta = lie.exp()( dat );
	  
  // 	    res(i) = 
  // 	      ( L(c, lie.prod(at, delta ), body) - 
  // 		L(c, lie.prod(at, lie.inv(delta) ), body))  / (2 * dh);
	  
  // 	    lie.alg().coord(dat, i) = 0;
	  
  // 	  }
  // 	});


  //     return res;
  //   }


  //   template<class Character>
  //   math::vec inertia(const Character& c, math::real dh = 1e-7) {
  //     return inertia(c, c.conf, c.prediction(), dh);
  //   }


  //   template<class Character>
  //   void integrate(Character& c, 
  // 		   const math::vec& vel, 
  // 		   const math::vec& mom,
  // 		   math::real dt) {
      
  //     c.integrate(c.lie.alg().elem( vel ),
  // 		  c.lie.coalg().elem( mom ),
  // 		  dt);
      
  //   }

  //   template<class Character>
  //   math::vec weight(const Character& c) {
      
  //     auto com = absolute(c, phys::rigid::com(c.skeleton) );
  //     const math::force p = -math::force::UnitY() * phys::constant::g * c.skeleton.mass;

  //     return c.lie.coalg().coords( math::pull(com, c.conf, p ).body() );
  //   }


  //   template<class Character>
  //   math::vec force(const Character& c,
  // 		    math::natural dof, 
  // 		    const math::vec3& local,
  // 		    const math::force& f){
  //     using namespace math::func;

  //     const auto map = absolute(c, math::func::absolute<>(local) << coord<math::SE3>(dof));
      
  //     return c.lie.coalg().coords( math::pull(map, c.conf, f).body() );
  //   }
    
  //   template<class Character>
  //   math::mat angular(const Character& c, const typename Character::conf_type& at) {
  //     return phys::rigid::momentum::angular(c.skeleton, absolute(c)(at)) * J(c, c.conf);
  //   }

  // }

}

#endif
