#ifndef __TNIRPS_NUM_Q_H__
#define	__TNIRPS_NUM_Q_H__

#include "output.h"
#include "tnirps_common.h"
#include <gmp.h>

#if QRING
class Cf {
public:
   Cf () {
      mpq_init(v);
   }

   ~Cf () {
      mpq_clear(v);
   }

   mpq_t v;
private:
   Cf (const Cf& other);
};

class Ring {
public:

   static void copy (Cf& a, const Cf& b) {
      mpq_init(a.v);
      mpq_set(a.v, b.v);
   }

   static void set (Cf& a, int v) {
      mpq_set_si(a.v, v, 1);
   }

   static void set (Cf& a, const char* s) {
      mpq_set_str(a.v, s, 10);
   }

   static void add (Cf& r, const Cf& a, const Cf& b) {
      mpq_add(r.v, a.v, b.v);
   }

   static void sub (Cf& r, const Cf& a, const Cf& b) {
      mpq_sub(r.v, a.v, b.v);
   }

   static void mul (Cf& r, const Cf& a, const Cf& b) {
      mpq_mul(r.v, a.v, b.v);
   }

   static void div (Cf& r, const Cf& a, const Cf& b) {
      mpq_div(r.v, a.v, b.v);
   }

   static void abs (Cf& r, const Cf& a) {
      mpq_abs(r.v, a.v);
   }

   static void neg (Cf& r, const Cf& a) {
      mpq_neg(r.v, a.v);
   }

   static int cmp (const Cf& a, const Cf& b) {
      return mpq_cmp(a.v, b.v);
   }

   static int cmp (const Cf& a, int b) {
      return mpq_cmp_si(a.v, b, 1);
   }

   static void print (Output& output, const Cf& a) {
      int sz = mpz_sizeinbase(mpq_denref(a.v), 10) + mpz_sizeinbase(mpq_numref(a.v), 10) + 10;
      buf.resize(sz);
      gmp_sprintf(buf.ptr(), "%Qd", a.v);
      output.printf("%s", buf.ptr());
   }
private:
   static Array<char> buf;
};
#else
class Cf {
public:
   Cf () : v(0) {}
   int v;
private:
   Cf (const Cf& other);
};

class Ring {
   const int p;
public:
   Ring () : p(31) {
      for (int i = 0; i < p; ++i) {
         int t = i, q;
         while ((q = (t * i) % p) != i)
            t = q;
         inv[i] = t;         
      }
   }

   void copy (Cf& a, const Cf& b) {
      a.v = b.v;
   }

   void set (Cf& a, int v) {
      a.v = v % p;
   }

   void set (Cf& a, const char* s) {
      sscanf(s, "%d", &a.v);
      a.v = a.v % p;
   }

   void add (Cf& r, const Cf& a, const Cf& b) {
      r.v = (a.v + b.v) % p;
   }

   void sub (Cf& r, const Cf& a, const Cf& b) {
      r.v = (p + a.v - b.v) % p;
   }

   void mul (Cf& r, const Cf& a, const Cf& b) {
      r.v = (a.v * b.v) % p;
   }

   void div (Cf& r, const Cf& a, const Cf& b) {
      r.v = (a.v * inv[b.v]) % p;
   }

   void abs (Cf& r, const Cf& a) {
      r.v = a.v;
   }

   void neg (Cf& r, const Cf& a) {
      r.v = p - a.v;
   }

   int cmp (const Cf& a, const Cf& b) {
      return a.v - b.v;
   }

   int cmp (const Cf& a, int b) {
      return a.v - b;
   }

   void print (Output& output, const Cf& a) {
      output.printf("%d", a.v);
   }

private:
   int inv[p];
};

extern Ring<31> R;
#endif

#endif	/* __TNIRPS_NUM_Q_H__ */

