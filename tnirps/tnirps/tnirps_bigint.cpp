#include "tnirps_bigint.h"
#include "exception.h"
#include "tnirps_common.h"
#include <stdarg.h>

#define TO_UNSIGNED(a) (a >= 0 ? (unsigned long)(a) : throw Exception("Positive value expected"))

void BigInt::init (bigint_t& a) {
   mpz_init(a);
}
int BigInt::cmp (const bigint_t& a, const bigint_t& b) {
   return mpz_cmp(a, b);
}
int BigInt::cmp (const bigint_t& a, int b) {
   return mpz_cmp_si(a, b);
}
void BigInt::clear (bigint_t& a) {
   mpz_clear(a);
}
void BigInt::printf (const char* format, ...) {
//   va_list va;
//
//   va_start(va, format);
//   gmp_vprintf(format, va);
//   va_end(va);
//
}
void BigInt::set (bigint_t& r, const bigint_t& a) {
   mpz_set(r, a);
}
void BigInt::set (bigint_t& r, const long a) {
   mpz_set_si(r, a);
}
void BigInt::set64 (bigint_t& r, const long long a) {
   long long b = ABS(a);
   unsigned u = b >> 32, l = b & 0xFFFFFFFFlu;
   if (u != 0) {
      mpz_set_ui(r, u);
      mpz_mul_2exp(r, r, 32);
   }
   mpz_add_ui(r, r, l);
   if (a < 0)
      mpz_mul_si(r, r, -1);
}
void BigInt::set (bigint_t& r, const char* str) {
   mpz_set_str(r, str, 10);
}
void BigInt::swap (bigint_t& a, bigint_t& b) {
   mpz_swap(a, b);
}
void BigInt::add (bigint_t& r, const bigint_t& a, const bigint_t& b) {
   mpz_add(r, a, b);
}
void BigInt::add (bigint_t& r, const bigint_t& a, const long b) {
   if (b >= 0)
      mpz_add_ui(r, a, TO_UNSIGNED(b));
   else
      mpz_add_ui(r, a, TO_UNSIGNED(-b));
}
void BigInt::sub (bigint_t& r, const bigint_t& a, const bigint_t& b) {
   mpz_sub(r, a, b);
}
void BigInt::sub (bigint_t& r, const bigint_t& a, const long b) {
   mpz_sub_ui(r, a, TO_UNSIGNED(b));
}
void BigInt::mul (bigint_t& r, const bigint_t& a, const bigint_t& b) {
   mpz_mul(r, a, b);
}
void BigInt::mul (bigint_t& r, const bigint_t& a, const long b) {
   mpz_mul_si(r, a, b);
}
void BigInt::addmul (bigint_t& r, const bigint_t& a, const bigint_t& b) {
   mpz_addmul(r, a, b);
}
void BigInt::addmul (bigint_t& r, const bigint_t& a, const long b) {
   mpz_addmul_ui(r, a, TO_UNSIGNED(b));
}
void BigInt::submul (bigint_t& r, const bigint_t& a, const bigint_t& b) {
   mpz_submul(r, a, b);
}
void BigInt::submul (bigint_t& r, const bigint_t& a, const long b) {
   mpz_submul_ui(r, a, TO_UNSIGNED(b));
}
void BigInt::mod (bigint_t& r, const bigint_t& a, const bigint_t& b) {
   mpz_mod(r, a, b);
}
void BigInt::mod (bigint_t& r, const bigint_t& a, const long b) {
   mpz_mod_ui(r, a, TO_UNSIGNED(b));
}
void BigInt::pow (bigint_t& r, const bigint_t& base, const long exp) {
   mpz_pow_ui(r, base, TO_UNSIGNED(exp));
}
void BigInt::pow (bigint_t& r, const long base, const long exp) {
   mpz_ui_pow_ui(r, base, TO_UNSIGNED(exp));
}
void BigInt::powmod (bigint_t& r, const bigint_t& base, const bigint_t& exp, const bigint_t& mod) {
   mpz_powm(r, base, exp, mod);
}
void BigInt::powmod (bigint_t& r, const bigint_t& base, const long exp, const bigint_t& mod) {
   mpz_powm_ui(r, base, TO_UNSIGNED(exp), mod);
}
void BigInt::gcd (bigint_t& r, const bigint_t& a, const bigint_t& b) {
   mpz_gcd(r, a, b);
}
void BigInt::gcd (bigint_t& r, const bigint_t& a, const long b) {
   mpz_gcd_ui(r, a, TO_UNSIGNED(b));
}
void BigInt::abs (bigint_t& r, const bigint_t& a) {
   mpz_abs(r, a);
}

void BigInt::neg (bigint_t& r, const bigint_t& a) {
   mpz_neg(r, a);
}
