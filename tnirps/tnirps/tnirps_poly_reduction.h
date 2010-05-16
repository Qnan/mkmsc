#ifndef __TNIRPS_POLY_REDUCTION_H__
#define __TNIRPS_POLY_REDUCTION_H__

#include "tnirps_scheme.h"
#include "tnirps_monopool.h"
#include "tnirps_reduction.h"

//#define DBG(op) op
#define DBG(op)

class Reductor {
public:
   Reductor (const ObjArray<Polynomial>& b) : basis(b), sr(b) {
   }

   ~Reductor () {
      for (int i = 0; i < values.size(); ++i)
         values[i].clear();
   }

   void reduce (Polynomial& res, const Scheme& scheme) {
      SCHEME_CALLBACKS_SET(scheme);
      values.resize(scheme.totalCount);
      scheme.proceed(this);
      res.copy(result);            
   }

   SCHEME_CALLBACKS_DEFINE(Reductor);

private:
   void crop (Polynomial& r, const Polynomial& p) {
      Polynomial t;
      t.copy(p);

      while (sr.reduceStep(r, t))
         t.copy(r);
   }

   void promote (Polynomial& r, Polynomial p, Monomial d) {
      Polynomial t, s; // temporary polynomials are cheap, no need to reuse
      for (int i = 0; i < p.size(); ++i) {
         t.clear();
         t.addTerm(MP.mul(p.m(i), d), p.at(i).f);
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
      const Polynomial& pa = values[a], &pb = values[b];
      Polynomial t, s;
      Polynomial& r = values[id];
      r.clear();
      for (int i = pb.begin(); i < pb.end(); i = pb.next(i)) {
         Polynomial::mul(t, pa, pb.m(i), pb.at(i).f);
         crop(s, t);
         Polynomial::add(r, s);
      }
   }
   void mulnum (int id, int a, int num) {
      Polynomial& p = values[id];
      p.copy(values[a]);
      p.mulnum(num);
   }
   void yield (int id) {
      result.copy(values[id]);
   }


   const ObjArray<Polynomial>& basis;
   ObjArray<Polynomial> values;
   Polynomial result;
   SimpleReductor sr;
};

#endif /* __TNIRPS_POLY_REDUCTION_H__ */