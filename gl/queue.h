#ifndef GL_QUEUE_H
#define GL_QUEUE_H

#include <queue>
#include <functional>

namespace gl {

  class queue {
  public:
    typedef std::function< void() > task;
   
    queue();
    ~queue();

    queue& flush();
    queue& push(const task& );

  private:
    std::queue< task >& data;
  };
  




};




#endif
