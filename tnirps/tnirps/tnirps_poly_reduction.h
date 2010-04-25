#ifndef __TNIRPS_POLY_REDUCTION_H__
#define __TNIRPS_POLY_REDUCTION_H__

#include "tnirps_scheme.h"
#include "tnirps_monopool.h"

//#define DBG(op) op
#define DBG(op)

class Reductor {
public:
   Reductor () {}

   ~Reductor () {
      for (int i = 0; i < intermediateValues.size(); ++i)
         intermediateValues[i].clear();
   }

   void reduce (const Scheme& scheme, const Array<Monomial>& b) {
      SCHEME_CALLBACKS_SET(scheme);
      basis.clear();
      basis.resize(b.size());
      for (int i = 0; i < b.size(); ++i)
         basis[i] = MP.clone(b[i]);
   }

   SCHEME_CALLBACKS_DEFINE(Reductor);

private:
   Monomial multiplyMonomial (Monomial src, Monomial d) {

   }

   void evaluateMonomial (const Array<Monomial>& mm, int i) {
//      DBG(printf("init %i\n", i));
//      const MData& m = MP.get(mm[i]);
//      bigint_t& v = intermediateValues[i];
//      BI::set(v, 1);
//      bigint_t t;
//      BI::init(t);
//      for (int i = 0; i < m.length(); ++i) {
//         BI::set(t, varValues[m.var(i)]);
//         BI::pow(t, t, m.deg(i));
//         BI::mul(v, v, t);
//      }
//      BI::clear(t);
   }

   void init (const Array<Monomial>& mm, int total) {
//      intermediateValues.clear();
//      intermediateValues.resize(total);
//      for (int i = 0; i < intermediateValues.size(); ++i)
//         BI::init(intermediateValues[i]);
//      BI::init(result);
//
//      for (int i = 0; i < mm.size(); ++i)
//         evaluateMonomial(mm, i);
   }
   void add (int id, int a, int b) {
//      DBG(printf("add %i %i %i\n", id, a, b));
//      BI::add(intermediateValues[id], intermediateValues[a], intermediateValues[b]);
   }
   void mul (int id, int a, int b) {
//      DBG(printf("mul %i %i %i\n", id, a, b));
//      BI::mul(intermediateValues[id], intermediateValues[a], intermediateValues[b]);
   }
   void mulnum (int id, int a, int num) {
//      DBG(printf("mulnum %i %i %i\n", id, a, num));
//      BI::mul(intermediateValues[id], intermediateValues[a], num);
   }
   void yield (int id) {
//      DBG(printf("yield %i\n", id));
//      BI::set(result, intermediateValues[id]);
//      gmp_printf("result: %Zd\n", result);
//      DBG(printf("yield done\n"));
   }

   Array<Monomial> basis;
   Array<Polynomial> intermediateValues;
   Polynomial result;
};

#endif /* __TNIRPS_POLY_REDUCTION_H__ */