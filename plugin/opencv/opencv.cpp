#include "opencv.h"

static const plugin::loader<plugin::opencv> loader( [] { return new opencv; } );
