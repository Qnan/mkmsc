#ifndef __TNIRPS_SCHEME_SIMPLE_H__
#define __TNIRPS_SCHEME_SIMPLE_H__

#include "tnirps_scheme.h"
#include "tnirps_polynomial.h"
class Exception;

class SchemeSimple {
public:
   SchemeSimple (Scheme& scheme, const Polynomial& poly) : _poly(poly), _scheme(scheme) {}

   void build () {
      _scheme.resultId = build(_poly);
      _scheme.totalCount = _scheme.resultId+1;
   }

private:
   int build (const Polynomial& p) {
      Array<Monomial>& initial = _scheme.monomials;
      initial.clear();
      initial.reserve(p.size());
      RedBlackMap<int, int> terms;
      for (int i = p.begin(); i < p.end(); i = p.next(i)) {
         Monomial m = MP.clone(p.m(i));
         terms.insert(i, initial.size());
         initial.push(m);
      }

      Array<Scheme::Op>& ops = _scheme.ops;
      ObjPool<NumPtr>& coeffs = _scheme.coeffs;
      // multiply monomials by coefficients
      int cnt = p.size();
      for (int i = p.begin(); i < p.end(); i = p.next(i)) {
         const Polynomial::Term& t = p.at(i);
         int id = coeffs.add(t.f.get());
         if (NP.cmp(t.f.get(), 1)) {
            int r = cnt++;
            ops.push().init(Scheme::OP_MULNUM, r, terms.at(i), id);
            terms.at(i) = r;
         }
      }
      // sum up
      int s = terms.at(p.begin());
      for (int i = p.next(p.begin()); i < p.end(); i = p.next(i)) {
         int r = cnt++;
         ops.push().init(Scheme::OP_ADD, r, s, terms.at(i));
         s = r;
      }
      return s;
   }
   
   const Polynomial& _poly;
   Scheme& _scheme;
   
   SchemeSimple (const SchemeSimple&); // no implicit copy
};

#endif //__TNIRPS_SCHEME_SIMPLE_H__
