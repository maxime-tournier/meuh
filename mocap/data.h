#ifndef MOCAP_DATA_H
#define MOCAP_DATA_H

#include <mocap/pose.h>
#include <mocap/fix/time.h>

#include <map>


namespace mocap {
    
  typedef std::pair<time, pose> frame;
  typedef std::map<time, pose> data;
  
  time start(const data& );
  time end(const data& );
  time length(const data& );

  math::vector<pose> raw(const data& );
  
}


#endif
