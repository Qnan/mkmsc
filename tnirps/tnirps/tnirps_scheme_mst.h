#ifndef __TNIRPS_SCHEME_MST_H__
#define __TNIRPS_SCHEME_MST_H__

#include "array.h"
#include "list.h"
#include "tnirps_scheme.h"
#include "tnirps_monopool.h"
#include "tnirps_polynomial.h"
class Exception;

class SchemeMst {
public:
   SchemeMst (const Polynomial& poly) : _poly(poly) {}

   void build () {
      build(_poly);
   }

   void build (const Polynomial& p) {
      //...
   }

private:
   const Polynomial& _poly;

   SchemeMst (const SchemeMst&); // no implicit copy
};

#endif //__TNIRPS_SCHEME_MST_H__
