#ifndef TOOL_TRACK_H
#define TOOL_TRACK_H

#include <math/types.h>
#include <math/vec.h>

#include <math/func/jacobian.h>
#include <math/func/any.h>
#include <math/func/compose.h>

#include <phys/rigid/skeleton.h>
#include <phys/system.h>

#include <control/pd.h>
#include <control/system.h>

#include <gui/frame.h>
#include <core/unique.h>
#include <core/log.h>

#include <mutex>
#include <core/macro/here.h>

namespace tool {

  namespace track {

    
    // features only describe geometry, targets describe desired
    // feature values (recorded or user-manipulated)

    struct feature_base;
    struct target_base;

    // TODO BLOAT
    struct feature_base {

      virtual ~feature_base() { }

      virtual void track( std::unique_ptr<target_base>&& target ) = 0;
    };

    struct target_base { 
      virtual ~target_base() { }
      
      virtual void update( math::real ) {  };
    };
    
    
    template<class Range>
    struct target : target_base {

      virtual math::T<Range> desired( math::real t ) const = 0;
    };
    


    template<class Range>
    struct feature : feature_base {
      
      typedef phys::rigid::config conf_type;

      math::func::any<conf_type, Range> map;
      const math::Lie<Range> lie;

      feature(const math::Lie<Range>& lie = math::Lie<Range>() ) : lie(lie) { }

      std::unique_ptr< track::target<Range> > target;
      
      // TODO: needed ?
      void track( std::unique_ptr<target_base>&& t) {
	target.reset( dynamic_cast< track::target<Range>* >(t.get()));
	assert(target);
      }
      
    };


    
    template<class Config>
    struct context {
      typedef Config conf_type;
      typedef phys::rigid::config pose_type;
    
      math::func::any<conf_type, pose_type> pose;

      const phys::dof* const dof;
      const math::Lie<conf_type> config;
      
      context( const phys::dof* dof,
	       const math::Lie<conf_type>& config) 
	: dof(dof),
	  config(config) { }

    };

    
    struct settings {
      math::real weight;
      bool soft;

      settings () : weight(1.0), soft(false) { }
      
    };
    

    template<class Config>
    struct chunk_base : settings {

      virtual ~chunk_base() { }
      
      virtual math::mat matrix(const Config& at) const = 0;
      virtual math::vec values(const math::T<Config>& current, math::real t, math::real dt) const = 0;
      virtual math::natural dim() const = 0;
      

      virtual void update( math::real ) = 0;

      virtual feature_base* feature() const = 0;

    };


    template<class Config> struct engine;
    
    template<class Config, class Range>
    class chunk : public chunk_base<Config> {
      std::unique_ptr< track::feature<Range> > _feature;
    public:
      const track::engine<Config>& engine;
      
      chunk(const track::engine<Config>& engine) : engine(engine) { }

      void update( math::real t ) { feature()->target->update(t); }

      math::mat matrix(const Config& at) const {
	using math::func::operator<<;
	return math::func::J( feature()->map << engine.context->pose, engine.context->config, feature()->lie )(at);
      }
      
      
      math::vec values(const math::T<Config>& current, math::real t, math::real dt) const {
	
	using namespace math;
	using math::func::operator<<;
	const T<Range> at = (feature()->map << engine.context->pose).diff(current);

	if( this->soft ) { 
	  return control::pd<Range>(engine.p(dt), engine.d())( at, feature()->target->desired(t).at() );
	} else {
	  // return lie::log( lie::prod(lie::inv(at.at()), feature()->target->desired(t).at() ) ) /dt;
	  return control::pd<Range>(1/dt, 0)( at, feature()->target->desired(t).at() );
	}

      }
      
      math::natural dim() const { return feature()->lie.alg().dim(); }
      

      track::feature<Range>* feature() const { assert(_feature); return _feature.get(); }
      void feature(std::unique_ptr< track::feature<Range> >&& feat) { _feature = std::move(feat); } 
    };
    

