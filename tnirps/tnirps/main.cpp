#include <stdio.h>
#include "array.h"
#include "obj_array.h"
#include "pool.h"
#include "list.h"
#include "tnirps_monomial.h"
#include "tnirps_polynomial.h"
#include "tnirps_reduction.h"
#include "tnirps_scheme_gorner.h"
#include "tnirps_scheme_simple.h"
#include "tnirps_scheme_tree.h"
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

#define TEST_PRE(name) \
   const char* _name = name; \
   MP.setOrder(MonoPool::LEX); \
   Array<char> buf

#define TEST_POST() \
   printf("%s succeeded\n", _name)

#define CHECK_MATCH(str) \
   if (strcmp(buf.ptr(), str) != 0) \
      throw Exception("%s: Error:\n\t%s\n\t!=\n\t%s\n", _name, buf.ptr(), str)

void testMonome (void) {
   TEST_PRE("testMonome");

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
   MP.toStr(buf, m1);
   if (strcmp(buf.ptr(), s1) != 0)
      throw Exception("%s: Monomial printing mismatch: %s != %s", _name, buf.ptr(), s1);

   int h = MP.countHash(m1);
   if (h != h1)
      throw Exception("%s: Hash mismatch: %X != %X", _name, h, h1);

   m2 = MP.init(i2, d2, 4);
   m3 = MP.mul(m1, m2);
   MP.toStr(buf, m3);
   if (strcmp(buf.ptr(), s3) != 0)
      throw Exception("%s: Monomial multiplication error: %s != %s", _name, buf.ptr(), s3);

   m4 = MP.div(m3, m1);
   MP.toStr(buf, m4);
   if (strcmp(buf.ptr(), s4) != 0)
      throw Exception("%s: Monomial division error: %s != %s", _name, buf.ptr(), s4);

   m5 = MP.gcd(m1, m2);
   MP.toStr(buf, m5);
   if (strcmp(buf.ptr(), s5) != 0)
      throw Exception("%s: Monomial GCD calculation error: %s != %s", _name, buf.ptr(), s5);

   TEST_POST();
}

void testMonLoad (void) {
   // TODO: test monome loading from string, with different syntax
}

void testMonDiv ()
{
   TEST_PRE("testMonDiv");
   Monomial m[3];
   const int dd[3][2] = {{5, 7}, {6, 5}, {5, 5}};

   for (int i = 0; i < 3; ++i)
      m[i] = MP.init(dd[i], 2);

   if (!MP.divides(m[0],m[2]) || !MP.divides(m[1],m[2]) || MP.divides(m[0],m[1])
            || MP.divides(m[1],m[0])  || MP.divides(m[2],m[0])  || MP.divides(m[2],m[1]))
      throw Exception("%s: Divisibility check failed", _name);

   TEST_POST();
}

void testPolyInit (void)
{
   TEST_PRE("testPolyInit");
   MP.varName = varNameXYZ;
   Polynomial p;
   const char *s = "13xy2+z3-71x+10xy+4x2yz-y+y2z";
   p.init(s, 0, 0, "xyz");
   p.toStr(buf);
   CHECK_MATCH("13 x*y^2 + z^3 - 71 x + 10 x*y + 4 x^2*y*z - y + y^2*z");
   TEST_POST();
}

void testPolySort (void)
{
   TEST_PRE("testPolySort");
   MP.varName = varNameXYZ;
   Polynomial p;
   const char *s = "13xy2+z3-71x+10xy+4x2yz-y+y2z";
   p.init(s, 0, 0, "xyz");
   p.sort();
   p.toStr(buf);
   CHECK_MATCH("4 x^2*y*z + 13 x*y^2 + 10 x*y - 71 x + y^2*z - y + z^3");
   TEST_POST();
}

