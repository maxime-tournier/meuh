#ifndef NAMESPACES_H
#define NAMESPACES_H

namespace function { }
namespace lie { }
namespace tuple { }
namespace euclid { }
namespace space { }

#define using_function using namespace function; 
#define using_lie using namespace lie;
#define using_tuple using namespace tuple;
#define using_euclid using namespace euclid;


#define using_all using_function; using_lie; using_tuple; using_linear;

#define using_common using_function; using_linear;

#endif

