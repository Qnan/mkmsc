#ifndef __TNIRPS_SCHEME_TREE_H__
#define __TNIRPS_SCHEME_TREE_H__

#include "array.h"
#include "list.h"
#include "red_black.h"
#include "tnirps_scheme.h"
#include "tnirps_monopool.h"
#include "tnirps_polynomial.h"
class Exception;

class SchemeHangingTree {
public:
   SchemeHangingTree (Scheme& scheme, const Polynomial& poly) : _poly(poly), _scheme(scheme) {}

   void build () {
      initialMonomials.clear();
      monomialCounter = 0;
      intermediateCounter = 0;
      _scheme.resultId = build1();
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
      for (int i = initialMonomials.begin(); i < initialMonomials.end(); i = initialMonomials.next(i)) {
         initial[initialMonomials.value(i)] = initialMonomials.key(i);
      }
      initialMonomials.clear();
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
      Monomial m = MP.clone(m1);
      int r;
      if (initialMonomials.find(m)) {
         r = initialMonomials.at(m);
      } else {
         r = monomialCounter++;
         initialMonomials.insert(m, r);
      }
      return -(r+1);
   }

   int build1 () {
      Polynomial p;
      p.copy(_poly);

      p.sort(MonoPool::DRL, true);
      
      Array<Scheme::Op>& ops = _scheme.ops;
      for (int i = p.begin(); i < p.end(); i = p.next(i)) {
         int k = -1;
         Monomial mi = p.m(i);
         // find the highest degree monomial that divides the current one
         for (int j = p.begin(); j != i; j = p.next(j))
            if (MP.divides(mi, p.m(j)))
               k = j;

         if (k < 0) {
            int id = addMonomial(mi);
            monomials.insert(i, id);
         } else {
            int id = addMonomial(MP.div(mi, p.m(k)));
            int r = intermediateCounter++;
            ops.push().init(Scheme::OP_MUL, r, id, monomials.at(k));
            id = r;
            monomials.insert(i, id);
         }
      }

      for (int i = p.begin(); i < p.end(); i = p.next(i)) {
         //NumPtr f(p.at(i).f.get());
         if (NP.cmp(p.at(i).f.get(), 1) == 0)
            continue;
         int f = _coeffs.add(p.at(i).f.get());

         int r = intermediateCounter++;
         ops.push().init(Scheme::OP_MULNUM, r, monomials.at(i), f);
         monomials.at(i) = r;
      }

      int s = monomials.at(p.begin());
      for (int i = p.next(p.begin()); i < p.end(); i = p.next(i)) {
         int r = intermediateCounter++;
         ops.push().init(Scheme::OP_ADD, r, s, monomials.at(i));
         s = r;
      }
      return s;
   }

private:
   RedBlackMap<int, int> monomials;
   RedBlackMap<Monomial, int> initialMonomials;
   int monomialCounter;
   int intermediateCounter;

   const Polynomial& _poly;
   Scheme& _scheme;
   ObjPool<NumPtr> _coeffs;

   SchemeHangingTree (const SchemeHangingTree&); // no implicit copy
};

#endif //__TNIRPS_SCHEME_TREE_H__
