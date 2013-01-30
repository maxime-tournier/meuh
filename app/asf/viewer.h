#ifndef VIEWER_H
#define VIEWER_H

#include <tool/viewer.h>
#include <tool/asf.h>
#include <memory>

struct viewer : tool::viewer {
  
  std::unique_ptr<tool::asf> file;
  
  void draw();

};



#endif
