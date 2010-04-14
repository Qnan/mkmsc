#ifndef __TNIRPS_SCHEME_H__
#define __TNIRPS_SCHEME_H__

#include "tnirps_monopool.h"
#include "tnirps_polynomial.h"
class Exception;

class Scheme {
public:
   Scheme (const Polynomial& poly) : _poly(poly) {}

   virtual void build () = 0;

   virtual void proceed (void* context) = 0;

   virtual ~Scheme () {}

protected:
   const Polynomial& _poly;

private:
   Scheme (const Scheme&); // no implicit copy
};

#endif //__TNIRPS_SCHEME_H__
