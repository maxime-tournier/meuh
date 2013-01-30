#include "viewer.h"

#include <gl/skeleton.h>

void viewer::draw() {

  
  if( !file ) return;

  phys::rigid::config pose = file->skeleton.lie().id();
  
  gl::skel::draw(file->skeleton,
		 pose);
  
  

}
