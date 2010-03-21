#include <stdio.h>
#include "array.h"
#include "pool.h"
#include "list.h"
#include "tnirps_monomial.h"
#include "tnirps_polynomial.h"

const char* varName (int idx) {
   static const char* vars[] = {"a", "b", "c", "d", "e", "f", "g", "h", "k", "m", "n", "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z"};
   if (idx < 0 || idx >= NELEM(vars))
      throw Exception("Variable index %i out of range!", idx);
   return vars[idx];
}

void testMonome (void)
{
   Monomial m1, m2, m3, m4, m5;
   const int i1[] = {1,2,3,5};
   const int d1[] = {3,5,7,11};
   const int i2[] = {2,3,4,6};
   const int d2[] = {21,22,23,24};
   //1:3, 2:26, 3:29, 4:23, 5:11, 6:24
   m1 = MP.init(i1, d1, 4);
   MP.print(m1);
   printf("\n%i\n", MP.countHash(m1));
   m2 = MP.init(i2, d2, 4);
   MP.print(m2);
   printf("\n%i\n", MP.countHash(m2));
   m3 = MP.mul(m1, m2);
   MP.print(m3);
   printf("\n%i\n", MP.countHash(m3));
   m4 = MP.div(m3, m1);
   MP.print(m4);
   printf("\n%i\n", MP.countHash(m4));
   m5 = MP.gcd(m1, m2);
   MP.print(m5);
   printf("\n%i\n", MP.countHash(m5));
   MP.release(m1);
   MP.release(m2);
   MP.release(m3);
   MP.release(m4);
   MP.release(m5);
   scanf("*");
}

void testMonSort (void)
{
   Monomial mm[10];
   int order[10];
   int cnt = 0;
   const int id[5][3] = {{2, 1, 0},
                        {1, 1, 1},
                        {1, 0, 1},
                        {0, 5, 0},
                        {1, 2, 0}};
   cnt = NELEM(id);
   for (int i = 0; i < cnt; ++i)
      mm[i] = MP.init(id[i], NELEM(id[i]));
   for (int i = 0; i < cnt; ++i)
      order[i] = i;
   printf("monomes: ");
   for (int i = 0; i < cnt; ++i) {
      if (i > 0) printf(", ");
      MP.print(mm[i]);
   }
   printf("\n");
   MP.setOrder(MonoPool::LEX);
   for (int i = 0, t; i < cnt; ++i) {
      for (int j = i + 1; j < cnt; ++j) {
         if (MP.cmp(mm[order[i]], mm[order[j]]) > 0) {
            t = order[i];
            order[i] = order[j];
            order[j] = t;
         }
      }
   }
   printf("lex: ");
   for (int i = 0; i < cnt; ++i) {
      if (i > 0) printf(", ");
      MP.print(mm[order[i]]);
   }
   printf("\n");
   MP.setOrder(MonoPool::DRL);
   for (int i = 0, t; i < cnt; ++i) {
      for (int j = i + 1; j < cnt; ++j) {
         if (MP.cmp(mm[order[i]], mm[order[j]]) > 0) {
            t = order[i];
            order[i] = order[j];
            order[j] = t;
         }
      }
   }
   printf("degRevLex: ");
   for (int i = 0; i < cnt; ++i) {
      if (i > 0) printf(", ");
      MP.print(mm[order[i]]);
   }

   for (int i = 0; i < cnt; ++i)
      MP.release(mm[i]);

   printf("\n");

   scanf("*");
}

void testMonDiv ()
{
   Monomial m1, m2, m3, m4;
   const int d1[] = {0,21};
   const int d2[] = {0,21,22,23,0,24};
   const int d3[] = {0,21,21,23,0,24};
   const int d4[] = {0,21,22,23,0,24};
   //1:3, 2:26, 3:29, 4:23, 5:11, 6:24
   m1 = MP.init(d1, NELEM(d1));
   MP.print(m1);
   printf("\n");
   m2 = MP.init(d2, NELEM(d2));
   MP.print(m2);
   printf("\n");
   m3 = MP.init(d3, NELEM(d3));
   MP.print(m3);
   printf("\n");
   m4 = MP.init(d4, NELEM(d4));
   MP.print(m4);
   printf("\n");

   printf("%s\n", MP.equals(m1,m2) ? "yes" : "no"); // no
   printf("%s\n", MP.equals(m2,m1) ? "yes" : "no"); // no
   printf("%s\n", MP.equals(m2,m3) ? "yes" : "no"); // no
   printf("%s\n", MP.equals(m2,m4) ? "yes" : "no"); // yes

   printf("%s\n", MP.divides(m1,m2) ? "yes" : "no"); // no
   printf("%s\n", MP.divides(m2,m1) ? "yes" : "no"); // yes
   printf("%s\n", MP.divides(m2,m3) ? "yes" : "no"); // yes
   printf("%s\n", MP.divides(m3,m2) ? "yes" : "no"); // no
   printf("%s\n", MP.divides(m2,m4) ? "yes" : "no"); // yes
   MP.release(m1);
   MP.release(m2);
   MP.release(m3);
   MP.release(m4);
   scanf("*");
}

