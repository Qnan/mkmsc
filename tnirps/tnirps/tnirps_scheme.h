#ifndef __TNIRPS_SCHEME_H__
#define __TNIRPS_SCHEME_H__

#include "array.h"
#include "list.h"
#include "tnirps_monopool.h"
#include "tnirps_polynomial.h"
class Exception;

class SchemeGorner {
public:
   SchemeGorner (const Polynomial& poly) : _poly(poly) {}

   void build () {
      build(_poly);
   }

   void build (const Polynomial& p) {
      if (p.size() <= 1) {
         p.print();
         return;
      }
      int i = p.begin();
      Monomial m = MP.clone(p.at(i).m), m2;
      CFTYPE f = p.at(i).f;
      for (i = p.next(i); i < p.end(); i = p.next(i)) {
         const Polynomial::Term& t = p.at(i);
         Monomial m2 = MP.gcd(m, t.m);
         // TODO: also find gcd of coefficients
         if (MP.length(m2) == 0)
            break;
         MP.release(m);
         m = m2;
      }
      Polynomial p1, p2;
      for (i = p.begin(); i < p.end() && MP.divides(p.at(i).m, m); i = p.next(i)) {
         m2 = MP.div(p.at(i).m, m); 
         p1.addTerm(m2, p.at(i).f);
         MP.release(m2);
      }
      for (; i < p.end(); i = p.next(i)) {
         p2.addTerm(p.at(i).m, p.at(i).f);
      }
      MP.print(m);
      if (MP.length(p1.lm()) > 0) {
         printf(" * ");
         if (p1.size() > 1)
            printf("("),build(p1),printf(")");
         else
            build(p1);
      }
      if (p2.size() > 0)
         printf(" + "), build(p2);
   }
   void proceed (void* context) {
      // call step() for each edge on the way
   }

   //void (mulMonomial*) (int id, const Monomial& m, void* context); // src == -1 stands for initial items

   //~Scheme () = 0 {}

private:
   const Polynomial& _poly;

   SchemeGorner (const SchemeGorner&); // no implicit copy
};

#endif //__TNIRPS_SCHEME_H__
