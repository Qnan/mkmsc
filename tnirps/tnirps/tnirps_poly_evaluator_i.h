#ifndef __TNIRPS_POLY_EVALUATOR_I_H__
#define __TNIRPS_POLY_EVALUATOR_I_H__

#include "tnirps_scheme.h"
#include "tnirps_monopool.h"

class EvaluatorInt {
public:
   EvaluatorInt () {}

   void evaluate (const Scheme& scheme, const Array<int>& varValues) {
      SCHEME_CALLBACKS_SET(scheme);
      this->varValues.copy(varValues);
      scheme.proceed(this);
   }

   SCHEME_CALLBACKS_DEFINE(EvaluatorInt);

private:
   void evaluateMonomial (const Array<Monomial>& mm, int i) {
      const MData& m = MP.get(mm[i]);
      int& v = intermediateValues[i];
      v = 1;
      for (int i = 0; i < m.length(); ++i)
         for (int j = 0; j < m.deg(i); ++j)
            v *= varValues[m.var(i)];
   }

   void init (const Array<Monomial>& mm, int total) {
      intermediateValues.clear();
      intermediateValues.resize(total);

      for (int i = 0; i < mm.size(); ++i)
         evaluateMonomial(mm, i);
   }
   void add (int id, int a, int b) {
      intermediateValues[id] = intermediateValues[a] + intermediateValues[b];
   }
   void mul (int id, int a, int b) {
      intermediateValues[id] = intermediateValues[a] * intermediateValues[b];
   }
   void mulnum (int id, int a, int num) {
      intermediateValues[id] = intermediateValues[a] * num;
   }
   void yield (int id) {
      result = intermediateValues[id];
      printf("%d\n", result);
   }

   Array<int> varValues;
   Array<int> intermediateValues;
   int result;
};


#endif /* __TNIRPS_POLY_EVALUATOR_I_H__ */

