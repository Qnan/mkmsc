#ifndef __TNIRPS_POLYNOMIAL_H__
#define __TNIRPS_POLYNOMIAL_H__

#include "obj_pool.h"
#include "obj_list.h"
#include "tnirps_monomial.h"
#include "tnirps_monopool.h"
#include "tnirps_numpool.h"

class Polynomial {
public:
   struct Term {
      Term () {
      }
      MonoPtr m;
      NumPtr f;
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
         printf("%s\n", buf.ptr());
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

         addTerm(m, NumPtr(NP.init(coeff)));
      }
   }

   int addTerm (Monomial m, const NumPtr& f)
   {                      
      int id = _terms.add();
      Term& t = _terms.at(id);
      t.f.set(f.get());
      t.m.set(m);
      return id;
   }

   int insertTerm (int m, const NumPtr&  f, int before)
   {     
      int id = _terms.insertBefore(before);
      Term& t = _terms.at(id);
      t.f.set(f.get());
      t.m.set(m);
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
   const NumPtr& lc () const { return _terms.at(_terms.begin()).f; }
   Term& lt () const { return _terms.at(_terms.begin()); }

   void toStr(Array<char>& buf) const {
      ArrayOutput output(buf);
      print(output);
      output.writeByte(0);
   }

   void print (Output& output) const {
      for (int i = _terms.begin(); i < _terms.end(); i = _terms.next(i)) {
         const Term& t = _terms.at(i);
         int c = NP.cmp(t.f.get(), 0);
         if (i != _terms.begin())
            output.printf(" %c ", c >= 0 ? '+' : '-');
         else if (c < 0)
            output.printf("-");
         bool showVars = !MP.isUnit(t.m.get());
         NumPtr f(NP.abs(t.f.get()));
         bool showCf = (NP.cmp(f.get(), 1) != 0 || !showVars);
         if (showCf)
            NP.print(output, f.get());
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

   void mul (const Polynomial& a, Monomial m, const NumPtr* cf = NULL) {
      copy(a);
      mul(m);
      if (cf != NULL)
         mulnum(*cf);
   }

   void mul (Monomial m) {
      if (MP.totalDeg(m) == 0)
         return;
      for (int i = begin(); i < end(); i = next(i)) {
         Term& t = _terms.at(i);
         t.m.set(MP.mul(t.m.get(), m));
      }
   }

   void sum (const Polynomial& a, const Polynomial& b, const NumPtr* fa = NULL, const NumPtr* fb = NULL) {
      copy(a);
      add(b, fa, fb);
   }

   void add (const Polynomial& a, const NumPtr* fr = NULL, const NumPtr* fa = NULL) {
      int ri = begin(), ai = a.begin();
      while (ri < end() && ai < a.end()) {
         Term& tr = at(ri);
         const Term& ta = a.at(ai);
         int c = MP.cmp(tr.m.get(), ta.m.get());
         if (c > 0) {
            if (fr != NULL)
               tr.f.set(NP.mul(tr.f.get(), fr->get()));
            ri = next(ri);
         } else if (c < 0) {
            NumPtr cf(ta.f.get());
            if (fa != NULL)
               cf.set(NP.mul(cf.get(), fa->get()));
            insertTerm(ta.m.get(), cf, ri);
            ai = a.next(ai);
         } else {
            NumPtr cfr(tr.f.get()), cfa(ta.f.get());
            if (fr != NULL)
               cfr.set(NP.mul(tr.f.get(), fr->get()));
            if (fa != NULL)
               cfa.set(NP.mul(ta.f.get(), fa->get()));
            tr.f.set(NP.sum(cfr.get(), cfa.get()));
            if (NP.cmp(tr.f.get(), 0) == 0) {
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
         NumPtr cf(tr.f.get());
         if (fr != NULL)
            cf.set(NP.mul(cf.get(), fr->get()));
         tr.f.set(cf.get());
         ri = next(ri);
      }
      while (ai < a.end()) {
         const Term& ta = a.at(ai);
         NumPtr cf(ta.f.get());
         if (fa != NULL)
            cf.set(NP.mul(cf.get(), fa->get()));
         addTerm(ta.m.get(), cf);
         ai = a.next(ai);
      }
   }

   void simplify () {
      sort();
      for (int i = begin(), j; i < end() && next(i) < end();) {
         Term& t = _terms.at(i);
         while ((j = next(i)) < end() && MP.equals(m(i), m(j))) {
            t.f.set(NP.sum(t.f.get(), _terms.at(j).f.get()));
            _terms.remove(j);
         }
         j = next(i);
         if (NP.cmp(t.f.get(), 0) == 0)
            _terms.remove(i);
         i = j;
      }
   }

   void mulnum (const NumPtr& f) {
      for (int i = _terms.begin(); i < _terms.end(); i = _terms.next(i))
         _terms[i].f.set(NP.mul(_terms[i].f.get(), f.get()));
   }

   void copy (const Polynomial& a) {
      clear();
      append(a);
   }

   void append (const Polynomial& a, const NumPtr& f) {
      for (int i = a.begin(); i < a.end(); i = a.next(i)) {
         const Term& t = a.at(i);
         NumPtr cf(NP.mul(t.f.get(), f.get()));
         addTerm(t.m.get(), cf);
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
