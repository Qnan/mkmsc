#ifndef __TNIRPS_SCHEME_MST_H__
#define __TNIRPS_SCHEME_MST_H__

#include "array.h"
#include "red_black.h"
#include "tnirps_scheme.h"
#include "tnirps_monopool.h"
#include "tnirps_polynomial.h"
class Exception;

class SchemeMst {
public:
   SchemeMst (Scheme& scheme, const Polynomial& poly) : _poly(poly), _scheme(scheme) {}

   void build () {
      monomialCounter = 0;
      sets.clear();
      edges.clear();
      mst.clear();
      monomialMap.clear();
      intermediateCounter = 0;
      try {
         makeDivisionGraph();
         makeMst();
         setOps();
      } catch (Exception& ex) {
         printf("%s\n", ex.message());
      }
   }

private:
   struct Edge {
      void set(int b, int e, int w, Monomial m) {
         beg = b; end = e; weight = w; f = m;
      }
      int beg, end, weight;
      Monomial f;
   };
   static int edge_cmp_w(const Edge& a, const Edge& b, const void* context) { return a.weight - b.weight; }

   void makeDivisionGraph () {
      const Polynomial& p = _poly;
      edges.reserve(p.size() * 3);
      for (int i = 0; i < p.size(); ++i) {
         for (int j = i+1; j < p.size(); ++j) {
            Monomial mi = p.at(i).m, mj = p.at(j).m, m;
            if (MP.divides(mi, mj)) {
               m = MP.div(mi, mj);
               edges.push().set(i, j, MP.maxDeg(m), m); // XXX: check the weight
            } else if (MP.divides(mj, mi)) {
               m = MP.div(mj, mi);
               edges.push().set(j, i, MP.maxDeg(m), m);
            }
         }
      }
      edges.qsort(edge_cmp_w, 0);
//      printf("\n==============\n");
//      for (int i = 0; i < edges.size(); ++i)
//         printf("%i, %i, %i\n", edges[i].beg, edges[i].end, edges[i].weight);
//      printf("==============\n");
   }

   struct Set {
      int label, size;
   };
   void initSets () {
      sets.clear();
      sets.resize(_poly.size());
      for (int i = 0; i < _poly.size(); ++i) {
         sets[i].label = i;
         sets[i].size = 1;
      }
   }
   int getSet (int i) {
      int c;
      // follow the route
      for (c = i; sets[c].label != c; c = sets[c].label)
         ;
      // propagate the label to all nodes on the route
      for (int j = i, k = 0; j != c; ) {
         k = sets[j].label;
         sets[j].label = c;
         j = k;
      }
      return c;
   }
   void mergeSets (int i, int j) {
      if (sets[i].size > sets[j].size) {
         sets[i].size += sets[j].size;
         sets[j].label = i;
      } else {
         sets[j].size += sets[i].size;
         sets[i].label = j;
      }
   }
   void printSets () {
      for (int i = 0; i < _poly.size(); ++i)
         printf(" %4i", getSet(i));
      printf("\n");
      for (int i = 0; i < _poly.size(); ++i)
         printf(" %4i", sets[getSet(i)].size);
      printf("\n");
   }
   void makeMst () {
      initSets();
      mst.clear();
      mst.reserve(_poly.size() - 1);
//      printSets();
      for (int i = 0; i < edges.size(); ++i) {
         const Edge& e = edges[i];
         int sb = getSet(e.beg), se = getSet(e.end);
         if (sb != se) {
            mst.push(i);
            mergeSets(se, sb);
//            printf("add %i %i\n", e.beg, e.end);
         } else {
//            printf("skip %i %i\n", e.beg, e.end);
         }
//         printSets();
      }
   }

   static int mst_cmp_e(const int& a, const int& b, const void* context) {
      Array<Edge>* edges = (Array<Edge>*)context;
      return edges->at(b).end - edges->at(a).end;
   }

   int addMonomial (const Monomial& m1) {
      Monomial m = MP.clone(m1);
      int r;
      if (monomialMap.find(m)) {
         r = monomialMap.at(m);
         MP.release(m);
      } else {
         r = monomialCounter++;
         monomialMap.insert(m, r);
      }
      return r;
   }

   void setOps () {
      // sort edges by their end node
      mst.qsort(mst_cmp_e, &edges);
      printf("\n==============\n");
      for (int i = 0; i < mst.size(); ++i) {
         const Edge& e = edges[mst[i]];
         printf("%i, %i, %i\n", e.beg, e.end, e.weight);
      }
      printf("==============\n");

      int n = _poly.size();
      Array<int> map;
      map.clear();map.resize(n);map.zerofill();
      for (int i = 0; i < mst.size(); ++i)
         map[edges[mst[i]].beg]++;

      // store monomials
      for (int i = 0; i < n; ++i)
         if (map[i] == 0)
            map[i] = addMonomial(_poly.at(i).m);
         else
            map[i] = -1;
      Array<int> mstMap;
      mstMap.clear();mstMap.resize(mst.size());
      for (int i = 0; i < mst.size(); ++i)
         mstMap[i] = addMonomial(edges[mst[i]].f);
      intermediateCounter = monomialCounter;
      Array<Scheme::Op>& ops = _scheme.ops;
      for (int i = 0; i < mst.size(); ++i) {
         const Edge& e = edges[mst[i]];
         int r = intermediateCounter++;
         ops.push().init(Scheme::OP_MUL, r, map[e.end], mstMap[i]);
         map[e.beg] = r;
      }
      for (int i = 0; i < n; ++i) {
         CFTYPE c = _poly.at(i).f;
         if (c != 1) {
            int r = intermediateCounter++;
            ops.push().init(Scheme::OP_MULNUM, r, map[i], c);
            map[i] = r;
         }
      }

      int r0 = map[0];
      for (int i = 1; i < n; ++i) {
         int r = intermediateCounter++;
         ops.push().init(Scheme::OP_ADD, r, r0, map[i]);
         r0 = r;
      }
      _scheme.resultId = r0;
      _scheme.totalCount = r0 + 1;
      Array<Monomial>& initial = _scheme.monomials;
      initial.clear();
      initial.resize(monomialCounter);
      for (int i = monomialMap.begin(); i < monomialMap.end(); i = monomialMap.next(i)) {
         initial[monomialMap.value(i)] = monomialMap.key(i);
      }

   }

private:
   Array<Set> sets;
   Array<Edge> edges;
   Array<int> mst;
   RedBlackMap<Monomial, int> monomialMap;
   int monomialCounter;
   int intermediateCounter;

   const Polynomial& _poly;
   Scheme& _scheme;

   SchemeMst (const SchemeMst&); // no implicit copy
};

#endif //__TNIRPS_SCHEME_MST_H__
