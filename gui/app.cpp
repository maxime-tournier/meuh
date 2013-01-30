#include "app.h"

#include <QApplication>
#include <thread>
#include <script/console.h>
#include <signal.h>

#include <vector>

#include <gui/callback.h>

#include <core/log.h>
#include <core/stl.h>

#include <QThread>

namespace gui {
  struct app::pimpl_type  {
    
    pimpl_type(int& argc, char** argv) : qapp(argc, argv) { }
    
    QApplication qapp;
    
    core::callback cleanup;

    std::unique_ptr< script::console > console;

    std::vector< std::thread > threads;
  };
  
  app::app(int& argc, char** argv) 
    : pimpl( new pimpl_type(argc, argv) ) {
    
    // QThread::currentThread()->setPriority( QThread::LowestPriority );
    
  }
  

  app& app::console() {

    std::thread thread([&] {
	pimpl->console.reset( new script::console );
	
	core::push_back(pimpl->cleanup, [&] {
	    if( pimpl->console ) pimpl->console->exit();
	  });
	
	(*pimpl->console)();
	pimpl->console.reset();
	
	pimpl->qapp.exit();
      });
    
    pimpl->threads.push_back( std::move(thread) );


    return *this;
  }

  app::~app()  { }
  

  int app::exec() const {
    int ret = pimpl->qapp.exec();    
    if( pimpl->cleanup ) pimpl->cleanup();

    core::each(pimpl->threads, [&](std::thread& t) {
	t.join();
      });
    
    return ret;
  }

 

}