void testPolyAdd (void)
{
   TEST_PRE("testPolyAdd");
   MP.varName = varNameXYZ;
   Polynomial p, p1;
   const char *s1 = "13xy2+z3-71x+10xy+4x2yz-y+y2z";
   p.init(s1, 0, 0, "xyz");
   p.sort();
   const char *s2 = "5 x - x*y^2 + 17 z^3 - 10 z^2 - 10 x*y + 5 y - 1 + x*y*z";
   p1.init(s2, 0,0,"xyz");
   p1.sort();

   Polynomial::add(p, p1);
   p.toStr(buf);
   CHECK_MATCH("4 x^2*y*z + 12 x*y^2 + x*y*z - 66 x + y^2*z + 4 y + 18 z^3 - 10 z^2 - 1");
   TEST_POST();
}

void testPolyAdd2 (void)
{
   TEST_PRE("testPolyAdd2");
   MP.varName = varNameXYZ;
   Polynomial p, p1;
   const char *s1 = "13xy2+z3-71x+10xy+4x2yz-y+y2z";
   p.init(s1, 0, 0, "xyz");
   p.sort();
   const char *s2 = "10 z^2 - x*y + 7";
   p1.init(s2, 0,0,"xyz");
   p1.sort();

   Polynomial::add(p, p1, 3, -6);
   p.toStr(buf);
   CHECK_MATCH("12 x^2*y*z + 39 x*y^2 + 36 x*y - 213 x + 3 y^2*z - 3 y + 3 z^3 - 60 z^2 - 42");
   TEST_POST();
}

void testPolyMul (void)
{
   TEST_PRE("testPolyMul");
   MP.varName = varNameXYZ;
   Polynomial p;
   const char *s1 = "13xy2+z3-71x+10xy+4x2yz-y+y2z";
   p.init(s1, 0, 0, "xyz");
   p.sort();
   const char *s2 = "z^2";
   Monomial m = MP.init(s2, 0, 0, "xyz");

   p.mul(m);
   p.mulnum(-2);
   p.toStr(buf);
   CHECK_MATCH("-8 x^2*y*z^3 - 26 x*y^2*z^2 - 20 x*y*z^2 + 142 x*z^2 - 2 y^2*z^3 + 2 y*z^2 - 2 z^5");
   TEST_POST();
}

void testPolyLoad (void) {
   // test
   //    initialization
}

void testReduce (void) {
   TEST_PRE("testReduce");
   MP.varName = varNameXYZ;
   MP.setOrder(MonoPool::DRL);
   const char *sg[] = {
      "y^3+x*y",
      "x*y^2+x^2",
      "x^2*y-2*y^2+x",
      "x^3-3*x*y"};
   ObjArray<Polynomial> g;
   for (int i = 0; i < NELEM(sg); ++i) {
      Polynomial& gi = g.push();
      gi.init(sg[i], 0, 0, "xy");
      gi.sort();
   }

   Polynomial p, r;
   p.init("x^7 - 2 x^4*y^3 + 3 y^6 + x*y^3 + 3 y", 0, 0, "xy");
   p.sort();

   SimpleReductor sr(g);
   while (sr.reduceStep(r, p))
      p.copy(r);
   r.toStr(buf);
   CHECK_MATCH("-92 y^2 - 9 x*y + 3 y + 46 x");
   TEST_POST();
}

void testSimple (void)
{
   TEST_PRE("testSimple");
   MP.varName = varNameXYZ;
   Polynomial p;

   p.init("x2y+13xy2+xy+x+y3+y2", 0, 0, "xyz");
   p.sort();

   Scheme scheme;
   SchemeSimple strategy(scheme, p);
   strategy.build();
   Printer prn;
   ArrayOutput output(buf);
   prn.evaluate(&output, scheme);
   output.writeChar(0);
   CHECK_MATCH("(((((x^2*y + 13 * x*y^2) + x*y) + x) + y^3) + y^2)");
   Evaluator eval;
   Array<int> values;
   values.push(3);
   values.push(-1);
   bigint_t res;
   BigInt::init(res);
   eval.evaluate(res, scheme, values);
      if (BigInt::cmp(res, 30) != 0)
         throw Exception("%s: Error: Result doesn't match expected value", _name);
   BigInt::clear(res);
   TEST_POST();
}

