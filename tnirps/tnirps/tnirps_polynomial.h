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

   void init (const char* expr, int begin = 0, int end = 0) {
      clear();
      if (end <= 0)
         end = strlen(expr);

      BufferScanner sc(expr + begin, end - begin);
      static Array<char> buf;
      while (!sc.isEOF()) {
         sc.skipSpace();
         char c = sc.lookNext();
         if (c == '+' || c == '-') {
            sc.readChar();
            sc.skipSpace();
         }
         int sgn = c == '-' ? -1 : 1;

         int coeff = 1;
         bool hasCoeff = false, variablesExpected = false;
         // read coefficient, if present
         if (isdigit(sc.lookNext())) {
            hasCoeff = true;
            coeff = sc.readInt();
            sc.skipSpace();
            if (!sc.isEOF() && sc.lookNext() == '*') {
               variablesExpected = true;
               sc.readChar();
               sc.skipSpace();
            }
         } else {
            variablesExpected = true;
         }
         coeff *= sgn;

         // read variables, if any
         sc.readWord(buf, "+-");
         buf.rstrip();
         Monomial m;
         if (strlen(buf.ptr()) > 0) {
            if (!variablesExpected)
               throw Exception("For non-trivial monomials, coefficient should be followed by multiplication sign:\n\t%s", expr + begin);
            m = MP.init(buf.ptr());
         } else {
            if (variablesExpected)
               throw Exception("Variables are expected at this point:\n\t%s", expr + begin);
               
            m = MP.unit();
         }

         addTerm(m, coeff);
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

   bool isSorted ()
   {
      for (int i = begin(); i < end(); i = next(i))
         if (MP.cmp(m(i), m(next(i))) < 0)
            return false;
      return true;
   }

   static int cmpDefault (const Term& a, const Term& b) {
      return MP.cmp(a.m.get(), b.m.get());
   }

   static MonoPool::ORDER specOrder;
   static bool specInvert;
   static int cmpSpec (const Term& a, const Term& b) {
      return (specInvert ? -1 : 1) * MP.cmp(a.m.get(), b.m.get(), specOrder);
   }

   void sort ()
   {
      _terms.bubbleSort(cmpDefault);
   }
   
   void sort (MonoPool::ORDER o, bool inverse = false)
   {
      specOrder = o;
      specInvert = inverse;
      _terms.bubbleSort(cmpSpec);
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

   void toStr(Array<char>& buf) const {
      ArrayOutput output(buf);
      print(output);
      output.writeByte(0);
   }

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
               output.printf("*");
            MP.print(output, t.m.get());
         }
      }
   }

   void clear ()
   {
      _terms.clear();
   }

   void mul (const Polynomial& a, Monomial m, CFTYPE cf) {
      copy(a);
      mul(m);
      mulnum(cf);
   }

   void mul (Monomial m) {
      if (MP.totalDeg(m) == 0)
         return;
      for (int i = begin(); i < end(); i = next(i)) {
         Term& t = _terms.at(i);
         t.m.set(MP.mul(t.m.get(), m));
      }
   }

   void sum (const Polynomial& a, const Polynomial& b, CFTYPE fr = 1, CFTYPE fa = 1) {
      copy(a);
      add(b);
   }

   void add (const Polynomial& a, CFTYPE fr = 1, CFTYPE fa = 1) {
      int ri = begin(), ai = a.begin();
      while (ri < end() && ai < a.end()) {
         Term& tr = at(ri);
         const Term& ta = a.at(ai);
         int c = MP.cmp(tr.m.get(), ta.m.get());
         if (c > 0) {
            tr.f *= fr;
            ri = next(ri);
         } else if (c < 0) {
            insertTerm(ta.m.get(), ta.f * fa, ri);
            ai = a.next(ai);
         } else {
            tr.f = tr.f * fr + ta.f * fa;
            if (tr.f == 0) {
               int t = ri;
               ri = next(ri);
               ai = a.next(ai);
               _terms.remove(t);
            } else {
               ri = next(ri);
               ai = a.next(ai);
            }
         }
      }
      while (ri < end()) {
         Term& tr = at(ri);
         tr.f *= fr;
         ri = next(ri);
      }
      while (ai < a.end()) {
         const Term& ta = a.at(ai);
         addTerm(ta.m.get(), ta.f * fa);
         ai = a.next(ai);
      }
   }

   void simplify () {
      sort();
      for (int i = begin(), j; i < end() && next(i) < end();) {
         while ((j = next(i)) < end() && MP.equals(m(i), m(j))) {
            _terms.at(i).f += _terms.at(j).f;
            _terms.remove(j);
         }
         j = next(i);
         if (_terms.at(i).f == 0)
            _terms.remove(i);
         i = j;
      }
   }

   void mulnum (CFTYPE f) {
      for (int i = _terms.begin(); i < _terms.end(); i = _terms.next(i))
         _terms[i].f *= f;
   }

   void copy (const Polynomial& a) {
      clear();
      append (a, 1);
   }

   void append (const Polynomial& a, CFTYPE f) {
      for (int i = a.begin(); i < a.end(); i = a.next(i)) {
         const Term& t = a.at(i);
         addTerm(t.m.get(), t.f * f);
      }
   }

   void append (const Polynomial& a) {
      for (int i = a.begin(); i < a.end(); i = a.next(i)) {
         const Term& t = a.at(i);
         addTerm(t.m.get(), t.f);
      }
   }

   void append (const Term& t) {
      addTerm(t.m.get(), t.f);
   }
private:
   static Pool<ObjList<Term>::Elem> _pool;
   ObjList<Term> _terms;
   Polynomial (const Polynomial&);
};

#endif //__TNIRPS_POLYNOMIAL_H__
