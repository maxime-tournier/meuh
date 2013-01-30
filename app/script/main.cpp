#include <script/meuh.h>
#include <script/console.h>  

#include <iostream>
#include <boost/program_options.hpp>

#include <core/log.h>
#include <script/call.h>

int main(int argc, char** argv) {

  namespace po = boost::program_options;
  po::options_description desc("Allowed options");
  desc.add_options()
    ("exec,e", po::value<std::string>(), "execute script");
  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm); 

  script::console console;
  
  if( vm.count("exec") ) script::require( vm["exec"].as<std::string>() );
  
  console();
}
