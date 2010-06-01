#ifndef __TNIRPS_POLY_EVALUATOR_H__
#define __TNIRPS_POLY_EVALUATOR_H__

#include "tnirps_scheme.h"
#include "tnirps_monopool.h"
#include "tnirps_bigint.h"

//#define DBG(op) op
#define DBG(op)

class Evaluator {
public:
   Evaluator () {}

   ~Evaluator () {
   }

   void evaluate (NumPtr& res, const Scheme& scheme, const Array<int>& varValues) {
      SCHEME_CALLBACKS_SET(scheme);
      this->varValues.copy(varValues);
      scheme.proceed(this);
      res.set(result.get());
   }

   SCHEME_CALLBACKS_DEFINE(Evaluator);

private:
   void evaluateMonomial (const Array<Monomial>& mm, int i) {
      DBG(printf("init %i\n", i));
      const int* degs = MP.getDegs(mm[i]);
      NumPtr& v = values[i];
      v.set(NP.init(1));
      NumPtr t;
      for (int i = 0; i < MP.nvars(); ++i) {
         if (degs[i] > 0) {
            t.set(NP.init(varValues[i]));
            t.set(NP.pow(t.get(), degs[i]));
            v.set(NP.mul(v.get(), t.get()));
         }
      }
   }

   void init (const Array<Monomial>& mm, int total) {
      values.clear();
      values.resize(total);

      for (int i = 0; i < mm.size(); ++i)
         evaluateMonomial(mm, i);
   }
   void add (int id, int a, int b) {
      DBG(printf("add %i %i %i\n", id, a, b));
      values[id].set(NP.sum(values[a].get(), values[b].get()));
   }
   void mul (int id, int a, int b) {
      DBG(printf("mul %i %i %i\n", id, a, b));
      values[id].set(NP.mul(values[a].get(), values[b].get()));
   }
   void mulnum (int id, int a, const NumPtr& num) {
      DBG(printf("mulnum %i %i \n", id, a); NP.print(num.get()); printf("\n"));
      values[id].set(NP.mul(values[a].get(), num.get()));
   }
   void yield (int id) {
      DBG(printf("yield %i\n", id));
      result.set(values[id].get());
      //gmp_printf("result: %Zd\n", result);
      DBG(printf("yield done\n"));
   }

   Array<int> varValues;
   ObjArray<NumPtr> values;
   NumPtr result;
};

#endif /* __TNIRPS_POLY_EVALUATOR_H__ */