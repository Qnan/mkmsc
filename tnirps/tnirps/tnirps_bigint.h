#ifndef __TNIRPS_BIGINT_H__
#define	__TNIRPS_BIGINT_H__

#include <gmp.h>

typedef mpz_t bigint_t;
class BigInt {
public:
   static void init (bigint_t& a);
   static int  cmp (const bigint_t& a, const bigint_t& b);
   static int  cmp (const bigint_t& a, int b);
   static void clear (bigint_t& a);
   static void printf (const char* format, ...);
   static void set (bigint_t& r, const bigint_t& a);
   static void set (bigint_t& r, const long a);
   static void set (bigint_t& r, const char* str);
   static void swap (bigint_t& a, bigint_t& b);
   static void add (bigint_t& r, const bigint_t& a, const bigint_t& b);
   static void add (bigint_t& r, const bigint_t& a, const long b);
   static void sub (bigint_t& r, const bigint_t& a, const bigint_t& b);
   static void sub (bigint_t& r, const bigint_t& a, const long b);
   static void mul (bigint_t& r, const bigint_t& a, const bigint_t& b);
   static void mul (bigint_t& r, const bigint_t& a, const long b);
   static void addmul (bigint_t& r, const bigint_t& a, const bigint_t& b);
   static void addmul (bigint_t& r, const bigint_t& a, const long b);
   static void submul (bigint_t& r, const bigint_t& a, const bigint_t& b);
   static void submul (bigint_t& r, const bigint_t& a, const long b);
   static void mod (bigint_t& r, const bigint_t& a, const bigint_t& b);
   static void mod (bigint_t& r, const bigint_t& a, const long b);
   static void pow (bigint_t& r, const bigint_t& base, const long exp);
   static void pow (bigint_t& r, const long base, const long exp);
   static void powmod (bigint_t& r, const bigint_t& base, const bigint_t& exp, const bigint_t& mod);
   static void powmod (bigint_t& r, const bigint_t& base, const long exp, const bigint_t& mod);
   static void gcd (bigint_t& r, const bigint_t& a, const bigint_t& b);
   static void gcd (bigint_t& r, const bigint_t& a, const long b);
};
typedef BigInt BI;

#endif	/* __TNIRPS_BIGINT_H__ */

