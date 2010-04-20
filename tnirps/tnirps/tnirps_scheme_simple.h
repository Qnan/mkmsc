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

   int build (const Polynomial& p) {
      Array<Monomial>& initial = _scheme.monomials;
      initial.clear();
      int n = p.size();
      initial.reserve(n);
      for (int i = 0; i < n; ++i) {
         Monomial m = MP.clone(p.at(i).m);
         initial.push(m);
      }

      Array<Scheme::Op>& ops = _scheme.ops;
      // multiply monomials by coefficients
      for (int i = 0; i < n; ++i)
         ops.push().init(Scheme::OP_MULNUM, i + n, i, p.at(i).f);
      int r = 2 * n;
      // multiply sum up terms
      ops.push().init(Scheme::OP_ADD, r, n, n + 1);
      for (int i = 2; i < n; ++i)
         ops.push().init(Scheme::OP_ADD, i + r - 1, r + i - 2, n + i);
      r = n + r - 2;
      return r;
   }

private:
   const Polynomial& _poly;
   Scheme& _scheme;

   SchemeSimple (const SchemeSimple&); // no implicit copy
};

#endif //__TNIRPS_SCHEME_SIMPLE_H__
