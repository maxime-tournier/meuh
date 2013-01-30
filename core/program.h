#ifndef CORE_PROGRAM_H
#define CORE_PROGRAM_H

namespace core {

  struct program {
    virtual ~program() { };

    virtual int operator()(int, char**) = 0;
    
  };

}

#endif
  