    template<class Config>
    class engine {
      
      typedef std::unique_ptr< chunk_base<Config> > chunk_type;
      std::list< chunk_type > chunks;
	
      std::mutex mutex;
    public:
      std::unique_ptr<track::context<Config> > context;
      math::real base_p, base_d;
      
      math::real p(math::real dt) const { return base_p; }
      math::real d() const {return base_d; }
      
      template<class Range>
      chunk<Config, Range>* add( track::feature<Range>* feature ) {
	std::lock_guard<std::mutex> lock(mutex);
	
	auto c = new chunk<Config, Range>(*this);
	c->feature( core::unique(feature));
      
	chunks.push_back( core::unique(c) );
	
	return c;
      }

      void remove( feature_base* f ) {
	std::lock_guard<std::mutex> lock(mutex);
	
	auto at = std::find_if(chunks.begin(), chunks.end(),
			       [&](const chunk_type& c) {
				 return c->feature() == f;
			       });
	if( at == chunks.end() ) throw std::logic_error(macro_here + "feature not found");
	chunks.erase( at );

      }
      
      void setup(phys::system& system, control::system& ctrl, const math::T<Config>& current,
		 math::real t, math::real dt, bool soft, bool correct = false) {
	std::lock_guard<std::mutex> lock(mutex);

	assert( context );

	core::each(chunks, [&](const chunk_type& c) {
	    if( !c->soft ) {
	      
	      auto id = new phys::constraint::bilateral( c->dim() );
	      
	      system.constraint.bilateral.storage.push_back( core::unique(id) );
	      
	      system.constraint.bilateral.matrix[ id ] [ this->context->dof ] = c->matrix( current.at() );
	      
	      if(correct) {
		system.constraint.bilateral.corrections[ id ] = c->values( current, t, 1.0 );
	      } else {
		system.constraint.bilateral.values[ id ] = c->values( current, t, dt );
	      }
	      // core::log()("constraint values:", system.constraint.bilateral.values[ id ].norm() );
	    } else if( soft )  {
	      
	      auto id = new control::feature( c->dim() );
	      
	      ctrl.feature.storage.push_back( core::unique(id) );
	      
	      ctrl.feature.matrix[ id ] [ this->context->dof ] = c->weight * c->matrix( current.at() );
	      ctrl.feature.values[ id ] = c->weight * c->values( current, t, dt );
	    }
	  });


      }

      void update(math::real t) { 
	core::each(chunks, [&](const chunk_type& c) {
	    c->update(t);
	  });
      }

    };


    

    template<class Range>
    struct handle : target<Range>{
      
      handle(gui::frame* frame) : frame(frame) { assert(frame); }

      gui::frame* frame;
      virtual math::T<Range> desired( math::real t ) const;

    };
    
    template<>
    math::T<math::vec3> handle<math::vec3>::desired( math::real t ) const;

    
    template<class Range>
    struct recorded : target<Range> {
      
      math::func::any<math::real, Range> curve;
      virtual math::T<Range> desired( math::real t ) const { 
	assert(curve); 
	return curve.diff( math::body(t, 1.0) ); 
      }
      
    };
    
    // either pure IK or recorded tracking
    template<class Range>
    struct flex : target<Range> {

      std::unique_ptr< gui::frame > frame;

      bool track;
      
      track::recorded<Range> recorded;

      ~flex() { 
	core::log()("removing target");
	if(frame) frame->release();
	frame.release();
	
      }

      virtual math::T<Range> desired( math::real t ) const {
	if( track ) {
	  return recorded.desired(t);
	} else {
	  assert( frame );
	  return handle<Range>(frame.get()).desired(t);
	}
      }
      
      virtual void update(math::real t) {
	if( track ) {
	  frame->transform( math::SE3( recorded.desired(t).at(), frame->transform().rotation()) );
	}
      }
    };



  }




 

}


#endif
