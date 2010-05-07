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
#include "tnirps_hashset.h"
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

void testMonome (void) {
   const char* name = "testMonome";

   MP.setOrder(MonoPool::LEX);
   Array<char> buf;
   ArrayOutput output(buf);
   Monomial m1, m2, m3, m4, m5;
   const int i1[] = {1,2,3,5}, d1[] = {3,5,7,11};
   const char* s1 = "[1]^3*[2]^5*[3]^7*[5]^11";
   const int h1 = 11564336; // hash

   const int i2[] = {2,3,4,6};
   const int d2[] = {21,22,23,24};
   
   const char* s3 = "[1]^3*[2]^26*[3]^29*[4]^23*[5]^11*[6]^24",
             * s4 = "[2]^21*[3]^22*[4]^23*[6]^24",
             * s5 = "[2]^5*[3]^7";
   m1 = MP.init(i1, d1, 4);
   output.clear();
   MP.print(output, m1);output.writeChar(0);
   if (strcmp(buf.ptr(), s1) != 0)
      throw Exception("%s: Monomial printing mismatch: %s != %s", name, buf.ptr(), s1);

   int h = MP.countHash(m1);
   if (h != h1)
      throw Exception("%s: Hash mismatch: %X != %X", name, h, h1);

   m2 = MP.init(i2, d2, 4);
   m3 = MP.mul(m1, m2);
   output.clear();
   MP.print(output, m3);output.writeChar(0);
   if (strcmp(buf.ptr(), s3) != 0)
      throw Exception("%s: Monomial multiplication error: %s != %s", name, buf.ptr(), s3);

   m4 = MP.div(m3, m1);
   output.clear();
   MP.print(output, m4);output.writeChar(0);
   if (strcmp(buf.ptr(), s4) != 0)
      throw Exception("%s: Monomial division error: %s != %s", name, buf.ptr(), s4);

   m5 = MP.gcd(m1, m2);
   output.clear();
   MP.print(output, m5);output.writeChar(0);
   if (strcmp(buf.ptr(), s5) != 0)
      throw Exception("%s: Monomial GCD calculation error: %s != %s", name, buf.ptr(), s5);

   MP.release(m1);
   MP.release(m2);
   MP.release(m3);
   MP.release(m4);
   MP.release(m5);
   printf("%s succeeded\n", name);
}

void testMonLoad (void) {
   // TODO: test monome loading from string, with different syntax
}

void testMonDiv ()
{
   const char* name = "testMonDiv";
   Monomial m[3];
   const int dd[3][2] = {{5, 7}, {6, 5}, {5, 5}};

   for (int i = 0; i < 3; ++i)
      m[i] = MP.init(dd[i], 2);

   if (!MP.divides(m[0],m[2]) || !MP.divides(m[1],m[2]) || MP.divides(m[0],m[1])
            || MP.divides(m[1],m[0])  || MP.divides(m[2],m[0])  || MP.divides(m[2],m[1]))
      throw Exception("%s: Divisibility check failed", name);

   for (int i = 0; i < 3; ++i)
      MP.release(m[i]);
   printf("%s succeeded\n", name);
}

void testPoly (void)
{
   Polynomial p;
   p.init("4x2yz+13xy2+10xy-71x+z3+y2z", 0, 0, "xyz");
   p.print(sout);

   // test
   //    initialization
   //    printing
   //    sorting
   //    addition
   //    multiplication
   //    copy
}

void testPolyLoad (void) {
   // test
   //    initialization
}

void testReduce (void) {
   // TODO: test simple reduction
//   Polynomial g[4],p;
//   //const char* pp[] = {"y^3+x*y", "y^2*x+x^2"};
//   g[0].init("y^3+x*y", 0, 0, vv);
//   g[1].init("y^2*x+x^2", 0, 0, vv);
//   g[2].init("x^2*y-2*y^2+x", 0, 0, vv);
//   g[3].init("x^3-3*x*y", 0, 0, vv);
//   g[0].sort();
//   g[1].sort();
//   g[2].sort();
//   g[3].sort();
//
//   p.init("x^3-2*y^2*x^2+7*y^5", 0, 0, vv);
//   p.sort();
//
//   reduce(g, NELEM(g), p);
//
}

//void testGorner (void)
//{
//   Polynomial p;
//
//   p.init("x2y+13xy2+xy+x+y3+y2", 0, 0, "xyz");
//   p.sort();
//
//   p.print(),printf("\n");
//   Scheme scheme;
//   SchemeGorner gorner(scheme, p);
//   gorner.build();
//   printf("\n\n");
//   Printer prn;
//   prn.evaluate(scheme);
//   printf("\n");
//   Printer prn1;
//   prn1.evaluate(scheme);
//   printf("\n");
//   Evaluator eval;
//   Array<int> values;
//   values.push(3);
//   values.push(-1);
//   eval.evaluate(scheme, values);
//}

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

int eq_str (int a, int b, void* context) {
   const char** ss = (const char**)context;
   return strcmp(ss[a], ss[b]);
}

void testHashSet() {
   const char* ss[] = {"alpha", "beta", "aqua", "baum", "astra", "aqua", "alpha", "aqua", "astra", "baum", "beta"};

   HashSet set;
   set.eq = eq_str;
   set.context = ss;

   for (int i = 0; i < NELEM(ss); ++i)
      printf("%s --- %d\n", ss[i], set.findOrAdd(i, (int)ss[i][0]));
}

int main (int argc, const char** argv)
{
   MP.checkLeaks(true);
   MP.setOrder(MonoPool::LEX);
   try {
      //testMonome();
      //testMonDiv();
      //testPoly();
   } catch (Exception ex) {
      printf("Error: %s", ex.message());
   }
//   testMonSort();
//   ;
//   testListSort();
//   testPolyPrint();
//   testPolyMul();
//   testPolySum();
//   testReduce();
//   testGorner();
//   testGMP();
//   testScript();
   
   MP.checkLeaks(true);
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