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
      Monomial m;
      CFTYPE f;
   private:
      Term (const Term& t);
   };

   Polynomial () : _terms(_pool) {}

   void init (const char* expr, int begin, int end, const char* vnames) {
      int val = 0;
      if (end <= 0)
         end = strlen(expr);
      int p0 = 0;
      for (int i = begin; i <= end; ++i) {
         if (i == end || expr[i] == '+' || expr[i] == '-') {
            while (isspace(expr[p0]))
               ++p0;
            if (sscanf(expr+p0, "%d", &val) != 1)
               val = 1;
            while (!isalpha(expr[p0]))
               ++p0;
            Monomial m = MP.init(expr, p0, i, vnames);
            addTerm(m,val);
            p0 = ++i;
         }
      }
   }

   int addTerm (Monomial m, CFTYPE f)
   {                      
      int id = _terms.add();
      Term& t = _terms.at(id);
      t.f = f;
      t.m = MP.clone(m);
      return id;
   }

   int insertTerm (int m, CFTYPE f, int before)
   {     
      int id = _terms.insertBefore(before);
      Term& t = _terms.at(id);
      t.f = f;
      t.m = MP.clone(m);
      return id;
   }

   void sort ()
   {          
      for (int i = 0; i < _terms.size(); ++i)
         for (int j = _terms.begin(), k = i + 1; k < _terms.size(); ++k)
            if (MP.cmp(_terms.at(j).m, _terms.at(_terms.next(j)).m) < 0)
               _terms.swap(j);
            else
               j = _terms.next(j);
   }

   int begin () const { return _terms.begin(); }
   int next (int i) const { return _terms.next(i); }
   int end () const { return _terms.end(); }
   const Term& at (int i) const { return _terms.at(i); }
   Term& at (int i) { return _terms.at(i); }
   int size () const { return _terms.size(); }
   int lm () const { return _terms.at(_terms.begin()).m; }
   CFTYPE lc () const { return _terms.at(_terms.begin()).f; }
   Term& lt () const { return _terms.at(_terms.begin()); }
   
   void print () const {
      for (int i = _terms.begin(); i < _terms.end(); i = _terms.next(i)) {
         if (i != _terms.begin())    
            printf(" + ");
         const Term& t = _terms.at(i);
         bool showVars = MP.length(t.m) > 0;
         bool showCf = (t.f != 1 || !showVars);
         if (showCf)
            printf("%lld", t.f);
         if (showVars) {
            if (showCf)
               printf(" ");
            MP.print(t.m);
         }
      }
   }

   void clear ()
   {
      _terms.clear();
   }

   static void mul (Polynomial& r, const Polynomial& a, int m) {
      r.clear();
      int t;
      for (int i = a.begin(); i < a.end(); i = a.next(i)) {
         const Term& ta = a.at(i);
         t = MP.mul(ta.m, m);
         r.addTerm(t, ta.f);
      }
   }

   static void add (Polynomial& r, const Polynomial& a, const CFTYPE& f) {
      add(r, a, 1, f);
   }

   static void add (Polynomial& r, const Polynomial& a, const CFTYPE& fr, const CFTYPE& fa) {
      int ri = r.begin(), ai = a.begin();
      while (ri < r.end() && ai < a.end()) {
         Term& tr = r.at(ri);
         const Term& ta = a.at(ai);
         int c = MP.cmp(tr.m, ta.m);
         if (c > 0) {
            tr.f *= fr;
            ri = r.next(ri);
         } else if (c < 0) {
            r.insertTerm(ta.m, ta.f * fa, ri);
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
         r.addTerm(ta.m, ta.f * fa);
         ai = a.next(ai);
      }
   }

   void copy (const Polynomial& a) {
      clear();
      for (int i = a.begin(); i < a.end(); i = a.next(i)) {
         const Term& t = a.at(i);
         addTerm(t.m, t.f);
      }
   }

private:
   static Pool<ObjList<Term>::Elem> _pool;
   ObjList<Term> _terms;
   Polynomial (const Polynomial&);
};

#endif //__TNIRPS_POLYNOMIAL_H__
