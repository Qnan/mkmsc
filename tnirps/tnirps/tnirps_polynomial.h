#ifndef __TNIRPS_POLYNOMIAL_H__
#define __TNIRPS_POLYNOMIAL_H__

#include "obj_pool.h"
#include "obj_list.h"
#include "tnirps_monomial.h"
#include "tnirps_monopool.h"

typedef long long CFTYPE;

class Polynomial {
public:
   struct Term {
      Term () : f(1) {
      }
      MonoPtr m;
      CFTYPE f;
   private:
      Term (const Term& t);
   };

   Polynomial () : _terms(_pool) {}

   ~Polynomial () {
      clear();
   }

   void init (const char* expr, int begin, int end, const char* vnames) {
      clear();
      int val = 0;
      if (end <= 0)
         end = strlen(expr);
      int p0 = 0;
      for (int i = begin; i <= end; ++i) {
         if (i == end || expr[i] == '+' || expr[i] == '-') {
            while (isspace(expr[p0]))
               ++p0;
            int sgn = 1;
            if (expr[p0] == '-') {
               sgn = -1;
               ++p0;
            }
            if (sscanf(expr+p0, "%d", &val) != 1)
               val = 1;
            val *= sgn;
            while (!isalpha(expr[p0]))
               ++p0;
            Monomial m = MP.init(expr, p0, i, vnames);
            addTerm(m,val);
            p0 = i;
         }
      }
   }

   int addTerm (Monomial m, CFTYPE f)
   {                      
      int id = _terms.add();
      Term& t = _terms.at(id);
      t.f = f;
      t.m.set(MP.clone(m));
      return id;
   }

   int insertTerm (int m, CFTYPE f, int before)
   {     
      int id = _terms.insertBefore(before);
      Term& t = _terms.at(id);
      t.f = f;
      t.m.set(MP.clone(m));
      return id;
   }

   void sort ()
   {          
      for (int i = _terms.begin(); i < _terms.end(); i = _terms.next(i))
         for (int j = _terms.begin(); _terms.next(j) < _terms.end();)
            if (MP.cmp(_terms.at(j).m.get(), _terms.at(_terms.next(j)).m.get()) < 0)
               _terms.swap(j);
            else
               j = _terms.next(j);
   }

   int begin () const { return _terms.begin(); }
   int next (int i) const { return _terms.next(i); }
   int end () const { return _terms.end(); }
   const Term& at (int i) const { return _terms.at(i); }
   Term& at (int i) { return _terms.at(i); }
   Monomial m (int i) const { return _terms.at(i).m.get(); }
   int size () const { return _terms.size(); }
   int lm () const { return _terms.at(_terms.begin()).m.get(); }
   CFTYPE lc () const { return _terms.at(_terms.begin()).f; }
   Term& lt () const { return _terms.at(_terms.begin()); }
   
   void print (Output& output) const {
      for (int i = _terms.begin(); i < _terms.end(); i = _terms.next(i)) {
         const Term& t = _terms.at(i);
         if (i != _terms.begin())
            output.printf(" %c ", t.f >= 0 ? '+' : '-');
         else if (t.f < 0)
            output.printf("-");
         bool showVars = MP.length(t.m.get()) > 0;
         CFTYPE f = abs(t.f);
         bool showCf = (f != 1 || !showVars);
         if (showCf)
            output.printf("%lld", f);
         if (showVars) {
            if (showCf)
               output.printf(" ");
            MP.print(output, t.m.get());
         }
      }
   }

   void clear ()
   {
      _terms.clear();
   }

   static void mul (Polynomial& r, const Polynomial& a, Monomial m, const CFTYPE& cf) {
      r.copy(a);
      r.mul(m);
      r.mulnum(cf);
   }

   void mul (Monomial m) {
      if (MP.totalDeg(m) == 0)
         return;
      for (int i = begin(); i < end(); i = next(i)) {
         Term& t = _terms.at(i);
         t.m.set(MP.mul(t.m.get(), m));
      }
   }

   static void sum (Polynomial& r, const Polynomial& a, const Polynomial& b) {
      r.copy(a);
      add(r, b);
   }

   static void add (Polynomial& r, const Polynomial& a) {
      add(r, a, 1, 1);
   }

   static void add (Polynomial& r, const Polynomial& a, const CFTYPE& f) {
      add(r, a, 1, f);
   }

   static void add (Polynomial& r, const Polynomial& a, const CFTYPE& fr, const CFTYPE& fa) {
      int ri = r.begin(), ai = a.begin();
      while (ri < r.end() && ai < a.end()) {
         Term& tr = r.at(ri);
         const Term& ta = a.at(ai);
         int c = MP.cmp(tr.m.get(), ta.m.get());
         if (c > 0) {
            tr.f *= fr;
            ri = r.next(ri);
         } else if (c < 0) {
            r.insertTerm(ta.m.get(), ta.f * fa, ri);
            ai = a.next(ai);
         } else {
            tr.f = tr.f * fr + ta.f * fa;
            if (tr.f == 0) {
               int t = ri;
               ri = r.next(ri);
               ai = a.next(ai);
               r._terms.remove(t);
            } else {
               ri = r.next(ri);
               ai = a.next(ai);
            }
         }
      }
      while (ri < r.end()) {
         Term& tr = r.at(ri);
         tr.f *= fr;
         ri = r.next(ri);
      }
      while (ai < a.end()) {
         const Term& ta = a.at(ai);
         r.addTerm(ta.m.get(), ta.f * fa);
         ai = a.next(ai);
      }
   }

   void mulnum (const CFTYPE& f) {
      for (int i = _terms.begin(); i < _terms.end(); i = _terms.next(i))
         _terms[i].f *= f;
   }

   void copy (const Polynomial& a) {
      clear();
      for (int i = a.begin(); i < a.end(); i = a.next(i)) {
         const Term& t = a.at(i);
         addTerm(t.m.get(), t.f);
      }
   }

private:
   static Pool<ObjList<Term>::Elem> _pool;
   ObjList<Term> _terms; // XXX: shouldn't we be usinig simple List<> here?
   Polynomial (const Polynomial&);
};

#endif //__TNIRPS_POLYNOMIAL_H__
