#include "test.h"
#include "test.inl"

#include "instanciator.h"

template class instanciator< arg2<foo, types(int, double), types(double, char) > >;

