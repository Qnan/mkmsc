#include <stdio.h>
#include "array.h"
#include "scanner.h"
#include "output.h"
#include "obj_array.h"
#include "red_black.h"
#include "pool.h"
#include "list.h"
#include "tnirps_var_map.h"
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
#include "tnirps_poly_reduction.h"
#include "tnirps_script.h"
#include "tnirps_hashset.h"
#include "scanner.h"
#include "tnirps_var_map.h"

#include <gmp.h>

#define TEST_PRE(name) \
   const char* _name = name; \
   MP.setOrder(MonoPool::LEX); \
   Array<char> buf

#define TEST_POST() \
   printf("%s succeeded\n", _name)

#define CHECK_MATCH(str) \
   if (strcmp(buf.ptr(), str) != 0) \
      throw Exception("%s: Error:\n\t%s\n\t!=\n\t%s\n", _name, buf.ptr(), str)

void testVarMap (void) {
   TEST_PRE("testVarMap");
   VarMap map("x1, y2, xxx3");

   if (strcmp(map.name(1), "y2") != 0 || map.id("xxx3") != 2)
      throw Exception("%s: Error", _name);

   TEST_POST();
}

void testMonome (void) {
   TEST_PRE("testMonome");
   MP.setVarMap("x0,x1,x2,x3,x4,x5,x6,x7");
   Monomial m1, m2, m3, m4, m5;
   const int h1 = 11564336; // hash
   const char *s1 = "x1^3*x2^5*x3^7*x5^11",
           *s2 = "x2^21*x3^22*x4^23*x6^24",
           *s3 = "x1^3*x2^26*x3^29*x4^23*x5^11*x6^24",
           *s4 = "x2^21*x3^22*x4^23*x6^24",
           *s5 = "x2^5*x3^7";
   m1 = MP.init(s1);
   MP.toStr(buf, m1);
   if (strcmp(buf.ptr(), s1) != 0)
      throw Exception("%s: Monomial printing mismatch: %s != %s", _name, buf.ptr(), s1);

   int h = MP.countHash(m1);
   if (h != h1)
      throw Exception("%s: Hash mismatch: %X != %X", _name, h, h1);

   m2 = MP.init(s2);
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
   MP.setVarMap("x,y");
   Monomial m[3];
   const char* dd[] = {"x^5 * y^7", "x^6 * y^5", "x^5 * y^5"};

   for (int i = 0; i < 3; ++i)
      m[i] = MP.init(dd[i]);

   if (!MP.divides(m[0],m[2]) || !MP.divides(m[1],m[2]) || MP.divides(m[0],m[1])
            || MP.divides(m[1],m[0])  || MP.divides(m[2],m[0])  || MP.divides(m[2],m[1]))
      throw Exception("%s: Divisibility check failed", _name);

   TEST_POST();
}

void testPolyInit (void)
{
   TEST_PRE("testPolyInit");
   MP.setVarMap("x,y,z");
   Polynomial p;
   const char *s = "13*x*y^2 + z^3 - 71*x + 10*x*y + 4*x^2*y*z - y + y^2*z";
   p.init(s);
   p.toStr(buf);
   CHECK_MATCH("13*x*y^2 + z^3 - 71*x + 10*x*y + 4*x^2*y*z - y + y^2*z");
   TEST_POST();
}

void testPolySort (void)
{
   TEST_PRE("testPolySort");
   MP.setVarMap("x,y,z");
   Polynomial p;
   const char *s = "13*x*y^2 + z^3 - 71*x + 10*x*y + 4*x^2*y*z - y + y^2*z";
   p.init(s);
   p.sort();
   p.toStr(buf);
   CHECK_MATCH("4*x^2*y*z + 13*x*y^2 + 10*x*y - 71*x + y^2*z - y + z^3");
   TEST_POST();
}

void testPolyAdd (void)
{
   TEST_PRE("testPolyAdd");
   MP.setVarMap("x,y,z");
   Polynomial p, p1;
   const char *s1 = "13*x*y^2 + z^3 - 71*x + 10*x*y + 4*x^2*y*z - y + y^2*z";
   p.init(s1);
   p.sort();
   const char *s2 = "5*x - x*y^2 + 17*z^3 - 10*z^2 - 10*x*y + 5*y - 1 + x*y*z";
   p1.init(s2);
   p1.sort();

   p.add(p1);
   p.toStr(buf);
   CHECK_MATCH("4*x^2*y*z + 12*x*y^2 + x*y*z - 66*x + y^2*z + 4*y + 18*z^3 - 10*z^2 - 1");
   TEST_POST();
}

void testPolyAdd2 (void)
{
   TEST_PRE("testPolyAdd2");
   MP.setVarMap("x,y,z");
   Polynomial p, p1;
   const char *s1 = "13*x*y^2 + z^3 - 71*x + 10*x*y + 4*x^2*y*z - y + y^2*z";
   p.init(s1);
   p.sort();
   const char *s2 = "10*z^2 - x*y + 7";
   p1.init(s2);
   p1.sort();

   p.add(p1, 3, -6);
   p.toStr(buf);
   CHECK_MATCH("12*x^2*y*z + 39*x*y^2 + 36*x*y - 213*x + 3*y^2*z - 3*y + 3*z^3 - 60*z^2 - 42");
   TEST_POST();
}

