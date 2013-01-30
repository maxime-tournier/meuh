#ifndef PHYS_ENGINE_H
#define PHYS_ENGINE_H


#include <phys/pool.h>

#include <sparse/vector.h>
#include <phys/object.h>
#include <math/vec.h>

// TODO unordered ?
#include <map>

namespace phys {


  
  namespace op {
    struct integrate {
      
      const dof::velocity& v;
      const dof::momentum& p;
      const math::real dt;
      
      template<class G>
      void operator()(object<G>* obj) const {
	
	auto ov = obj->lie.alg().zero();
	auto op = obj->lie.coalg().zero();
	
	v.find( obj, [&](sparse::const_vec_chunk v) {
	    // TODO size asserts
	    math::euclid::set(ov, v);
	  });

	p.find( obj, [&](sparse::const_vec_chunk p) {
	    // TODO size asserts
	    math::euclid::set(op, p);
	  });

	obj->integrate(ov, op, dt);
      }

    };


    struct correct {
      
      const dof::velocity& c;
      
      template<class G>
      void operator()(object<G>* obj) const {
	
	auto oc = obj->lie.alg().zero();
	
	c.find( obj, [&](sparse::const_vec_chunk c) {
	    // TODO size asserts
	    math::euclid::set(oc, c);
	  });
	
	obj->correct(oc);
      }
      
    };

    struct momentum {
      dof::momentum& p;
      
      const dof::force& f;
      const math::real dt;

      template<class G>
      void operator()(object<G>* obj) const {
	typename object<G>::momentum_type of = obj->lie.coalg().zero();
	f.find(obj, [&](sparse::const_vec_chunk f) {
	    math::euclid::set(of, f);
	  });
	
	math::euclid::get( p(obj), obj->net( of, dt ) );
      }
    };


    struct set {
      const dof::velocity& v;
      const dof::momentum& p;

      template<class G>
      void operator()(object<G>* obj) const {
	v.find( obj, [&](sparse::const_vec_chunk v) {
	    // TODO size asserts
	    math::euclid::set(obj->v(), v);
	  });

	p.find( obj, [&](sparse::const_vec_chunk p) {
	    // TODO size asserts
	    math::euclid::set(obj->p(), p);
	  });
      }
      
    };


    // fix velocities
    struct add {
      const dof::velocity& v;
      const dof::momentum& p;

      add(const dof::velocity& v,
	  const dof::momentum& p);
      
      mutable math::vec storage;

      template<class G>
      void operator()(object<G>* obj) const {
	v.find( obj, [&](sparse::const_vec_chunk v) {
	    // TODO size asserts
	    
	    storage.resize(obj->lie.alg().dim());
	    math::euclid::get(storage, obj->v());
	    storage += v;
	    math::euclid::set(obj->v(), storage);
	  });

	p.find( obj, [&](sparse::const_vec_chunk p) {
	    // TODO size asserts

	    math::euclid::get(storage, obj->p());
	    storage += p;
	    math::euclid::set(obj->p(), storage);

	  });
      }
      
    };

  }

  class engine {
    
    typedef phys::pool<op::integrate,
		       op::correct,
		       op::momentum,
		       op::set,
		       op::add> pool_type;
    
    pool_type pool;
  public:
    
    dof::momentum momentum(const dof::momentum& f, math::real dt) const;
    void correct(const dof::velocity& c) const;
    void integrate(const dof::velocity& v, const dof::momentum& p, math::real dt) const;

    void set(const dof::velocity& v, const dof::momentum& p) const;
    void add(const dof::velocity& v, const dof::momentum& p) const;
    
    template<class G>
    void add(object<G>* obj) {
      pool.add( obj );
    }
 
    void clear();

  };

}


#endif
