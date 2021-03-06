#ifndef __TNIRPS_SCHEME_GORNER_H__
#define __TNIRPS_SCHEME_GORNER_H__

#include "array.h"
#include "list.h"
#include "red_black.h"
#include "tnirps_scheme.h"
#include "tnirps_monopool.h"
#include "tnirps_polynomial.h"
class Exception;

class SchemeGorner {
public:
   SchemeGorner (Scheme& scheme, const Polynomial& poly) : _poly(poly), _scheme(scheme) {}

   void build () {
      monomials.clear();
      monomialCounter = 0;
      intermediateCounter = 0;
      _scheme.resultId = build(_poly);
      postProcIds(_scheme.resultId);
      Array<Scheme::Op>& ops = _scheme.ops;
      for (int i = 0; i < ops.size(); ++i) {
         Scheme::Op& op = ops[i];
         postProcIds(op.r);
         postProcIds(op.a);
         if (op.type != Scheme::OP_MULNUM)
            postProcIds(op.b);
      }
      Array<Monomial>& initial = _scheme.monomials;
      initial.clear();
      initial.resize(monomialCounter);
      for (int i = monomials.begin(); i < monomials.end(); i = monomials.next(i)) {
         initial[monomials.value(i)] = monomials.key(i);
      }
      monomials.clear();
      _scheme.totalCount = monomialCounter + intermediateCounter;
   }
   
private:
   void postProcIds (int& id) {
      if (id < 0)
         id = -(id + 1);
      else
         id += monomialCounter;
   }

   int addMonomial (const Monomial& m1) {
      Monomial m = m1;
      int r;
      if (monomials.find(m)) {
         r = monomials.at(m);
      } else {
         r = monomialCounter++;
         monomials.insert(m, r);
      }
      return -(r+1);
   }

   int build (const Polynomial& p) {
      int i = p.begin();
      Monomial m = p.m(i), m2;
      //NumPtr f(p.at(i).f.get());
      for (i = p.next(i); i < p.end(); i = p.next(i)) {
         const Polynomial::Term& t = p.at(i);
         Monomial m2 = MP.gcd(m, t.m.get());
         // TODO: also find gcd of coefficients
         if (MP.isUnit(m2))
            break;
         m = m2;
      }
      Polynomial p1, p2;
      for (i = p.begin(); i < p.end() && MP.divides(p.m(i), m); i = p.next(i)) {
         m2 = MP.div(p.m(i), m);
         p1.addTerm(m2, p.at(i).f);
      }
      for (; i < p.end(); i = p.next(i)) {
         p2.addTerm(p.m(i), p.at(i).f);
      }
      int id0 = addMonomial(m), id1 = -1, id2 = -1;
      bool hasFactor = !MP.isUnit(p1.lm());
      if (hasFactor) {
         id1 = build(p1);
      } else if (Ring::cmp(p1.lc(), 1) != 0) {
      }
      bool hasSummand = p2.size() > 0;
      if (hasSummand) {
//         printf(" + ");
         id2 = build(p2);
      }
      Array<Scheme::Op>& ops = _scheme.ops;
      ObjPool<Cf>& coeffs = _scheme.coeffs;
      if (hasFactor) {
         int r = intermediateCounter++;
         ops.push().init(Scheme::OP_MUL, r, id0, id1);
         id0 = r;
      } else if (Ring::cmp(p1.lc(), 1) != 0) {
         int r = intermediateCounter++;
         int f = coeffs.add();
         Ring::copy(coeffs.at(f), p1.lc());
         ops.push().init(Scheme::OP_MULNUM, r, id0, f);
         id0 = r;
      }
      if (hasSummand) {
         int r = intermediateCounter++;
         ops.push().init(Scheme::OP_ADD, r, id0, id2);
         id0 = r;
      }
      return id0;
   }

private:
   RedBlackMap<Monomial, int> monomials;
   int monomialCounter;
   int intermediateCounter;

   const Polynomial& _poly;
   Scheme& _scheme;

   SchemeGorner (const SchemeGorner&); // no implicit copy
};

#endif //__TNIRPS_SCHEME_GORNER_H__
