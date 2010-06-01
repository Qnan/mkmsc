#ifndef __TNIRPS_NUM_Q_H__
#define	__TNIRPS_NUM_Q_H__

#include <gmp.h>

class QNum {
public:
   QNum () {
      mpq_init(v);
   }

   QNum (int i) {
      mpq_init(v);
      mpq_set_si(v, i);
   }

   void copy (QNum& a, const QNum& b) {
      mpq_init(a.v);
      mpq_set(a.v, b.v);
   }

   void set (int v) {
      mpq_set_si(v);
   }

   void set (const char* s) {
      mpq_set_str(s);
   }
   
   void add (const QNum& a, const QNum& b) {
      mpq_add(v, a.v, b.v);
   }

   void sub (const QNum& a, const QNum& b) {
      mpq_sub(v, a.v, b.v);
   }

   void mul (const QNum& a, const QNum& b) {
      mpq_mul(v, a.v, b.v);
   }

   void div (const QNum& a, const QNum& b) {
      mpq_div(v, a.v, b.v);
   }

   int cmp (const QNum& a, const QNum& b) {
      return mpq_cmp(a.v, b.v);
   }

   int cmp (const QNum& a, int b) {
      return mpq_cmp_si(a.v, b);
   }

   ~QNum () {
      mpq_clear(v);
   }
private:
   mpq_t v;
   QNum (const QNum& other);
};


#endif	/* __TNIRPS_NUM_Q_H__ */

