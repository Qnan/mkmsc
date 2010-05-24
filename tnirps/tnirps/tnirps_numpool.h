#ifndef __TNIRPS_NUMPOOL_H__
#define __TNIRPS_NUMPOOL_H__

#include "output.h"
#include "obj_pool.h"
#include "tnirps_hashset.h"
#include "tnirps_common.h"
#include "tnirps_bigint.h"

typedef long long Number;

class NumPool {
public:
   static NumPool& inst () {
      return _inst;
   }

   Number copy (Number a) {
      if (!isSmall(a))
         pool.at(decodeBig(a)).refcnt++;
      return a;
   }
   void release (Number a) {
      if (!isSmall(a)) {
         int id = decodeBig(a);
         Item& item = pool.at(id);
         if (item.refcnt == 1) {
            pool.remove(id);
         } else {
            --item.refcnt;
         }
      }
   }
   Number init (long long v) {
      if (fits(v)) {
         return encodeSmall(v);
      } else {
         int id = pool.add();
         Item& item = pool.at(id);
         BI::set64(item.bigVal, v);
         return encodeBig(id);
      }
   }
   Number init (const char* str) {
      long long v = 0;
      sscanf(str, "%lld", &v);
      BI::set(tmp1, str);
      BI::set64(tmp2, v);

      if (BI::cmp(tmp1, tmp2) == 0 && fits(v)) {
         return encodeSmall(v);
      } else {
         int id = pool.add();
         BI::set(pool.at(id).bigVal, tmp1);
         return encodeBig(id);
      }
   }
   Number sum (Number a, Number b) {
      if (isSmall(a) && isSmall(b)) {
         long long va = decodeSmall(a), vb = decodeSmall(b), r;
         r = va + vb;
         if (r - va == vb)
            return init(r);
      } 
      return sumBig(a, b);
   }
   Number diff (Number a, Number b) {
      if (isSmall(a) && isSmall(b)) {
         long long va = decodeSmall(a), vb = decodeSmall(b), r;
         r = va - vb;
         if (r + vb == va)
            return init(r);
      }
      return diffBig(a, b);
   }

   Number mul (Number a, Number b) {
      if (isSmall(a) && isSmall(b)) {
         long long va = decodeSmall(a), vb = decodeSmall(b), r;
         r = va * vb;
         if ((va != 0 && r / va == vb) || (vb != 0 && r / vb == va))
            return init(r);
      }
      return mulBig(a, b);
   }

   Number pow (Number a, long b) {
      if (isSmall(a) && decodeSmall(a) == 1)
         return copy(a); // TODO: try using simple integers for low powers
      a = toBig(a);
      int r = pool.add();
      Item& ia = pool.at(decodeBig(a)), &ir = pool.at(r);
      BI::pow(ir.bigVal, ia.bigVal, b);
      return encodeBig(r);
   }

   Number abs (Number a) {
      if (isSmall(a))
         return init(ABS(decodeSmall(a)));
      else {
         int id = decodeBig(a);
         if (BI::cmp(pool.at(id).bigVal, 0) >= 0)
            return copy(a);
         int b = pool.add();
         BI::abs(pool.at(b).bigVal, pool.at(id).bigVal);
         return encodeBig(b);
      }
   }

   Number neg (Number a) {
      if (isSmall(a))
         return init(-decodeSmall(a));
      else {
         int id = decodeBig(a);
         int b = pool.add();
         BI::abs(pool.at(b).bigVal, pool.at(id).bigVal);
         return encodeBig(b);
      }
   }

   int cmp (Number a, int b) {
      if (isSmall(a)) {
         int diff = decodeSmall(a) - b;
         if (diff > 0)
            return 1;
         else if (diff == 0)
            return 0;
         else
            return -1;
      } else {
         int id = decodeBig(a);
         return BI::cmp (pool.at(id).bigVal, b);
      }
   }

   void print (Number a) {
      if (isSmall(a))
         printf("%lld", decodeSmall(a));
      else
         gmp_printf("%i:%Zd", decodeBig(a), pool.at(decodeBig(a)).bigVal);
   }

   void print (Output& output, Number a) {
      if (isSmall(a))
         output.printf("%lld", decodeSmall(a));
      else {
         bigint_t& v = pool.at(decodeBig(a)).bigVal;
         int sz = mpz_sizeinbase(v, 10) + 2;
         buf.resize(sz);
         gmp_sprintf(buf.ptr(), "%Zd", v);
      }
   }
private:
   Array<char> buf;

   NumPool () {
      BI::init(tmp1);
      BI::init(tmp2);
   }
   ~NumPool () {
      BI::clear(tmp1);
      BI::clear(tmp2);
   }
   NumPool (const NumPool&);
   
   Number toBig (Number a) {
      if (!isSmall(a))
         return a;
      int r = pool.add();
      Item& ir = pool.at(r);
      BI::set64(ir.bigVal, decodeSmall(a));
      return encodeBig(r);
   }
   Number sumBig (Number a, Number b) {
      a = toBig(a);
      b = toBig(b);
      int r = pool.add();
      Item& ia = pool.at(decodeBig(a)), &ib = pool.at(decodeBig(b)), &ir = pool.at(r);
      BI::add(ir.bigVal, ia.bigVal, ib.bigVal);
      return encodeBig(r);
   }
   Number diffBig (Number a, Number b) {
      a = toBig(a);
      b = toBig(b);
      int r = pool.add();
      Item& ia = pool.at(decodeBig(a)), &ib = pool.at(decodeBig(b)), &ir = pool.at(r);
      BI::sub(ir.bigVal, ia.bigVal, ib.bigVal);
      return encodeBig(r);
   }
   Number mulBig (Number a, Number b) {
      a = toBig(a);
      b = toBig(b);
      int r = pool.add();
      Item& ia = pool.at(decodeBig(a)), &ib = pool.at(decodeBig(b)), &ir = pool.at(r);
      BI::mul(ir.bigVal, ia.bigVal, ib.bigVal);
      return encodeBig(r);
   }

   bool isSmall (Number num) {
      return (num & 1) == 0;
   }

   bool fits (long long v) {
      return (v * 2) >> 1 == v;
   }

   Number encodeBig (int id) {
      return (id * 2) + 1;
   }

   Number encodeSmall (long long id) {
      return (id * 2);
   }

   int decodeBig (Number n) {
      return (int)(n / 2);
   }

   long long decodeSmall (Number n) {
      return n / 2;
   }

   struct Item {
      Item() {
         BI::init(bigVal);
         refcnt = 1;
      }
      ~Item() {
         BI::clear(bigVal);
      }
      bigint_t bigVal;
      int refcnt;
   };

   static NumPool _inst;
   ObjPool<Item> pool;
   bigint_t tmp1, tmp2;
};

extern NumPool& NP; // global!

class NumPtr {
public:
   NumPtr () : _n(-1), _valid(false) {
   }
   explicit NumPtr (Number n) : _n(-1), _valid(false) {
      set(n);
   }
   void reset () {
      if (_valid)
         NP.release(_n);
      _n = -1;
      _valid = false;
   }
   void set (Number n) {
      reset();
      _n = NP.copy(n);
      _valid = true;
   }
   Number get () const {
      return _n;
   }
   ~NumPtr() {
      reset();
   }
private:
   Number _n;
   bool _valid;
   NumPtr (const NumPtr& mp);
};

#endif	/* __TNIRPS_NUMPOOL_H__ */

