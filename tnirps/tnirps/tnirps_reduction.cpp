#include "tnirps_common.h"
#include "tnirps_monomial.h"
#include "tnirps_polynomial.h"
#include "obj_array.h"
#include "tnirps_reduction.h"

bool SimpleReductor::reduceStep (Polynomial& r, const Polynomial& p) {
   Polynomial t;
   r.copy(p);
   Monomial lm = r.lm();
   int i, i0 = -1, maxDeg = 0;
   for (i = 0; i < g.size(); ++i) {
      Monomial m = g[i].lm();
      if (MP.divides(lm, m)) {
         int deg = MP.totalDeg(m);
         // choose the element with highest leading monomial degree
         if (deg > maxDeg) { 
            maxDeg = deg;
            i0 = i;
         }
      }
   }
   if (i0 < 0)
      return false;
   Polynomial::mul(t, g[i0], MP.div(lm, g[i0].lm()), 1);
   Polynomial::add(r, t, t.lc(), -r.lc());
   return true;
}