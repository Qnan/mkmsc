#include "tnirps_ring.h"

Ring R;

#if QRING
Array<char> Ring::buf;
Pool<Ring::Cell> Ring::pool;
#else
int Ring::inv[Ring::p];
#endif

