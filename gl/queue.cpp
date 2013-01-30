#include "queue.h"

namespace gl {

  std::queue< gl::queue::task > data;

  queue::queue() : data(gl::data){
    
    // TODO aquire mutex ?

  }


  queue::~queue(){
    // TODO release mutex ?

  }

  queue& queue::push( const task& t ) {
    data.push( t );
    return *this;
  }
  
  queue& queue::flush() {
    while(!data.empty() ) {
      data.front()();		// calls the task
      data.pop();
    }
    
    return *this;
  }


}