void testGorner (void)
{
   TEST_PRE("testGorner");
   MP.varName = varNameXYZ;
   Polynomial p;

   p.init("x2y+13xy2+xy+x+y3+y2", 0, 0, "xyz");
   p.sort();

   Scheme scheme;
   SchemeGorner strategy(scheme, p);
   strategy.build();
   Printer prn;
   ArrayOutput output(buf);
   prn.evaluate(&output, scheme);
   output.writeChar(0);
   CHECK_MATCH("(x * (y * (x + (13 * y + 1)) + 1) + y^2 * (y + 1))");   
   Evaluator eval;
   Array<int> values;
   values.push(3);
   values.push(-1);
   bigint_t res;
   BigInt::init(res);
   eval.evaluate(res, scheme, values);
      if (BigInt::cmp(res, 30) != 0)
         throw Exception("%s: Error: Result doesn't match expected value", _name);
   BigInt::clear(res);
   TEST_POST();
}

void testTree (void)
{
   TEST_PRE("testMST");
   MP.varName = varNameXYZ;
   Polynomial p;

   p.init("x2y+13xy2+xy+x2+y3+y2", 0, 0, "xyz");
   p.sort();

   Scheme scheme;
   SchemeHangingTree strategy(scheme, p);
   strategy.build();
   Printer prn;
   ArrayOutput output(buf);
   prn.evaluate(&output, scheme);
   output.writeChar(0);
   CHECK_MATCH("(((((x^2 + x*y) + y^2) + x * x*y) + 13 * x * y^2) + y * y^2)");
   Evaluator eval;
   Array<int> values;
   values.push(3);
   values.push(-1);
   bigint_t res;
   BigInt::init(res);
   eval.evaluate(res, scheme, values);
      if (BigInt::cmp(res, 36) != 0)
         throw Exception("%s: Error: Result doesn't match expected value", _name);
   BigInt::clear(res);
   TEST_POST();
}

void testGMP() {
   TEST_PRE("testGMP");
   bigint_t a, b, c;
   buf.resize(2048);
   BI::init(a),BI::init(b),BI::init(c);
   BI::set(a, 19283); BI::set(b, 723890); BI::mul(c, a, b);
   gmp_sprintf(buf.ptr(), "%Zd", c);
   CHECK_MATCH("13958770870");
   BI::set(a, 232111124); BI::set(b, 342123); BI::mul(c, a, b);
   gmp_sprintf(buf.ptr(), "%Zd", c);
   CHECK_MATCH("79410554076252");
   BI::set(a, "1298371928379482364826482364"); BI::set(b, "123812389162"); BI::add(c, a, b);
   gmp_sprintf(buf.ptr(), "%Zd", c);
   CHECK_MATCH("1298371928379482488638871526");
   BI::pow(c, 123, 4);
   gmp_sprintf(buf.ptr(), "%Zd", c);
   CHECK_MATCH("228886641");
   BI::clear(a),BI::clear(b),BI::clear(c);
   TEST_POST();
}

void testScript() {
   TEST_PRE("testScript");
   MP.varName = varNameXYZ;
   printf("\n%s:\n", _name);
   ScriptInterpreter interpreter;
   const char* input = "vars xyz             \n\
      set p1 x3yz+x2y+13xy2+67xyz+x+11y3+26y2\n\
      build sg p1 gorner                     \n\
      build ss p1 simple                     \n\
      build sm p1 tree                       \n\
      eval sg 23,-112,7                      \n\
      eval ss 23,-112,7                      \n\
      eval sm 23,-112,7                      \n\
   ";
   BufferScanner scanner(input);
   interpreter.execute(scanner);
   TEST_POST();
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
   MP.setOrder(MonoPool::LEX);
   try {
      testMonome();
      testMonDiv();
      testPolyInit();
      testPolySort();
      testPolyAdd();
      testPolyAdd2();
      testPolyMul();
      testGMP();

      testSimple();
      testGorner();
      testTree();
      testReduce();

      testScript();
   } catch (Exception ex) {
      printf("Error: %s", ex.message());
   }
   
   try {
      MP.checkLeaks(true);
   } catch (Exception ex) {
      printf("Error: %s", ex.message());
   }
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