#ifndef __TNIRPS_NUM_Q_H__
#define	__TNIRPS_NUM_Q_H__

#include "output.h"
#include "tnirps_common.h"
#include <gmp.h>

//QRING
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
   static void init () {
   }

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

