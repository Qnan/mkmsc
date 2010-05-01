#ifndef __TNIRPS_POLY_REDUCTION_H__
#define __TNIRPS_POLY_REDUCTION_H__

#include "tnirps_scheme.h"
#include "tnirps_monopool.h"

//#define DBG(op) op
#define DBG(op)

class Reductor {
public:
   Reductor (const ObjArray<Polynomial>& b) : basis(b) {
   }

   ~Reductor () {
      for (int i = 0; i < values.size(); ++i)
         values[i].clear();
   }

   void reduce (const Scheme& scheme) {
      SCHEME_CALLBACKS_SET(scheme);
      
   }

   SCHEME_CALLBACKS_DEFINE(Reductor);

private:
   void crop (Polynomial& r, Polynomial p) {
      r.clear();
      Polynomial t;
      r.copy(p);

      do {
         Monomial lm = r.lm();
         int i;
         // TODO: think of better reduction strategy
         for (i = 0; i < basis.size(); ++i) {
            Monomial m = basis[i].lm();
            if (MP.divides(lm, m)) {
               Polynomial::mul(t, basis[i], MP.div(lm, m));
               Polynomial::add(r, t, t.lc(), -r.lc());
               break;
            }
         }
         if (i == basis.size())
            break;
      } while (true);

   }

   void promote (Polynomial& r, Polynomial p, Monomial d) {
      Polynomial t, s; // temporary polynomials are cheap, no need to reuse
      for (int i = 0; i < p.size(); ++i) {
         t.clear();
         t.addTerm(MP.mul(p.at(i).m, d), p.at(i).f);
         crop(s, t);
         Polynomial::add(r, s);
      }
   }

   void evaluateMonomial (const Array<Monomial>& mm, int i) {
      const MData& m = MP.get(mm[i]);
      Polynomial t;
      t.addTerm(mm[i], 1);
      crop(values[i], t);
      // TODO: we should initialize these monomials inductively, too
      // TODO: think of alteration between two temporary polynomials on such occasions
   }

   void init (const Array<Monomial>& mm, int total) {
      values.clear();
      values.resize(total);
      for (int i = 0; i < mm.size(); ++i)
         evaluateMonomial(mm, i);
   }
   void add (int id, int a, int b) {
      Polynomial::sum(values[id], values[a], values[b]);
   }
   void mul (int id, int a, int b) {
      throw "Not implemented";
      // TODO: take one polynomial and promote it carefully
   }
   void mulnum (int id, int a, int num) {
      Polynomial& p = values[id];
      p.copy(a);
      p.mulnum(num);
   }
   void yield (int id) {
      result.copy(values[id]);
   }

   const ObjArray<Polynomial>& basis;

   Array<Polynomial> values;
   Polynomial result;
};

#endif /* __TNIRPS_POLY_REDUCTION_H__ */