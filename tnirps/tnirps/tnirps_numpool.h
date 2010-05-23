#ifndef __TNIRPS_NUMPOOL_H__
#define __TNIRPS_NUMPOOL_H__

#include "output.h"
#include "obj_pool.h"
#include "tnirps_hashset.h"
#include "tnirps_common.h"
#include "tnirps_bigint.h"

class NumPool {
public:
   typedef long long Number;
   static NumPool& inst () {
      return _inst;
   }

   Number copy (Number a) {
      if (!isSmall(a))
         pool.at(decodeBig(a)).refcnt++;
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

   void print (Number a) {
      if (isSmall(a))
         printf("%lld", decodeSmall(a));
      else
         gmp_printf("%i:%Zd", decodeBig(a), pool.at(decodeBig(a)).bigVal);
   }
   //Number div (Number a, Number b);
private:
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

#endif	/* __TNIRPS_NUMPOOL_H__ */

