#ifndef __TNIRPS_POLY_REDUCTION_H__
#define __TNIRPS_POLY_REDUCTION_H__

#include "profiling.h"
#include "tnirps_scheme.h"
#include "tnirps_monopool.h"
#include "tnirps_reduction.h"

//#define DBG(op) op
#undef DBG
#define DBG(op)

#define MONOEVAL 1

class Reductor {
public:
   Reductor (const ObjArray<Polynomial>& b) : basis(b), sr(b) {
        stack.reserve(1048576);
   }

   ~Reductor () {
   }

   void reduce (Polynomial& res, const Scheme& scheme) {
      SCHEME_CALLBACKS_SET(scheme);
      values.resize(scheme.totalCount);
      scheme.proceed(this);
      res.copy(result);
      DBG(for (int i = normalForms.begin(); i < normalForms.end(); i = normalForms.next(i)) {
         MP.print(sout, normalForms.key(i));
         printf("  :  ");
         normalForms.value(i).print(sout);
         printf("\n");
      })
   }

   SCHEME_CALLBACKS_DEFINE(Reductor);

private:
   bool reduceStep (Polynomial& p) {
      Polynomial t;
      Monomial lm = p.lm();
      int i;
      for (i = 0; i < basis.size(); ++i) {
         Monomial m = basis[i].lm();
         if (MP.divides(lm, m)) {
            Cf cf;
            DBG(printf("p: "); p.print(sout); printf("\n"));
            DBG(printf("gi: "); basis[i].print(sout); printf("\n"));
            Ring::div(cf, p.lc(), basis[i].lc());
            DBG(printf("cf: "); Ring::print(sout, p.lc()); printf(" / "); Ring::print(sout, basis[i].lc()); printf(" = "); Ring::print(sout, cf); printf("\n"));
            Ring::neg(cf, cf);
            t.mul(basis[i], MP.div(lm, m), &cf);
            p.add(t, Cf(1), Cf(1));
            //p.simplify();
            return true;
         }
      }
      return false;
   }

   
   //void init ()
   
   void evaluateMonomial (const Array<Monomial>& mm, int i) {
      const int* degs = MP.getDegs(mm[i]);
      Polynomial t[4];
      Cf c;
      Ring::set(c, 1);
      t[0].addTerm(MP.unit(), c);
      int k = 0;
#if MONOEVAL == 3
      for (int v = 1; v < MP.nvars(); ++v)
         if (degs[v] != degs[v-1])
            throw Exception("Error");
      int deg = degs[0];
      Array<int> bdegs;
      for (int v = 0; v < MP.nvars(); ++v) {
         bdegs.push(1);
      }

      Monomial base = MP.init(bdegs.ptr());
      t[0].clear();
      t[0].addTerm(base, 1);
      int max = 0;
      while ((deg >> (max+1)) != 0)
         ++max;
      for (;max>=0;--max) {
//      while (deg > 0) {
         int b = (k++) & 1;
         t[1-b].clear();
         for (int i = t[b].begin(); i < t[b].end(); i = t[b].next(i)) {
            t[2].mul(t[b], t[b].m(i), &t[b].at(i).f);
            normalize(t[3], t[2]);
            t[1-b].add(t[3]);
         }
         if (deg & (1<<max)) {
            t[b].mul(base);
            normalize(t[1 - b], t[b]);
         }
         //deg >>= 1;
      }
#elif MONOEVAL == 0
      bool hasDeg = true;
      for (int d = 0; hasDeg; ++d) {
         hasDeg = false;
         for (int v = 0; v < MP.nvars(); ++v) {
            if (degs[v] > d) {
               int b = (k++) & 1;
               t[b].mul(MP.single(v));
               normalize(t[1 - b], t[b]);
               hasDeg = true;
            }
         }
      }
#elif MONOEVAL == 1
      bool hasDeg = true;
      for (int d = 0; hasDeg; ++d) {
         hasDeg = false;
         Monomial v1 = MP.unit();
         for (int v = 0; v < MP.nvars(); ++v) {
            if (degs[v] > d) {
               v1 = MP.mul(v1, MP.single(v));
               hasDeg = true;
            }
         }
         if (hasDeg) {
            int b = (k++) & 1;
            t[b].mul(v1);
            normalize(t[1 - b], t[b]);
         }
      }
#elif MONOEVAL == 2
      for (int v = 0; v < MP.nvars(); ++v) {
         Monomial s = MP.single(v);
         for (int i = 0; i < degs[v]; ++i) {
            int b = (k++) & 1;
            DBG(printf("a: "); t[b].print(sout); printf("\n"));
            t[b].mul(s);
            DBG(printf("b: "); t[b].print(sout); printf("\n"));
            normalize(t[1 - b], t[b]);
         }
      }
#endif
      values[i].copy(t[k & 1]);
      DBG(printf("(%i): ", i); MP.print(sout, mm[i]); printf(" -> "); values[i].print(sout); printf("\n"));
   }

//   void promoteMonomial (Monomial a, Monomial b) {
//      const int* degs = MP.getDegs(mm[i]);
//      Polynomial t[2];
//      Cf c;
//      Ring::set(c, 1);
//      t[0].addTerm(MP.unit(), c);
//      int k = 0;
//      bool hasDeg = true;
//      for (int d = 0; hasDeg; ++d) {
//         hasDeg = false;
//         for (int v = 0; v < MP.nvars(); ++v) {
//            if (degs[v] > d) {
//               int b = (k++) & 1;
//               DBG(printf("a: "); t[b].print(sout); printf("\n"));
//               t[b].mul(MP.single(v));
//               DBG(printf("b: "); t[b].print(sout); printf("\n"));
//               normalize(t[1 - b], t[b]);
//               hasDeg = true;
//            }
//         }
//      }
//
//      values[i].copy(t[k & 1]);
//      DBG(printf("(%i): ", i); MP.print(sout, mm[i]); printf(" -> "); values[i].print(sout); printf("\n"));
//   }

