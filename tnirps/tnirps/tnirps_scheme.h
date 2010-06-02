#ifndef __TNIRPS_SCHEME_H__
#define __TNIRPS_SCHEME_H__

#include "array.h"
#include "tnirps_monopool.h"
#include "tnirps_polynomial.h"

#define SCHEME_CALLBACKS_DEFINE(className) \
   static void cb_init (const Array<Monomial>& mm, int total, void* context) { ((className*)context)->init(mm, total); } \
   static void cb_add (int id, int a, int b, void* context) { ((className*)context)->add(id, a, b); } \
   static void cb_mul (int id, int a, int b, void* context) { ((className*)context)->mul(id, a, b); } \
   static void cb_mulnum (int id, int a, const Cf& num, void* context) { ((className*)context)->mulnum(id, a, num); } \
   static void cb_yield (int id, void* context) { ((className*)context)->yield(id); }

#define SCHEME_CALLBACKS_SET(scheme) \
   scheme.cb_init = cb_init; \
   scheme.cb_add = cb_add; \
   scheme.cb_mul = cb_mul; \
   scheme.cb_mulnum = cb_mulnum; \
   scheme.cb_yield = cb_yield;

class Exception;

class Scheme {
public:
   enum OPTYPE {
      OP_SET,
      OP_ADD,
      OP_MUL,
      OP_MULNUM
   };
   struct Op {
      void init (OPTYPE type, int r, int a, int b) {
         this->type = type;
         this->r = r;
         this->a = a;
         this->b = b;
      }
      OPTYPE type;
      int r;
      int a;
      int b;
   };

   Scheme () : totalCount(0) {}

   mutable void (*cb_init) (const Array<Monomial>& mm, int total, void* context);
   mutable void (*cb_add) (int id, int a, int b, void* context);
   mutable void (*cb_mul) (int id, int a, int b, void* context);
   mutable void (*cb_mulnum) (int id, int a, const Cf& num, void* context);
   mutable void (*cb_yield) (int id, void* context);

   void proceed (void* context) const {
      cb_init(monomials, totalCount, context);
      for (int i = 0; i < ops.size(); ++i) {
         const Op& op = ops[i];
         switch (op.type) {
            case OP_ADD:
               cb_add(op.r, op.a, op.b, context);
               break;
            case OP_MUL:
               cb_mul(op.r, op.a, op.b, context);
               break;
            case OP_MULNUM:
               cb_mulnum(op.r, op.a, coeffs.at(op.b), context);
               break;
            default:
               throw Exception("Uknown operation type");
         }
      }
      cb_yield(resultId, context);
   }

//   const ObjPool<NumPtr>& coeffs () const {
//      return _coeffs;
//   }

   Array<Monomial> monomials;
   int totalCount;
   int resultId;
   Array<Op> ops;
   ObjPool<Cf> coeffs;

private:
   Scheme (const Scheme&); // no implicit copy
};

#endif //__TNIRPS_SCHEME_H__
