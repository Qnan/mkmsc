#ifndef __TNIRPS_POLY_REDUCTION_H__
#define __TNIRPS_POLY_REDUCTION_H__

#include "tnirps_scheme.h"
#include "tnirps_monopool.h"
#include "tnirps_reduction.h"

//#define DBG(op) op
#undef DBG
#define DBG(op)

class Reductor {
public:
   Reductor (const ObjArray<Polynomial>& b) : basis(b), sr(b) {
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
         normalForms.value(i).p.print(sout);
         printf("  /  ");
         Ring::print(normalForms.value(i).d.get());
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
            Ring::div(cf, p.lc(), basis[i].lc());
            Ring::neg(cf, cf);
            t.mul(basis[i], MP.div(lm, m), &cf);
            p.add(t);
            p.simplify();
            return true;
         }
      }
      return false;
   }
   
   void evaluateMonomial (const Array<Monomial>& mm, int i) {
      const int* degs = MP.getDegs(mm[i]);
      Polynomial t[2];
      Cf c;
      Ring::set(c, 1);
      t[0].addTerm(MP.unit(), c);
      int k = 0;
      for (int v = 0; v < MP.nvars(); ++v) {
         Monomial single = MP.single(v);
         for (int j = 0; j < degs[v]; ++j) {
            int b = (k++) & 1;
            t[b].mul(single);
            normalize(t[1 - b], t[b]);
         }
      }
      
      values[i].copy(t[k&1]);
      DBG(printf("(%i): ", i); MP.print(sout, mm[i]); printf(" -> "); values[i].p.print(sout); printf("\t/"); Ring::print(values[i].d.get()); printf("\n"));
      // TODO: we should initialize these monomials inductively, too
      // TODO: think of alteration between two temporary polynomials on such occasions
      }

   void init (const Array<Monomial>& mm, int total) {
      values.clear();
      values.resize(total);
      for (int i = 0; i < mm.size(); ++i)
         evaluateMonomial(mm, i);
   }
   void add (int id, int a, int b) {
      values[id].copy(values[a]);
      values[id].add(values[b]);
      values[id].simplify();
      DBG(printf("(%i) = (%i) + (%i): ", id, a, b); 
         values[id].p.print(sout);
         printf("\t/");
         Ring::print(values[id].d.get());
         printf("\n"));
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
      r.simplify();
      DBG(printf("(%i) = (%i) * (%i): ", id, a, b);
         values[id].p.print(sout);
         printf("\t/");
         Ring::print(values[id].d.get());
         printf("\n"));
   }
   void mulnum (int id, int a, const Cf& num) {
      Polynomial& res = values[id];
      res.copy(values[a]);
      res.mulnum(num);
      DBG(printf("(%i) = (%i) * ", id, a);
         Ring::print(num.get());
         printf(": ");
         values[id].p.print(sout);
         printf("\t/");
         Ring::print(values[id].d.get());
         printf("\n"));
   }

   void yield (int id) {
      result.copy(values[id]);
      DBG(printf("yield (%i):", id);
         values[id].p.print(sout);
         printf("\t/");
         Ring::print(values[id].d.get());
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
            task.q.p.print(sout);
            printf ("\n"));
         task.state = task.q.begin();
      } else {
         task.state = task.q.next(task.state);
      }
      if (task.state >= task.q.end()) {
         task.t.simplify();
         task.q.copy(task.t);
         normalForms.insert(task.m).copy(task.q);
         DBG(printf ("\tdone: ");
            task.q.p.print(sout);
            printf ("\n"));
         return true;
      }
      DBG(printf ("\t push: ");
         MP.print(sout, task.q.p.m(task.state));
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
            par.t.add(cur.q);
         } else {
            res.copy(stack[0].q);
         }
         stack.pop();         
      }

      DBG(printf("NF: ");
         MP.print(sout, m);
         printf(" -> ");
         res.p.print(sout);
         printf("\t/");
         Ring::print(res.d.get());
         printf("\n"));
   }

   void normalize (Polynomial& res, const Polynomial& q) {
      Polynomial w;
      res.clear();
      for (int i = q.begin(); i < q.end(); i = q.next(i)) {
         const Polynomial::Term& term = q.at(i);
         normalize(w, term.m.get());
         w.mulnum(term.f);
         res.add(w);
      }
      res.simplify();
   }

   const ObjArray<Polynomial>& basis;
   ObjArray<Polynomial> values;
   RedBlackObjMap<Monomial, Polynomial> normalForms;
   Polynomial result;
   SimpleReductor sr;
};

#endif /* __TNIRPS_POLY_REDUCTION_H__ */