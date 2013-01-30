#ifndef CORE_DLL_H
#define CORE_DLL_H

#ifdef WIN32

#ifdef BUILD_DLL 

#define DLL __declspec(dllexport)

#else

#define DLL __declspec(dllimport)
#endif

#else 

#define DLL

#endif


#endif
