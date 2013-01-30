
#define DLL

%module meuh
%{
#include "vector.h"
#include "matrix.h"
#include "solver.h"
#include "config.h"
#include "cg.h"
#include "graph.h"
%}

%include "vector.h"
%include "matrix.h"
%include "solver.h"
%include "config.h"
%include "cg.h"
%include "graph.h"



