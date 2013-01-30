
#include <tool/mocap.h>
#include <tool/to_lua.h>

#include <script/meuh.h>
#include <script/api.h>
#include <script/console.h>


int main(int, char** ) {
  
  script::console console;
  
  script::api api("sofa");
  
  api.fun("skeleton", [&](const std::string& s) {
      
      tool::mocap mocap(s);
      return tool::to_lua( mocap.skeleton(), mocap.absolute()(0) );
      
    });
  
  script::require("sofa");
  
  console();
}
