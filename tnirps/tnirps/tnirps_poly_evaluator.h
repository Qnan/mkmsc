#ifndef __TNIRPS_POLY_EVALUATOR_H_
#define __TNIRPS_POLY_EVALUATOR_H_

#include "tnirps_scheme.h"
#include "tnirps_monopool.h"
#include "tnirps_bigint.h"

//#define DBG(op) op
#define DBG(op)

class Evaluator {
public:
   Evaluator () {}

   ~Evaluator () {
      for (int i = 0; i < intermediateValues.size(); ++i)
         BI::clear(intermediateValues[i]);
      BI::clear(result);
   }

   void evaluate (const Scheme& scheme, const Array<int>& varValues) {
      SCHEME_CALLBACKS_SET(scheme);
      this->varValues.copy(varValues);
      scheme.proceed(this);
   }

   SCHEME_CALLBACKS_DEFINE(Evaluator);

private:
   void evaluateMonomial (const Array<Monomial>& mm, int i) {
      DBG(printf("init %i\n", i));
      const MData& m = MP.get(mm[i]);
      bigint_t& v = intermediateValues[i];
      BI::set(v, 1);
      bigint_t t;
      BI::init(t);
      for (int i = 0; i < m.length(); ++i) {
         BI::set(t, varValues[m.var(i)]);
         BI::pow(t, t, m.deg(i));
         BI::mul(v, v, t);
      }
      BI::clear(t);
   }

   void init (const Array<Monomial>& mm, int total) {
      intermediateValues.clear();
      intermediateValues.resize(total);
      for (int i = 0; i < intermediateValues.size(); ++i)
         BI::init(intermediateValues[i]);
      BI::init(result);

      for (int i = 0; i < mm.size(); ++i)
         evaluateMonomial(mm, i);
   }
   void add (int id, int a, int b) {
      DBG(printf("add %i %i %i\n", id, a, b));
      BI::add(intermediateValues[id], intermediateValues[a], intermediateValues[b]);
   }
   void mul (int id, int a, int b) {
      DBG(printf("mul %i %i %i\n", id, a, b));
      BI::mul(intermediateValues[id], intermediateValues[a], intermediateValues[b]);
   }
   void mulnum (int id, int a, int num) {
      DBG(printf("mulnum %i %i %i\n", id, a, num));
      BI::mul(intermediateValues[id], intermediateValues[a], num);
   }
   void yield (int id) {
      DBG(printf("yield %i\n", id));
      BI::set(result, intermediateValues[id]);
      gmp_printf("%Zd\n", result);
      DBG(printf("yield done\n"));
   }

   Array<int> varValues;
   Array<bigint_t> intermediateValues;
   bigint_t result;
};

#endif /* __TNIRPS_POLY_EVALUATOR__ */

