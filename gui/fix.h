#ifndef GUI_FIX_H
#define GUI_FIX_H

#ifdef QLOCATION
#undef QLOCATION
#define QLOCATION "\0" __FILE__ ":" QTOSTRING(__LINE__)
#else
// #warning "useless fix !"
#endif


#endif
