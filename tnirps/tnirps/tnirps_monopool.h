#ifndef __TNIRPS_MONOPOOL_H__
#define __TNIRPS_MONOPOOL_H__

#include "obj_pool.h"
class Exception;

typedef int Monomial;

class MonoPool {
public:
   static MonoPool& inst () {
      return _inst;
   }
   enum ORDER {LEX, DRL};
   void setOrder (ORDER o) {
      _order = o;
      if (_order == LEX)
         _cmp = _Mon::cmpLex;
      else if (_order == DRL)
         _cmp = _Mon::cmpDegRevLex;
      else
         throw Exception("Order unknown");
   }

   Monomial init (const int* idcs, const int* degs, const int length) {
      int id = _pool.add();
      _pool.at(id).init(idcs, degs, length);
      return id;
   }

   Monomial init (const int* degs, const int length) {
      int id = _pool.add();
      _pool.at(id).init(degs, length);
      return id;
   }

   Monomial init (const char* expr, int begin, int end, const char* vnames) {
      int var = -1, deg = 0;
      static int degs[1024];
      int len = strlen(vnames);
      for (int j = 0; j < len; ++j)
         degs[j] = 0;
      if (end <= 0)
         end = strlen(expr);
      char c;
      for (int i = begin; i < end; ++i) {
         c = expr[i];
         if (isalpha(c)) {
            if (var >= 0)
               degs[var]++;
            for (var = 0; var < len && vnames[var] != c; ++var);
            if (var == len)
               throw Exception("Variable name unrecognized: %c", c);
         } else if (isdigit(c)) {
            if (sscanf(expr+i, "%d", &deg) != 1 || var < 0)
               throw Exception("Error parsing expression %s, starting %s", expr, expr+i);
            degs[var] += deg;
            var = -1;
            deg = 0;
            while (i+1 < end && isdigit(expr[i+1]))
               ++i;
         } else if (isspace(c) || c == '*' || c == '^') {
         } else
            throw Exception("Unexpected symbol at %s, starting %s", expr, expr+i);
      }
      if (var >= 0)
         degs[var]++;
      return init(degs, len);
   }

   int length (Monomial id) const {
      return _pool.at(id).length();
   }
   
   void release (Monomial id) {
      _pool.remove(id);
   }

   void print(Monomial id) const {
      _pool.at(id).print(varName);
   }

   Monomial clone(Monomial id) {
      int id2 = _pool.add();
      _pool.at(id2).copy(_pool.at(id));
      return id2;
   }

   Monomial mul(Monomial id1, Monomial id2) {
      int r = _pool.add();
      _pool.at(r).mul(_pool.at(id1), _pool.at(id2));
      return r;
   }

   Monomial div(Monomial id1, Monomial id2) {
      int r = _pool.add();
      _pool.at(r).div(_pool.at(id1), _pool.at(id2));
      return r;
   }

   Monomial gcd(Monomial id1, Monomial id2) {
      int r = _pool.add();
      _pool.at(r).gcd(_pool.at(id1), _pool.at(id2));
      return r;
   }

   bool divides(Monomial id1, Monomial id2) { // id2 divides id1
      return _Mon::divides(_pool.at(id1), _pool.at(id2));
   }

   bool equals(Monomial id1, Monomial id2) {
      return _Mon::equals(_pool.at(id1), _pool.at(id2));
   }

   int cmp (Monomial id1, Monomial id2) const {
      return _cmp(_pool.at(id1), _pool.at(id2));
   }

   int countHash(Monomial id) const
   {
      return _pool.at(id).countHash();
   }

   const char* (*varName) (int idx);

private:
   struct Var {
      Var (int i, int d) : idx(i), deg(d) {}
      int idx;
      int deg;
   };

   class _Mon {
   public:
      _Mon () {}

      void copy (const _Mon& a) {
         vars.clear();
         vars.copy(a.vars);
      }

      void init (const int* idcs, const int* degs, int length)
      {
         vars.clear();
         for (int i = 0; i < length; ++i)
            vars.push(Var(idcs[i], degs[i]));
      }

      void init (const int* degs, int length)
      {
         vars.clear();
         for (int i = 0; i < length; ++i)
            if (degs[i] > 0)
               vars.push(Var(i, degs[i]));
      }

      void print (const char* (*vnames) (int) = NULL) const
      {
         for (int i = 0; i < vars.size(); ++i) {
            if (i > 0)    
               printf("*");
            if (vnames)
               printf("%s", vnames(vars[i].idx));
            else
               printf("[%i]", vars[i].idx);
            if (vars[i].deg > 1)
               printf("^%i", vars[i].deg);
         }
      }

      void mul (const _Mon& a, const _Mon& b)
      {
         vars.clear();
         int i = 0, j = 0, i1 = a.vars.size(), j1 = b.vars.size();
         while (i < i1 && j < j1) {             
            const Var& va = a.vars[i], vb = b.vars[j];
            if (va.idx == vb.idx) {
               vars.push(Var(va.idx, va.deg + vb.deg));
               ++i;
               ++j;
            } else if (va.idx < vb.idx) {
               vars.push(Var(va.idx, va.deg));
               ++i;
            } else {
               vars.push(Var(vb.idx, vb.deg));
               ++j;
            }  
         }
         while (i < i1) {
            vars.push(Var(a.vars[i].idx, a.vars[i].deg));
            ++i;
         }
         while (j < j1) {
            vars.push(Var(b.vars[j].idx, b.vars[j].deg));
            ++j;
         }
      }

