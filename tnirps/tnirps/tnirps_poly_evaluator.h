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
      for (int i = 0; i < values.size(); ++i)
         BI::clear(values[i]);
      BI::clear(result);
   }

   void evaluate (bigint_t& res, const Scheme& scheme, const Array<int>& varValues) {
      SCHEME_CALLBACKS_SET(scheme);
      this->varValues.copy(varValues);
      scheme.proceed(this);
      BigInt::set(res, result);
   }

   SCHEME_CALLBACKS_DEFINE(Evaluator);

private:
   void evaluateMonomial (const Array<Monomial>& mm, int i) {
      DBG(printf("init %i\n", i));
      const MData& m = MP.get(mm[i]);
      bigint_t& v = values[i];
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
      values.clear();
      values.resize(total);
      for (int i = 0; i < values.size(); ++i)
         BI::init(values[i]);
      BI::init(result);

      for (int i = 0; i < mm.size(); ++i)
         evaluateMonomial(mm, i);
   }
   void add (int id, int a, int b) {
      DBG(printf("add %i %i %i\n", id, a, b));
      BI::add(values[id], values[a], values[b]);
   }
   void mul (int id, int a, int b) {
      DBG(printf("mul %i %i %i\n", id, a, b));
      BI::mul(values[id], values[a], values[b]);
   }
   void mulnum (int id, int a, int num) {
      DBG(printf("mulnum %i %i %i\n", id, a, num));
      BI::mul(values[id], values[a], num);
   }
   void yield (int id) {
      DBG(printf("yield %i\n", id));
      BI::set(result, values[id]);
      //gmp_printf("result: %Zd\n", result);
      DBG(printf("yield done\n"));
   }

   Array<int> varValues;
   Array<bigint_t> values;
   bigint_t result;
};

#endif /* __TNIRPS_POLY_EVALUATOR_H__ */