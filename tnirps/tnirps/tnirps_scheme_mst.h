#ifndef __TNIRPS_SCHEME_MST_H__
#define __TNIRPS_SCHEME_MST_H__

#include "array.h"
#include "list.h"
#include "tnirps_scheme.h"
#include "tnirps_monopool.h"
#include "tnirps_polynomial.h"
class Exception;

class SchemeMst : public Scheme {
public:
   SchemeMst (const Polynomial& poly) : Scheme(poly) {}

   // override
   virtual void build () {
      build(_poly);
   }

   // override
   virtual void proceed (void* context) {
      // call step() for each edge on the way
   }

   void build (const Polynomial& p) {
      //...
   }


private:
   SchemeMst (const SchemeMst&); // no implicit copy
};

#endif //__TNIRPS_SCHEME_MST_H__
