#ifndef __TNIRPS_NUM_Q_H__
#define	__TNIRPS_NUM_Q_H__

#include "output.h"
#include "pool.h"
#include "tnirps_common.h"
#include <gmp.h>

#if QRING
class Ring {
public:
   class Cf {
   public:
      Cf () : id(-1) {
      }

      Cf (int v) : id(-1) {
         Ring::set(*this, v);
      }

      bool operator!= (int v) {
         return id < 0 || mpq_cmp_si(get(), v, 1);
      }

      ~Cf () {
         set(-1);
      }

      void set (int n) {
         if (id >= 0)
            Ring::refDec(id);
         id = n;
         if (n >= 0)
            Ring::refInc(id);
      }

      mpq_t& get () {
         return Ring::_get(id);
      }

      int gid () {
         return id;
      }

      Cf (const Cf& other) : id(-1) {
         set(other.id);
      }

   private:
      int id;
   };

   static void init () {
   }

   static void copy (Cf& a, Cf b) {
      a.set(b.gid());
   }

   static void set (Cf& a, int v) {
      a.set(alloc());
      mpq_set_si(a.get(), v, 1);
   }

   static void set (Cf& a, const char* s) {
      a.set(alloc());
      mpq_set_str(a.get(), s, 10);
   }

   static void add (Cf& r, Cf a, Cf b) {
      r.set(alloc());
      mpq_add(r.get(), a.get(), b.get());
   }

   static void sub (Cf& r, Cf a, Cf b) {
      r.set(alloc());
      mpq_sub(r.get(), a.get(), b.get());
   }

   static void mul (Cf& r, Cf a, Cf b) {
      r.set(alloc());
      mpq_mul(r.get(), a.get(), b.get());
   }

   static void div (Cf& r, Cf a, Cf b) {
      r.set(alloc());
      mpq_div(r.get(), a.get(), b.get());
   }

   static void abs (Cf& r, Cf a) {
      r.set(alloc());
      mpq_abs(r.get(), a.get());
   }

   static void neg (Cf& r, Cf a) {
      r.set(alloc());
      mpq_neg(r.get(), a.get());
   }

   static int cmp (Cf a, Cf b) {
      return mpq_cmp(a.get(), b.get());
   }

   static int cmp (Cf a, int b) {
      return mpq_cmp_si(a.get(), b, 1);
   }

   static void print (Output& output, Cf a) {
      int sz = mpz_sizeinbase(mpq_denref(a.get()), 10) + mpz_sizeinbase(mpq_numref(a.get()), 10) + 10;
      buf.resize(sz);
      gmp_sprintf(buf.ptr(), "%Qd", a.get());
      output.printf("%s", buf.ptr());
   }

   static int alloc () {
      int id = pool.add();
      pool.at(id).refcnt = 0;
      mpq_init(pool.at(id).v);
      return id;
   }

   static void refInc (int id) {
      //printf("inc: %d\n", id);
      ++pool.at(id).refcnt;
   }

   static void refDec (int id) {
      //printf("dec: %d\n", id);
      --pool.at(id).refcnt;
      if (pool.at(id).refcnt == 0) {
//         printf("free: %d\n", id);
         mpq_clear(pool.at(id).v);
         pool.remove(id);
      }
   }
   static mpq_t& _get (int id) {
      return pool.at(id).v;
   }

   struct Cell {
      mpq_t v;
      int refcnt;
   };
private:
   static Array<char> buf;
   static Pool<Cell> pool;

};

typedef Ring::Cf Cf;

#else

typedef int Cf;

class Ring {
   static const int p = 31;
public:
   static void init () {
      inv[0] = 0;
      for (int i = 1; i < p; ++i) {
         int t = i, q;
         while ((q = (t * i) % p) != 1)
            t = q;
         inv[i] = t;         
      }
//      for (int i = 0; i < p; ++i)
//         printf (" %2d", i);
//      printf ("\n");
//      for (int i = 0; i < p; ++i)
//         printf (" %2d", inv[i]);
//      printf ("\n");
   }

   static void copy (Cf& a, Cf b) {
      a = b;
   }

   static void set (Cf& a, int v) {
      a = (v % p + p) % p; // trick to convert negative numbers
   }

   static void set (Cf& a, const char* s) {
      sscanf(s, "%d", &a);
      a =(a % p + p) % p;
   }

   static void add (Cf& r, Cf a, Cf b) {
      r = (a + b) % p;
   }

   static void sub (Cf& r, Cf a, Cf b) {
      r = (p + a - b) % p;
   }

   static void mul (Cf& r, Cf a, Cf b) {
      r = (a * b) % p;
   }

   static void div (Cf& r, Cf a, Cf b) {
      r = (a * inv[b]) % p;
   }

   static void abs (Cf& r, Cf a) {
      r = a;
   }

   static void neg (Cf& r, Cf a) {
      r = p - a;
   }

   static int cmp (Cf a, Cf b) {
      return a - b;
   }

//   static int cmp (Cf a, int b) {
//      return a - b;
//   }

   static void print (Output& output, Cf a) {
      output.printf("%d", a);
   }

private:
   static int inv[p];
};

extern Ring R;
#endif

#endif	/* __TNIRPS_NUM_Q_H__ */

