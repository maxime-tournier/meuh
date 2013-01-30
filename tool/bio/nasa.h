#ifndef BIO_NASA_H
#define BIO_NASA_H

#include <tool/bio/info.h>
#include <map>

namespace tool {
  namespace bio {

    class nasa : public info {
    public:
      typedef std::map< std::string, math::real > real_map;
      typedef std::map< std::string, math::vec3 > vec3_map;
    private:

      struct part {
	const math::real mass;
	const real_map weights;

	part(const math::real mass, const real_map& weights);

	bool contains(const std::string& name) const;

      };
	
      const part head, neck, thorax, abdomen, pelvis, upperarm, forearm, hand,  hip, thigh, calf, foot;

      const vec3_map _dim;
      const real_map _mass;

      real_map make_mass() const;
      vec3_map make_dim(const real_map& lenghts) const;
    
    public:
      

      nasa(const real_map& lengths);
    
      virtual math::vec3 dim(const std::string& name) const;
      virtual math::real mass(const std::string& name) const;
      virtual math::vec3 inertia(const std::string& name) const;
    
    };
  }
}



#endif
