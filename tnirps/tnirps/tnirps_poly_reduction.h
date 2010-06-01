#ifndef __TNIRPS_POLY_REDUCTION_H__
#define __TNIRPS_POLY_REDUCTION_H__

#include "tnirps_scheme.h"
#include "tnirps_monopool.h"
#include "tnirps_reduction.h"

//#define DBG(op) op
#undef DBG
#define DBG(op)

class Reductor {
   struct Item {
      Item () {
         d.set(NP.init(1));
      }
      void copy (const Item& other) {
         p.copy(other.p);
         d.set(other.d.get());
      }
      void add (const Item& other) {
         NumPtr d1(d.get());
         NumPtr d2(other.d.get());
         NumPtr gcd(NP.gcd(d1.get(), d2.get()));
         // if nontrivial, divide them over it
         if (NP.cmp(gcd.get(), 1) > 0) {
            d1.set(NP.div(d1.get(), gcd.get()));
            d2.set(NP.div(d2.get(), gcd.get()));            
         }
         p.mulnum(d2);
         p.append(other.p, d1);
         d.set(NP.mul(d.get(), d2.get()));
      }
      void mulnum (const NumPtr& f) {
         NumPtr g(f.get());
         NumPtr gcd(NP.gcd(d.get(), g.get()));
         if (NP.cmp(gcd.get(), 1) > 0) {
            g.set(NP.div(g.get(), gcd.get()));
            d.set(NP.div(d.get(), gcd.get()));
         }
         p.mulnum(g);
      }
      void mul (Monomial m) {
         p.mul(m);
      }
      void simplify () {
         p.simplify();

         // find the GCD of all the coefficients and the denomiator
         NumPtr gcd(d.get());
         int j = 1;
         for (int i = p.begin(); i < p.end(); i = p.next(i)) {
            gcd.set(NP.gcd(gcd.get(), p.at(i).f.get()));
         }
         // if nontrivial, divide them over it
         if (NP.cmp(gcd.get(), 1) > 0) {
            for (int i = p.begin(); i < p.end(); i = p.next(i)) {
               NumPtr& c = p.at(i).f;
               c.set(NP.div(c.get(), gcd.get()));
            }
            d.set(NP.div(d.get(), gcd.get()));
         }
      }
      Polynomial p;
      NumPtr d; // denominator
   private:
      Item (const Item&);
   };

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
         NP.print(normalForms.value(i).d.get());
         printf("\n");
      })
   }

   SCHEME_CALLBACKS_DEFINE(Reductor);

private:
   bool reduceStep (Item& p) {
      Item t;
      Monomial lm = p.p.lm();
      int i;
      for (i = 0; i < basis.size(); ++i) {
         Monomial m = basis[i].lm();
         if (MP.divides(lm, m)) {
            t.p.mul(basis[i], MP.div(lm, m));
            t.d.set(t.p.lc().get());
            t.mulnum(NumPtr(NP.neg(p.p.lc().get())));
            p.add(t);
            p.simplify();
            return true;
         }
      }
      return false;
   }
   
   void evaluateMonomial (const Array<Monomial>& mm, int i) {
      const int* degs = MP.getDegs(mm[i]);
      Item t[2];
      t[0].p.addTerm(MP.unit(), NumPtr(NP.init(1)));
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
      DBG(printf("(%i): ", i); MP.print(sout, mm[i]); printf(" -> "); values[i].p.print(sout); printf("\t/"); NP.print(values[i].d.get()); printf("\n"));
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
         NP.print(values[id].d.get());
         printf("\n"));
   }
   void mul (int id, int a, int b) {
      const Item& pa = values[a], &pb = values[b];
      Item t, s;
      Item& r = values[id];
      for (int i = pb.p.begin(); i < pb.p.end(); i = pb.p.next(i)) {
         t.copy(pa);
         t.mul(pb.p.m(i));
         t.mulnum(pb.p.at(i).f);
         normalize(s, t);
         r.add(s);
      }
      r.simplify();
      DBG(printf("(%i) = (%i) * (%i): ", id, a, b);
         values[id].p.print(sout);
         printf("\t/");
         NP.print(values[id].d.get());
         printf("\n"));
   }
   void mulnum (int id, int a, const NumPtr& num) {
      Item& res = values[id];
      res.copy(values[a]);
      res.mulnum(num);
      DBG(printf("(%i) = (%i) * ", id, a);
         NP.print(num.get());
         printf(": ");
         values[id].p.print(sout);
         printf("\t/");
         NP.print(values[id].d.get());
         printf("\n"));
   }

   void yield (int id) {
      result.copy(values[id].p);
      resDenomiator.set(values[id].d.get());
      DBG(printf("yield (%i):", id);
         values[id].p.print(sout);
         printf("\t/");
         NP.print(values[id].d.get());
         printf("\n"));
   }

   struct NormTask {
      NormTask (Monomial a, int p) : m(a), parent(p), state(-1) {
      }
      Item q, t;
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
         task.q.p.addTerm(task.m, NumPtr(NP.init(1)));
         if (!reduceStep(task.q)) {
            normalForms.insert(task.m).copy(task.q);
            DBG(printf ("\tirreducible\n"));
            return true;
         }
         DBG(printf ("\t"); 
            task.q.p.print(sout);
            printf ("\n"));
         task.state = task.q.p.begin();
      } else {
         task.state = task.q.p.next(task.state);
      }
      if (task.state >= task.q.p.end()) {
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
      stack.push(task.q.p.m(task.state), stack.size() - 1);
      return false;
   }

   void normalize (Item& res, Monomial m) {
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
            cur.q.mulnum(par.q.p.at(par.state).f);
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
         NP.print(res.d.get());
         printf("\n"));
   }

   void normalize (Item& res, const Item& q) {
      Item w;
      res.p.clear();
      res.d.set(NP.init(1));
      for (int i = q.p.begin(); i < q.p.end(); i = q.p.next(i)) {
         const Polynomial::Term& term = q.p.at(i);
         normalize(w, term.m.get());
         w.mulnum(term.f);
         res.add(w);
      }
      res.simplify();
   }

   const ObjArray<Polynomial>& basis;
   ObjArray<Item> values;
   RedBlackObjMap<Monomial, Item> normalForms;
   Polynomial result;
   NumPtr resDenomiator;
   SimpleReductor sr;
};

#endif /* __TNIRPS_POLY_REDUCTION_H__ */