void testPolyMul (void)
{
   TEST_PRE("testPolyMul");
   Polynomial p;
   const char *s1 = "13*x*y^2 + z^3 - 71*x + 10*x*y + 4*x^2*y*z - y + y^2*z";
   p.init(s1, 0, 0);
   p.sort();
   const char *s2 = "z^2";
   Monomial m = MP.init(s2, 0, 0);

   p.mul(m);
   p.mulnum(-2);
   p.toStr(buf);
   CHECK_MATCH("-8*x^2*y*z^3 - 26*x*y^2*z^2 - 20*x*y*z^2 + 142*x*z^2 - 2*y^2*z^3 + 2*y*z^2 - 2*z^5");
   TEST_POST();
}

void testPolySimplify (void)
{
   TEST_PRE("testPolyMul");
   Polynomial p1, p2, p3;
   const char *s1 = "13*x*y^2 + z^3 - 71*x + 10*x*y + 4*x^2*y*z - y + y^2*z",
           *s2 = "x - x*y*z",
           *s3 = "x - 4*x*y^2 + 2 + 13";
   p1.init(s1, 0, 0);
   p2.init(s2, 0, 0);
   p3.init(s3, 0, 0);
   p1.append(p2, 70);
   p1.append(p3);
   p1.toStr(buf);
   CHECK_MATCH("13*x*y^2 + z^3 - 71*x + 10*x*y + 4*x^2*y*z - y + y^2*z + 70*x - 70*x*y*z + x - 4*x*y^2 + 2 + 13");
//   p1.sort();
//   p1.toStr(buf);
//   CHECK_MATCH("4*x^2*y*z + 13*x*y^2 - 70*x*y*z - 4*x*y^2 + 10*x*y - 71*x + 70*x + x + y^2*z - y + z^3 + 2 + 13");
   p1.simplify();
   p1.toStr(buf);
   CHECK_MATCH("4*x^2*y*z + 9*x*y^2 - 70*x*y*z + 10*x*y + y^2*z - y + z^3 + 15");
   TEST_POST();
}

void testPolyLoad (void) {
   // test
   //    initialization
}

void testReduce (void) {
   TEST_PRE("testReduce");
   MP.setVarMap("x,y");
   MP.setOrder(MonoPool::DRL);
   const char *sg[] = {
      "y^3+x*y",
      "x*y^2+x^2",
      "x^2*y-2*y^2+x",
      "x^3-3*x*y"};
   ObjArray<Polynomial> g;
   for (int i = 0; i < NELEM(sg); ++i) {
      Polynomial& gi = g.push();
      gi.init(sg[i]);
      gi.sort();
   }

   Polynomial p, r;
   p.init("x^7 - 2*x^4*y^3 + 3*y^6 + x*y^3 + 3*y");
   p.sort();

   SimpleReductor sr(g);
   while (sr.reduceStep(r, p))
      p.copy(r);
   r.toStr(buf);
   CHECK_MATCH("-92*y^2 - 9*x*y + 3*y + 46*x");
   TEST_POST();
}

void testReduceIncr (void) {
   TEST_PRE("testReduceIncr");
   MP.setVarMap("x,y");
   MP.setOrder(MonoPool::DRL);
   const char *sg[] = {
      "y^3+x*y",
      "x*y^2+x^2",
      "x^2*y-2*y^2+x",
      "x^3-3*x*y"};
   ObjArray<Polynomial> g;
   for (int i = 0; i < NELEM(sg); ++i) {
      Polynomial& gi = g.push();
      gi.init(sg[i]);
      gi.sort();
   }

   Polynomial p, r;
   p.init("x^7 - 2*x^4*y^3 + 3*y^6 + x*y^3 + 3*y");
   p.sort();

   Scheme scheme;
   SchemeHangingTree strategy(scheme, p);
   strategy.build();
   Reductor reductor(g);
   reductor.reduce(r, scheme);
   r.toStr(buf);
   CHECK_MATCH("-92*y^2 - 9*x*y + 3*y + 46*x");
   TEST_POST();
}

void testSimple (void)
{
   TEST_PRE("testSimple");
   MP.setVarMap("x,y");

   Polynomial p;

   p.init("x^2*y + 13*x*y^2 + x*y + x + y^3 + y^2");
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
   MP.setVarMap("x,y");
   Polynomial p;

   p.init("x^2*y + 13*x*y^2 + x*y + x + y^3 + y^2");
   p.simplify();

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
   TEST_PRE("testTree");
   MP.setVarMap("x,y");
   Polynomial p;

   p.init("x^2*y + 13*x*y^2 + x*y + x^2 + y^3 + y^2");
   p.simplify();

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
   MP.setVarMap("");
   printf("\n%s:\n", _name);
   ScriptInterpreter interpreter;
   const char* input = "vars x,y,z             \n\
      set p1 x^3*y*z+x^2*y+13*x*y^2+67*x*y*z+x+11*y^3+26*y^2\n\
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
      testVarMap();
      testMonome();
      testMonDiv();
      testPolyInit();
      testPolySort();
      testPolyAdd();
      testPolyAdd2();
      testPolyMul();
      testPolySimplify();
      testGMP();

      testSimple();
      testGorner();
      testTree();
      testReduce();

      testReduceIncr();
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