      void div (const _Mon& a, const _Mon& b)
      {
         vars.clear();
         int i = 0, j = 0, i1 = a.vars.size(), j1 = b.vars.size(), d;
         while (i < i1 && j < j1) {             
            const Var& va = a.vars[i], vb = b.vars[j];
            if (va.idx == vb.idx) {
               d = va.deg - vb.deg;
               if (d > 0)
                  vars.push(Var(va.idx, d));
               ++i;
               ++j;
            } else if (va.idx < vb.idx) {
               vars.push(Var(va.idx, va.deg));
               ++i;
            } else {
               throw Exception("Monomes don't divide");
            }  
         }
         while (i < i1) {
            vars.push(Var(a.vars[i].idx, a.vars[i].deg));
            ++i;
         }
         if (j < j1)
            throw Exception("Monomes don't divide");
      }

      void gcd (const _Mon& a, const _Mon& b)
      {
         vars.clear();
         int i = 0, j = 0, i1 = a.vars.size(), j1 = b.vars.size();
         while (i < i1 && j < j1) {             
            const Var& va = a.vars[i], vb = b.vars[j];
            if (va.idx == vb.idx) {
               vars.push(Var(va.idx, __min(va.deg, vb.deg)));
               ++i;
               ++j;
            } else if (va.idx < vb.idx) {
               ++i;
            } else {
               ++j;
            }  
         }
      }

      int countHash() const
      {
         static long k[]={0,4,8,12,16,20,24,28};
         static int and = 0x7;
         int hash = 0;
         for(int i = 0; i < vars.size(); ++i)
            hash ^= vars[i].deg << k[vars[i].idx & and];
         return hash;
      }

      static bool divides (const _Mon& a, const _Mon& b)
      {
         int i = 0, j = 0, i1 = a.vars.size(), j1 = b.vars.size();
         while (i < i1 && j < j1) {             
            const Var& va = a.vars[i], vb = b.vars[j];
            if (va.idx > vb.idx)
               return false;
            if (va.idx == vb.idx)  {
               if (vb.deg > va.deg)
                  return false;
               ++i;
               ++j;
            } else {
               ++i;
            }
         }
         if (j < j1)
            return false;
         return true;
      }

      static bool equals (const _Mon& a, const _Mon& b)
      {
         int i1 = a.vars.size();
         if (i1 != b.vars.size())
            return false;
         for (int i = 0; i < i1; ++i) {
            const Var& va = a.vars[i], vb = b.vars[i];
            if (va.idx != vb.idx)
               return false;
            if (va.deg != vb.deg)
               return false;
         }
         return true;
      }

      static int cmpLex (const _Mon& a, const _Mon& b)
      { // 1 if a > b
         int i = 0, j = 0, i1 = a.vars.size(), j1 = b.vars.size();
         while (i < i1 && j < j1) {             
            const Var& va = a.vars[i], vb = b.vars[j];
            if (va.idx < vb.idx)
               return 1;
            if (va.idx > vb.idx)
               return -1;
            if (va.deg < vb.deg)
               return -1;
            if (va.deg > vb.deg)
               return 1;
            ++i;
            ++j;
         }
         if (j < j1)
            return -1;
         if (i < i1)
            return 1;
         return 0;
      }

      static int cmpRevLex (const _Mon& a, const _Mon& b)
      { // 1 if a > b
         int i = a.vars.size() - 1, j = b.vars.size() - 1;
         while (i >= 0 && j >= 0) {
            const Var& va = a.vars[i], vb = b.vars[j];
            if (va.idx > vb.idx)
               return 1;
            if (va.idx < vb.idx)
               return -1;
            if (va.deg < vb.deg)
               return -1;
            if (va.deg > vb.deg)
               return 1;
            --i;
            --j;
         }
         if (j >= 0)
            return -1;
         if (i >= 0)
            return 1;
         return 0;
      }

      static int cmpDegRevLex (const _Mon& a, const _Mon& b)
      { // 1 if a > b
         int da = 0, db = 0;
         for (int k = 0; k < a.vars.size(); ++k)
            da += a.vars[k].deg;
         for (int k = 0; k < b.vars.size(); ++k)
            db += b.vars[k].deg;
         if (da > db)
            return 1;
         if (db > da)
            return -1;
         int i = a.vars.size() - 1, j = b.vars.size() - 1;
         while (i >= 0 && j >= 0) {
            const Var& va = a.vars[i], vb = b.vars[j];
            if (va.idx > vb.idx)
               return 1;
            if (va.idx < vb.idx)
               return -1;
            if (va.deg < vb.deg)
               return -1;
            if (va.deg > vb.deg)
               return 1;
            --i;
            --j;
         }
         if (j >= 0)
            return -1;
         if (i >= 0)
            return 1;
         return 0;
      }

      int length () const {
         return vars.size();
      }

   private:
      Array<Var> vars;
      _Mon (const _Mon&);
   };

   ObjPool<_Mon> _pool;
   int (*_cmp) (const _Mon& a, const _Mon& b);
   ORDER _order;
   static MonoPool _inst;

   MonoPool () {}
   MonoPool (const MonoPool&);
};

extern MonoPool& MP; // global!

#endif //__TNIRPS_MONOPOOL_H__
