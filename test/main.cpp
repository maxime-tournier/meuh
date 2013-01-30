
#include <script/api.h>
#include <script/console.h>
#include <script/meuh.h>

int main(int argc, char** argv) {
  
  
  script::api test("test");
  

  int bob = 5;

  test.fun("michel", [&]() ->int& { return bob; } );
  
  script::require("api");
  script::exec("test = api.load('test')");

  script::console console;
  console();
  
}
