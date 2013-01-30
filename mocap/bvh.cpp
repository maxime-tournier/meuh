#include "bvh.h"

#include <mocap/bvh/file.h>

#include <math/pi.h>
#include <mocap/exception.h>

#include <core/stl.h>

#include <core/log.h>

namespace mocap  {

  namespace bvh {

    mocap::skeleton* skeleton(const joint& j  ) {
      mocap::skeleton* res = new mocap::skeleton();
      res->set().name = j.name;
      res->set().offset = math::vec3(j.offset.x, j.offset.y, j.offset.z); 
  
      
      core::each( j.children, [&res](const joint& j) {
	  res->push( skeleton( j ) );
	});
  
      
      return res;
    }

    void flatten(std::vector< joint >& res,  const joint& j ) {
      res.push_back( j );
      core::each( j.children, [&res]( const joint& j ) {
	  flatten(res, j);
	});
    }


    math::vec3 axis(channel::axis_type a) {
      switch(a) {
      case 'X': return math::vec3::UnitX(); break;
      case 'Y': return math::vec3::UnitY(); break;
      case 'Z': return math::vec3::UnitZ(); break;
      default: assert(false && "shouldn't be here");
      }
      throw exception();
    }

    math::SE3 transform(channel c, real value) {
      using namespace math;
      switch(c.kind) {
      case channel::position:
	return SE3::translation( value * axis(c.axis) );
	break;
      case channel::rotation:
	// TODO ATTATION DEGRES !!!11
	return SE3::rotation( SO3( Eigen::AngleAxis<math::real>(value * math::deg, axis(c.axis)) ) );
	break;
      default:
	assert(false && "shouldn't be here");
      }
      throw exception();
    }


    static math::vector< math::vec3> angles(const std::vector<joint>& flat, 
					    const std::vector< unsigned int>& offsets,
					    const std::vector< real >& data) {
      using namespace math;
      vector<vec3> res = vector<vec3>::Constant(flat.size(), vec3::Zero() );

      for(natural j = 0; j < flat.size(); ++j) {
	for(natural i = 0; i < flat[j].channels.size(); ++i) {
	  real alpha = data[ offsets[j] + i ] * math::deg;
	  
	  auto chan = flat[j].channels[i];
	  if(chan.kind == channel::rotation) {
	    switch(chan.axis) {
	    case 'X' : res(j)(0) = alpha;
	      break;
	    case 'Y' : res(j)(1) = alpha;
	      break;
	    case 'Z' : res(j)(2) = alpha;
	      break;
	    }
	  }

	}
      }
      return res;
    }

    static mocap::pose pose(const std::vector<joint>& flat, 
			    const std::vector< unsigned int>& offsets,
			    const std::vector< real >& data ) {
      
      mocap::pose res;
      res.resize( offsets.size() );
  
      for(math::natural j = 0; j < flat.size(); ++j) {
	math::vec3 off (flat[j].offset.x, flat[j].offset.y, flat[j].offset.z );
	res(j) = math::SE3::translation(off);
	
	for(math::natural i = 0; i < flat[j].channels.size(); ++i) {
	  assert( offsets[j] + i < data.size() );
	  res(j) = res(j) * transform( flat[j].channels[i], data[ offsets[j] + i ] );
	}
	
      }

      return res;
    }

    void name_leafs(mocap::skeleton* j) {
      if( j->leaf() ) {
	j->set().name = j->parent()->get().name + " end";
      }
    }

    mocap::clip clip(const bvh::file& file) {
      mocap::clip res;
      
      // sampling
      res.sampling.start = 0;
      res.sampling.dt = 1.0/120.0; // file.motion.frame_time; // HARDCODE !
      res.sampling.frames = file.motion.frames;

      // skeleton
      res.skeleton.reset( skeleton( file.hierarchy.root ) );
      mocap::number( res.skeleton.get() );
      res.skeleton->each( &name_leafs );
      
      // flatten
      std::vector< mocap::bvh::joint > flat;
      flatten( flat, file.hierarchy.root );

      // offsets
      std::vector<unsigned int> offsets;

      unsigned int sum = 0;
      core::each( flat, [&](const mocap::bvh::joint& j ) {
	  offsets.push_back(sum);
	  sum += j.channels.size();
	});
      
      // fill poses
      for(math::natural i = 0; i < file.motion.frames; ++i) {
	res.data[ res.sampling.time(i) ] =
	  pose( flat, offsets, file.motion.data[i] );
      }
      
      // angles

      res.angles.resize(file.motion.frames, flat.size());

      std::vector<math::real> squared_norm(file.motion.data[0].size());
      for(math::natural j = 0; j < squared_norm.size(); ++j){
	squared_norm[j] = 0;
      }
      
      for(math::natural i = 0; i < file.motion.frames; ++i) {
	res.angles.row(i) = angles( flat, offsets, file.motion.data[i] ).transpose();

	for(math::natural j = 0; j < squared_norm.size(); ++j){
	  squared_norm[j] += file.motion.data[i][j] * file.motion.data[i][j];
	}
      }
      
      math::natural nonzero = 0;
      for(math::natural j = 0; j < squared_norm.size(); ++j){
	if( squared_norm[j] > 1e-5 ) ++nonzero;
      }
      
      core::log("bvh dofs:", nonzero, "/", squared_norm.size());
      
      return res;
    }
  }

}