void testListSort ()
{
   const int nn[] = {0,5,8,4,1,2,7,6,3};
   List<int> list;
   for (int i = 0; i < NELEM(nn); ++i)
      list.add(nn[i]);

   for (int i = list.begin(); i < list.end(); i = list.next(i))
      printf(" %i", list.at(i));
   printf("\n");

   for (int i = 0; i < list.size(); ++i)
      for (int j = list.begin(), k = i + 1; k < list.size(); ++k)
         if (list.at(j) > list.at(list.next(j)))
            list.swap(j);
         else
            j = list.next(j);

   for (int i = list.begin(); i < list.end(); i = list.next(i))
      printf(" %i", list.at(i));
   printf("\n");

   scanf("*");
}

void testPolyPrint (void)
{
   Monomial m;
   int cnt = 0;
   const char*const vars[] = {"x", "y", "z"};
   const int id[5][3] = {{2, 1, 0},
                        {1, 1, 1},
                        {1, 0, 1},
                        {0, 5, 0},
                        {1, 2, 0}};
   const int cf[5] = {77, 4, 35, 36, 8};
   cnt = NELEM(id);
   Polynomial p;
   for (int i = 0; i < cnt; ++i) {
      m = MP.init(id[i], NELEM(id[i]));
      p.addTerm(m, cf[i]);
      MP.release(m);
   }
   p.print();

   scanf("*");
}

void testPolyMul (void)
{
   Monomial m;
   int cnt = 0;
   const int id[5][3] = {{2, 1, 0},
                        {1, 1, 1},
                        {1, 0, 1},
                        {0, 5, 0},
                        {1, 2, 0}};
   const int id2[] = {1, 0, 0, 1};
   const int cf[5] = {77, 4, 35, 36, 8};
   cnt = NELEM(id);
   Polynomial p, r;
   for (int i = 0; i < cnt; ++i) {
      m = MP.init(id[i], NELEM(id[i]));
      p.addTerm(m, cf[i]);
      MP.release(m);
   }
   p.print(); printf("\n");
   p.sort();
   p.print(); printf("\n");
   m = MP.init(id2, NELEM(id2));
   MP.print(m); printf("\n");
   Polynomial::mul(r, p, m);
   r.print(); printf("\n");
   MP.release(m);

   scanf("*");
}

void initTestPoly (Polynomial& p, const int* id, const int* cf, int nvars, int nterms)
{
   static Monomial m;
   p.clear();
   for (int i = 0; i < nterms; ++i) {
      m = MP.init(id + i * nvars, nvars);
      p.addTerm(m, cf[i]);
      MP.release(m);
   }
   p.sort();
}

void testPolySum (void)
{
   const int id[] = {
      2, 1, 0,
      1, 1, 1,
      1, 0, 1,
      0, 5, 0,
      1, 2, 0};
   const int id2[] = {
      1, 0, 0,
      1, 1, 1,
      1, 2, 0,
      1, 2, 1};
   const int cf[] = {
      77, 4, 35, 36, 8};
   const int cf2[] = {
      10, 4, 4, 6};

   Polynomial p1, p2, r;
   initTestPoly(p1, id, cf, 3, NELEM(id) / 3);
   initTestPoly(p2, id2, cf2, 3, 4);
   p1.print(); printf("\n");
   p2.print(); printf("\n");
   r.copy(p1);
   Polynomial::add(r, p2, -1);
   r.print(); printf("\n");       
   r.copy(p1);
   Polynomial::add(r, p2, -2);
   r.print(); printf("\n");       

   scanf("*");
}

int main (void)
{
   MP.setOrder(MonoPool::LEX);
   MP.varName = varName;
   // TODO: reduction, etc.
   //testPolySum();
   //testPolyMul();
   //testPolyPrint();
   //testListSort();
   //testMonome();
   //testMonSort();
   //testMonDiv();
   return 0;
}