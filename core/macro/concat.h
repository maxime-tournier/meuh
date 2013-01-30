#ifndef CORE_MACRO_CONCAT_H
#define CORE_MACRO_CONCAT_H

#define macro_concat_impl(x, y) x ## y
#define macro_concat(x, y) macro_concat_impl(x, y)


#endif
