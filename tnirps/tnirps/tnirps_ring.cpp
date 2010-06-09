#include "tnirps_ring.h"

Ring R;

#if QRING
Array<char> Ring::buf;
#else
int Ring::inv[Ring::p];
#endif

