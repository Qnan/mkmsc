#include "tnirps_monomial.h"
#include "tnirps_polynomial.h"

void reduce (Polynomial* g, int num, const Polynomial& p) {
   printf("G:\n");
   for (int i = 0; i < num; ++i)
      g[i].print(),printf("\n");
   printf("p:\n");
   p.print(),printf("\n");

   Polynomial r, t;
   r.copy(p);

   do {
      Monomial lm = r.lm();
      int i;
      for (i = 0; i < num; ++i) {
         Monomial m = g[i].lm();
         if (MP.divides(lm, m)) {
            printf("q: "), g[i].print(), printf("\n");
            printf("lm: "), MP.print(lm), printf("\n");
            printf("m: "), MP.print(m), printf("\n");
            Polynomial::mul(t, g[i], MP.div(lm, m));
            printf("t: "), t.print(), printf("\n");
            Polynomial::add(r, t, t.lc(), -r.lc());
            printf("r: "), r.print(), printf("\n");
            printf("\n");
            break;
         }
      }
      if (i == num)
         break;
   } while (true);
}
