#include "tnirps_polynomial.h"

Pool<List<Polynomial::Term>::Elem> Polynomial::_pool;
MonoPool::ORDER Polynomial::specOrder;
bool Polynomial::specInvert;
