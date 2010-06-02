#include "tnirps_common.h"
#include "tnirps_monomial.h"
#include "tnirps_polynomial.h"
#include "obj_array.h"
#include "tnirps_reduction.h"

bool SimpleReductor::reduceStep (Polynomial& r, const Polynomial& p) {
   Polynomial t;
   r.copy(p);
   Monomial lm = r.lm();
   int i, i0 = -1;
   for (i = 0; i < g.size(); ++i) {
      Monomial m = g[i].lm();
      if (MP.divides(lm, m)) {
         t.mul(g[i], MP.div(lm, m));
         Cf f;
         Ring::neg(f, r.lc());
         r.add(t, &t.lc(), &f);
         return true;
      }
   }
   return false;
}