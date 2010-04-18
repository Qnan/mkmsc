#ifndef __TNIRPS_POLY_EVALUATOR_H_
#define __TNIRPS_POLY_EVALUATOR_H_

#include "tnirps_scheme.h"
#include "tnirps_monopool.h"
#include "tnirps_bigint.h"

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
      printf("init\n");
      const MData& m = MP.get(mm[i]);
      bigint_t& v = intermediateValues[i];
      BI::init(v);
      BI::set(v, 1);
      bigint_t t;
      BI::init(t);
      for (int i = 0; i < m.length(); ++i) {
         BI::pow(t, varValues[m.var(i)], m.deg(i));
         BI::mul(v, v, t);
      }
      BI::clear(t);
   }

   void init (const Array<Monomial>& mm, int total) {
      intermediateValues.clear();
      intermediateValues.resize(total);
      for (int i = 0; i < intermediateValues.size(); ++i)
         BI::init(intermediateValues[i]);

      for (int i = 0; i < mm.size(); ++i)
         evaluateMonomial(mm, i);
   }
   void add (int id, int a, int b) {
      printf("add\n");
      BI::add(intermediateValues[id], intermediateValues[a], intermediateValues[b]);
   }
   void mul (int id, int a, int b) {
      printf("mul\n");
      BI::mul(intermediateValues[id], intermediateValues[a], intermediateValues[b]);
   }
   void mulnum (int id, int a, int num) {
      printf("mulnum\n");
      BI::mul(intermediateValues[id], intermediateValues[a], num);
   }
   void yield (int id) {
      printf("yield\n");
      BI::set(result, intermediateValues[id]);
      BI::printf("%Zd\n", result);
      printf("yield done\n");
   }

   Array<int> varValues;
   Array<bigint_t> intermediateValues;
   bigint_t result;
};


#endif /* __TNIRPS_POLY_EVALUATOR__ */

