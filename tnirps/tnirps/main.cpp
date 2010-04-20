#include <stdio.h>
#include "array.h"
#include "pool.h"
#include "list.h"
#include "tnirps_monomial.h"
#include "tnirps_polynomial.h"
#include "tnirps_reduction.h"
#include "tnirps_scheme_gorner.h"
#include "tnirps_scheme_simple.h"
#include "tnirps_scheme_mst.h"
#include "tnirps_bigint.h"
#include "tnirps_poly_evaluator.h"
#include "tnirps_poly_evaluator_i.h"
#include "tnirps_poly_printer.h"
#include "tnirps_script.h"
#include "scanner.h"

#include <gmp.h>

const char* varName (int idx) {
   static const char* vars[] = {"a", "b", "c", "d", "e", "f", "g", "h", "k", "m", "n", "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z"};
   if (idx < 0 || idx >= NELEM(vars))
      throw Exception("Variable index %i out of range!", idx);
   return vars[idx];
}

const char* varNameXYZ (int idx) {
   static const char* vars[] = {"x", "y", "z"};
   if (idx < 0 || idx >= NELEM(vars))
      throw Exception("Variable index %i out of range!", idx);
   return vars[idx];
}

const char* vv = "abcdefghkmnpqrstuvwxyz";

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
}

void testInitFromString (void)
{
   const char* vv = "abcdefghkmnpqrstuvwxyz";
   Monomial m;
   m = MP.init("a5", 0, 0, vv);
   MP.print(m);
   MP.release(m);
   printf("\n");

   m = MP.init("b4", 0, 0, vv);
   MP.print(m);
   MP.release(m);
   printf("\n");

   m = MP.init("a71 b 13", 0, 0, vv);
   MP.print(m);
   MP.release(m);
   printf("\n");

   m = MP.init("c^5*d*e^2", 0, 0, vv);
   MP.print(m);
   MP.release(m);
   printf("\n");

   m = MP.init("aaa*abc", 2, 0, vv);
   MP.print(m);
   MP.release(m);
   printf("\n");

   Polynomial g[3];
   g[0].init("x^5+3*x^3*y^2+7*y^3", 0, 0, vv);
   g[1].init("x^2*y^2+x^8", 0, 0, vv);
   g[2].init("x^4*y+x*y", 0, 0, vv);

   g[0].print(); printf("\n");
   g[1].print(); printf("\n");
   g[2].print(); printf("\n");
}

void testReduce (void)
{
   Polynomial g[4],p;
   //const char* pp[] = {"y^3+x*y", "y^2*x+x^2"};
   g[0].init("y^3+x*y", 0, 0, vv);
   g[1].init("y^2*x+x^2", 0, 0, vv);
   g[2].init("x^2*y-2*y^2+x", 0, 0, vv);
   g[3].init("x^3-3*x*y", 0, 0, vv);
   g[0].sort();
   g[1].sort();
   g[2].sort();
   g[3].sort();
   
   p.init("x^3-2*y^2*x^2+7*y^5", 0, 0, vv);
   p.sort();

   reduce(g, NELEM(g), p);

}

void testGorner (void)
{
   Polynomial p;

   p.init("x2y+13xy2+xy+x+y3+y2", 0, 0, "xyz");
   p.sort();

   p.print(),printf("\n");
   Scheme scheme;
   SchemeGorner gorner(scheme, p);
   gorner.build();
   printf("\n\n");
   Printer prn;
   prn.evaluate(scheme);
   printf("\n");
   Printer prn1;
   prn1.evaluate(scheme);
   printf("\n");
   Evaluator eval;
   Array<int> values;
   values.push(3);
   values.push(-1);
   eval.evaluate(scheme, values);
}

void testGMP() {
   bigint_t a, b, c, d;
   BI::init(a),BI::init(b),BI::init(c),BI::init(d);
   BI::set(a, 19283); BI::set(b, 723890); BI::mul(c, a, b);
   gmp_printf("%Zd * %Zd = %Zd\n", a, b, c);
   BI::set(a, 232111124); BI::set(b, 342123); BI::mul(c, a, b);
   gmp_printf("%Zd * %Zd = %Zd\n", a, b, c);
   BI::set(a, "1298371928379482364826482364"); BI::set(b, "123812389162"); BI::add(c, a, b);
   gmp_printf("%Zd + %Zd = %Zd\n", a, b, c);
   BI::pow(c, 123, 4);
   gmp_printf("123 ^ 4 = %Zd\n", c);
   BI::clear(a),BI::clear(b),BI::clear(c),BI::clear(d);
}

void testScript() {
   ScriptInterpreter interpreter;
   const char* input = "vars xyz             \n\
      set p1 x3yz+x2y+13xy2+67xyz+x+11y3+26y2\n\
      build sg p1 gorner                     \n\
      build ss p1 simple                     \n\
      build sm p1 mst                        \n\
      eval sg 23,-112,7                      \n\
      eval ss 23,-112,7                      \n\
      eval sm 23,-112,7                      \n\
   ";
   BufferScanner scanner(input);
   interpreter.execute(scanner);
}

int main (int argc, const char** argv)
{
   MP.setOrder(MonoPool::LEX);
//   script(argc, argv);

//   MP.varName = varNameXYZ;
   //testReduce();
//   testGorner();
   testScript();
//   testGMP();
   
   //testPolySum();
   //testPolyMul();
   //testPolyPrint();
   //testListSort();
   //testMonome();
   //testMonSort();
   //testMonDiv();
   return 0;
}

int script (int argc, const char** argv)
{
   if (argc == 1) {
      printf("Please pass input file path as a parameter.\n");
      printf("Script file example is included in the distribution.\n");
   } else {
      ScriptInterpreter interpreter;

      FileScanner scanner(argv[1]);
      interpreter.execute(scanner);
   }
}