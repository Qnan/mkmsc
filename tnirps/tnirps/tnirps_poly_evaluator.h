#ifndef __TNIRPS_POLY_EVALUATOR_H__
#define __TNIRPS_POLY_EVALUATOR_H__

#include "tnirps_scheme.h"
#include "tnirps_monopool.h"
#include "tnirps_bigint.h"

#define DBG(op)

class Evaluator {
public:
   Evaluator () {}

   ~Evaluator () {
   }

   void evaluate (const Scheme& scheme) {
      SCHEME_CALLBACKS_SET(scheme);
      scheme.proceed(this);
   }

   SCHEME_CALLBACKS_DEFINE(Evaluator);

private:
   void evaluateMonomial (const Array<Monomial>& mm, int i) {
      nmul += MP.totalDeg(mm[i]);
   }

   void init (const Array<Monomial>& mm, int total) {
      nmul = 0;
      for (int i = 0; i < mm.size(); ++i)
         evaluateMonomial(mm, i);
   }
   void add (int id, int a, int b) {
   }
   void mul (int id, int a, int b) {
      nmul++;
   }
   void mulnum (int id, int a, const Cf& num) {
   }
   void yield (int id) {
      printf("nmul: %d\n", nmul);
   }

   int nmul;
};

#endif /* __TNIRPS_POLY_EVALUATOR_H__ */