   void init (const Array<Monomial>& mm, int total) {
      values.clear();
      values.resize(total);
      for (int i = 0; i < mm.size(); ++i)
         evaluateMonomial(mm, i);
   }
   void add (int id, int a, int b) {
      values[id].copy(values[a]);
      values[id].add(values[b]);
      //values[id].simplify();
      DBG(printf("(%i) = (%i) + (%i): ", id, a, b); values[id].print(sout); printf("\n"));
   }
   void mul (int id, int a, int b) {
      const Polynomial& pa = values[a], &pb = values[b];
      Polynomial t, s;
      Polynomial& r = values[id];
      for (int i = pb.begin(); i < pb.end(); i = pb.next(i)) {
         t.copy(pa);
         t.mul(pb.m(i));
         t.mulnum(pb.at(i).f);
         normalize(s, t);
         r.add(s);
      }
      //r.simplify();
      DBG(printf("(%i) = (%i) * (%i): ", id, a, b); values[id].print(sout); printf("\n"));
   }
   void mulnum (int id, int a, const Cf& num) {
      Polynomial& res = values[id];
      res.copy(values[a]);
      res.mulnum(num);
      DBG(printf("(%i) = (%i) * ", id, a);
         Ring::print(sout, num);
         printf(": ");
         values[id].print(sout);
         printf("\n"));
   }

   void yield (int id) {
      result.copy(values[id]);
      DBG(printf("yield (%i):", id);
         values[id].print(sout);
         printf("\n"));
   }

   struct NormTask {
      NormTask (Monomial a, int p) : m(a), parent(p), state(-1) {
      }
      Polynomial q, t;
      Monomial m;
      int state;
      int parent;
   };
   ObjArray<NormTask> stack;

   bool procnorm () {
//      static int ccc = 0;
//      ccc++;
//      printf("%i\n", ccc);
//      if (ccc == 4075)
//         ccc = 4075;
      NormTask& task = stack.top();
      DBG(printf ("%d(%d): ", stack.size() - 1, task.state);
         MP.print(sout, task.m);
         printf ("\n"));
      if (task.state < 0) {
         if (normalForms.find(task.m)) {
            task.q.copy(normalForms.at(task.m));
            DBG(printf("\tcached\n"));
            return true;
         }
         Cf c;
         Ring::set(c, 1);
         task.q.addTerm(task.m, c);
         if (!reduceStep(task.q)) {
            normalForms.insert(task.m).copy(task.q);
            DBG(printf ("\tirreducible\n"));
            return true;
         }
         DBG(printf ("\t"); 
            task.q.print(sout);
            printf ("\n"));
         task.state = task.q.begin();
      } else {
         task.state = task.q.next(task.state);
      }
      if (task.state >= task.q.end()) {
         //task.t.simplify();
         task.q.copy(task.t);
         normalForms.insert(task.m).copy(task.q);
         DBG(printf ("\tdone: ");
            task.q.print(sout);
            printf ("\n"));
         return true;
      }
      DBG(printf ("\t push: ");
         MP.print(sout, task.q.m(task.state));
         printf ("\n"));
      stack.push(task.q.m(task.state), stack.size() - 1);
      return false;
   }

   void normalize (Polynomial& res, Monomial m) {
      if (normalForms.find(m)) {
         res.copy(normalForms.at(m));
         return;
      }
      NormTask& task = stack.push(m, -1);
      while (stack.size() > 0) {
         int cc = 0;
         while (!procnorm())
            cc++;
         NormTask& cur = stack.top();
         if (cur.parent >= 0) {
            NormTask& par = stack[cur.parent];
            cur.q.mulnum(par.q.at(par.state).f);
            profTimerStart(add, "addition");
            par.t.add(cur.q);
            profTimerStop(add);
         } else {
            res.copy(stack[0].q);
         }
         stack.pop();         
      }

      DBG(printf("NF: ");
         MP.print(sout, m);
         printf(" -> ");
         res.print(sout);
         printf("\n"));
   }

   void normalize (Polynomial& res, const Polynomial& q) {
      Polynomial w;
      res.clear();
      for (int i = q.begin(); i < q.end(); i = q.next(i)) {
         normalize(w, q.m(i));
         w.mulnum(q.at(i).f);
         profTimerStart(add, "addition");
         res.add(w);
         profTimerStop(add);
      }
      //res.simplify();
   }

   const ObjArray<Polynomial>& basis;
   ObjArray<Polynomial> values;
   RedBlackObjMap<Monomial, Polynomial> normalForms;
   Polynomial result;
   SimpleReductor sr;
};

#endif /* __TNIRPS_POLY_REDUCTION_H__ */