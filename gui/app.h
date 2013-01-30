#ifndef GUI_APP_H
#define GUI_APP_H

#include <memory>

namespace gui {

  class app {
    struct pimpl_type;
    std::unique_ptr<pimpl_type> pimpl;
  public:
    
    app(int& argc, char** argv);
    ~app();

    // starts console
    app& console();
    
    int exec() const;
    void exit();
  };
  
}


#endif
