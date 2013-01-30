#ifndef PHYS_RIGID_SKELETON_H
#define PHYS_RIGID_SKELETON_H

#include <phys/rigid/topology.h>
#include <phys/rigid/config.h>

#include <core/stl.h>

namespace phys {
  namespace rigid {
    
    struct skeleton  {
    
      typedef rigid::topology::directed topology_type;
      const topology_type topology;
      
      const topology_type::vertex_descriptor root;
      const math::real mass;
      
      skeleton(const topology_type& );
      
      // convenience 
      template<class F>
      void prefix(const F& f) const { core::each(_prefix, f); }

      template<class F>
      void postfix(const F& f) const { core::each(_postfix, f); }

      template<class F>
      void each(const F& f) const { core::each(boost::vertices(topology), f); }
      
      topology_type::edge_descriptor in_edge(topology_type::vertex_descriptor v) const;
      topology_type::vertex_descriptor parent(topology_type::vertex_descriptor v) const;

      math::natural size() const;

      math::lie::group<config> lie() const;
      
      topology_type::vertex_descriptor find(const std::string& name) const; // O(n)
      
      const rigid::body& body( topology_type::vertex_descriptor ) const;
      const rigid::joint& joint( topology_type::edge_descriptor ) const;

    private:
      // caches
      const std::vector< topology_type::vertex_descriptor > _prefix;
      const std::vector< topology_type::vertex_descriptor > _postfix;
    };
    
    
    
    
  }
}



#endif
