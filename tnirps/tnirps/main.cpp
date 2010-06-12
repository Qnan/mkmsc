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
#include "tnirps_poly_printer.h"
#include "tnirps_poly_reduction.h"
#include "tnirps_script.h"
#include "tnirps_hashset.h"
#include "scanner.h"
#include "tnirps_var_map.h"
#include "profiling.h"

#include <gmp.h>

#define TEST_PRE(name) \
   const char* _name = name; \
   MP.reset(); \
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
   MP.setOrder(MonoPool::DRL);
   MP.setVarMap("x,y,z");
   Polynomial p;
   //const char *s = "13*x*y^2 + z^3 - 71*x + 10*x*y + 4*x^2*y*z - y + y^2*z";
   const char *s = "y^2 + x*y^2*z^2 + x^2*z + y^5 + y^4*z + x^3 + x*z + x + z^5 + x^3*z^2 + x*y*z + y^4 + y^3*z^2 + y*z^4 + x*y*z^3 + y*z^2 + x*y^2 + x^4 + y^2*z^2 + y^3";
   const char *w = "y^5 + y^4*z + x^3*z^2 + x*y^2*z^2 + y^3*z^2 + x*y*z^3 + y*z^4 + z^5 + x^4 + y^4 + y^2*z^2 + x^3 + x*y^2 + y^3 + x^2*z + x*y*z + y*z^2 + y^2 + x*z + x";
   p.init(s);
   p.toStr(buf);
   printf("orig: %s\n", s);
   printf("drl: %s\n", w);
   printf("our: %s\n", buf.ptr());
   printf("sorted: %s\n", p.isSorted() ? "yes" : "no");
   //CHECK_MATCH("13*x*y^2 + z^3 - 71*x + 10*x*y + 4*x^2*y*z - y + y^2*z");
   //p.sortDrl();
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
   if (p.isSorted())
      printf("sorted");
   else
      printf("not sorted");
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

   Cf c1;
   Ring::set(c1, 3);
   Cf c2;
   Ring::set(c2, -6);

   p.add(p1, &c1, &c2);
   p.toStr(buf);
   CHECK_MATCH("12*x^2*y*z + 39*x*y^2 + 36*x*y - 213*x + 3*y^2*z - 3*y + 3*z^3 - 60*z^2 - 42");
   TEST_POST();
}

void testPolyMul (void)
{
   TEST_PRE("testPolyMul");
   MP.setVarMap("x,y,z");
   Polynomial p;
   const char *s1 = "13*x*y^2 + z^3 - 71*x + 10*x*y + 4*x^2*y*z - y + y^2*z";
   p.init(s1, 0, 0);
   p.sort();
   const char *s2 = "z^2";
   Monomial m = MP.init(s2, 0, 0);

   p.mul(m);
   Cf c;
   Ring::set(c, -2);
   p.mulnum(c);
   p.toStr(buf);
   CHECK_MATCH("-8*x^2*y*z^3 - 26*x*y^2*z^2 - 20*x*y*z^2 + 142*x*z^2 - 2*y^2*z^3 + 2*y*z^2 - 2*z^5");
   TEST_POST();
}

void testPolySimplify (void)
{
   TEST_PRE("testPolySimplify");
   MP.setVarMap("x,y,z");
   Polynomial p1, p2, p3;
   const char *s1 = "13*x*y^2 + z^3 - 71*x + 10*x*y + 4*x^2*y*z - y + y^2*z",
           *s2 = "x - x*y*z",
           *s3 = "x - 4*x*y^2 + 2 + 13";
   p1.init(s1, 0, 0);
   p2.init(s2, 0, 0);
   p3.init(s3, 0, 0);
   Cf c;
   Ring::set(c, 70);
   p1.add(p2, NULL, &c);
   p1.add(p3, NULL, NULL);
   p1.toStr(buf);
   CHECK_MATCH("13*x*y^2 + z^3 - 71*x + 10*x*y + 4*x^2*y*z - y + y^2*z + 70*x - 70*x*y*z + x - 4*x*y^2 + 2 + 13");
   //p1.simplify();
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
   while (sr.reduceStep(r, p)) {
      p.copy(r);
   }
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
   SchemeSimple strategy(scheme, p);
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
//   Evaluator eval;
//   Array<int> values;
//   values.push(3);
//   values.push(-1);
//   NumPtr res;
//   eval.evaluate(res, scheme, values);
//   if (NP.cmp(res.get(), 30) != 0)
//      throw Exception("%s: Error: Result doesn't match expected value", _name);
   TEST_POST();
}

void testGorner (void)
{
   TEST_PRE("testGorner");
   MP.setVarMap("x,y");
   Polynomial p;

   p.init("x^2*y + 13*x*y^2 + x*y + x + y^3 + y^2");
   //p.simplify();

   Scheme scheme;
   SchemeGorner strategy(scheme, p);
   strategy.build();
   Printer prn;
   ArrayOutput output(buf);
   prn.evaluate(&output, scheme);
   output.writeChar(0);
   CHECK_MATCH("(x * (y * (x + (13 * y + 1)) + 1) + y^2 * (y + 1))");   
//   Evaluator eval;
//   Array<int> values;
//   values.push(3);
//   values.push(-1);
//   NumPtr res;
//   eval.evaluate(res, scheme, values);
//   if (NP.cmp(res.get(), 30) != 0)
//      throw Exception("%s: Error: Result doesn't match expected value", _name);
   TEST_POST();
}

void testTree (void)
{
   TEST_PRE("testTree");
   MP.setVarMap("x,y");
   Polynomial p;

   p.init("x^2*y + 13*x*y^2 + x*y + x^2 + y^3 + y^2");
   //p.simplify();

   Scheme scheme;
   SchemeHangingTree strategy(scheme, p);
   strategy.build();
   Printer prn;
   ArrayOutput output(buf);
   prn.evaluate(&output, scheme);
   output.writeChar(0);
   CHECK_MATCH("(((((x^2 + x*y) + y^2) + x * x*y) + 13 * x * y^2) + y * y^2)");
//   Evaluator eval;
//   Array<int> values;
//   values.push(3);
//   values.push(-1);
//   NumPtr res;
//   eval.evaluate(res, scheme, values);
//   if (NP.cmp(res.get(), 36) != 0)
//      throw Exception("%s: Error: Result doesn't match expected value", _name);
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
   MP.setOrder(MonoPool::DRL);
   MP.setVarMap("");
   printf("\n%s:\n", _name);
   ScriptInterpreter interpreter;
   const char* input = "vars x,y,z                             \n\
      set p1 x^3*y*z+x^2*y+13*x*y^2+67*x*y*z+x+11*y^3+26*y^2   \n\
      set p2 x^650*y^667                                       \n\
      set p3 x^7 - 2*x^4*y^3 + 3*y^6 + x*y^3 + 3*y             \n\
      build sg p1 gorner                                       \n\
      build ss p1 simple                                       \n\
      build sm p1 tree                                         \n\
      eval sg 23,-112,7                                        \n\
      eval ss 23,-112,7                                        \n\
      eval sm 23,-112,7                                        \n\
      build ss3 p3 simple                                      \n\
      build ss2 p2 simple                                      \n\
      reduce ss2 y^3+x*y, x*y^2+x^2, x^2*y-2*y^2+x, x^3-3*x*y  \n";

   BufferScanner scanner(input);
   interpreter.execute(scanner);
   TEST_POST();
}

int tests ()
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

int cmp (const int& a, const int& b) {
   return b-a;
}

void testSort ()
{
   Pool<List<int>::Elem> pool;
   Array<int> ids;
   srand(70036);
   for (int i = 0; i < 1000; ++i)
      ids.push(pool.add());

   List<int> a(pool);
   for (int i = 0; i < 10; ++i) {
      int j = rand() % ids.size();
      if (ids[j] >= 0) {
         pool.remove(j);
         ids[j] = -1;
      }

      a.add(rand() % 100);
   }
   for (int i = a.begin(); i < a.end(); i = a.next(i))
      printf(" %d", a.at(i));
   printf("\n");
   a.bubbleSort(cmp);
   bool error = false;
   for (int i = a.begin(); a.next(i) < a.end(); i = a.next(i))
      if (a.at(i) > a.at(a.next(i)))
         error = true;
   if (error)
      for (int i = a.begin(); i < a.end(); i = a.next(i))
         printf(" %d", a.at(i));
   printf("\n");

}

int testMaple (const char* path) {
   FileScanner fs(path);
   Array<char> buf, sub;
   fs.skipSpace();
   fs.readChar();fs.readChar();
   fs.readWord(buf,"\n\r");
   MP.setVarMap(buf.ptr());
   fs.skipSpace();
   fs.readChar();fs.readChar();
   fs.readWord(buf,"\n\r");

   ObjArray<Polynomial> basis;
   BufferScanner bs(buf);
   while (!bs.isEOF()) {
      bs.readWord(sub, ",");
      basis.push().init(sub.ptr());
      basis.top().simplify();
      if (!bs.isEOF())
         bs.readChar();
   }

   Polynomial p;
   fs.skipSpace();
   fs.readChar();fs.readChar();
   fs.readWord(buf,"\n\r");
   p.init(buf.ptr());
   //p.simplify();

   Polynomial r;
   fs.skipSpace();
   fs.readChar();fs.readChar();
   fs.readWord(buf,"\n\r");
   r.init(buf.ptr());
   //r.simplify();

   fs.skipSpace();
   fs.readChar();fs.readChar();
   int t = fs.readInt();

   printf("G:\n");
   for (int i = 0; i < basis.size(); ++i) {
      basis[i].print(sout);printf("\n");
   }
   printf("p:\n");
   p.print(sout);printf("\n");
   printf("r:\n");
   p.print(sout);printf("\n");
   printf("t: %i\n", t);

   Scheme s;
   SchemeSimple ss(s, p);
   ss.build();

   Reductor redr(basis);
   float time;
   Polynomial res;
   qword t0 = nanoClock();
   redr.reduce(res, s);
   qword t1 = nanoClock();
   time = 1000.0f * nanoHowManySeconds(t1 - t0);
   printf("\ttime: %.3f ms\n", time);
   printf("\tresult: "), res.print(sout), printf("\n");
}

int testSing (const char* mode, const char* path) {
   Array<char> buf, fname;
   Polynomial p, r, r1;
   int t;
   ObjArray<Polynomial> basis;
   {
      FileScanner fs("%s/v", path);
      fs.skipSpace();
      fs.readAll(buf);
      while (isspace(buf.top()))
         buf.pop();
      buf.push(0);
      MP.setVarMap(buf.ptr());
   }
   {
      FileScanner fs("%s/g", path);
      fs.skipSpace();
      while (!fs.isEOF()) {
         fs.readWord(buf, ",");
         basis.push().init(buf.ptr());
         basis.top().simplify();
         if (!fs.isEOF())
            fs.readChar();
      }
   }
   {
      FileScanner fs("%s/p", path);
      fs.skipSpace();
      fs.readAll(buf);
      while (isspace(buf.top()))
         buf.pop();
      buf.push(0);
      p.init(buf.ptr());
      //p.simplify();
   }
   {
      FileScanner fs("%s/r", path);
      fs.skipSpace();
      fs.readAll(buf);
      while (isspace(buf.top()))
         buf.pop();
      buf.push(0);
      r.init(buf.ptr());
      //r.simplify();
   }
   {
      FileScanner fs("%s/t", path);
      fs.skipSpace();
      t = fs.readInt();
   }
   Polynomial r2;

   printf("G:\n");
   for (int i = 0; i < basis.size(); ++i) {
      basis[i].print(sout);printf("\n");
   }
//   scanf("*");
   printf("p:\n");
   p.print(sout);printf("\n");
//   scanf("*");
//   printf("r:\n");
//   r.print(sout);printf("\n");
//   scanf("*");
   printf("t: %i\n", t);
   //scanf("*");


   //Polynomial q;
//   Cf cc;
//   Ring::set(cc, 1);
//   p.copy(basis[0]);
   printf("p:\n");
   p.print(sout);printf("\n");
   //p.addTerm(MP.single(0), cc);
   Scheme s;
   if (strcmp(mode, "simple") == 0) {
      SchemeSimple ss(s, p);
      ss.build();
   } else if (strcmp(mode, "gorner") == 0) {
      SchemeGorner ss(s, p);
      ss.build();
   } else if (strcmp(mode, "tree") == 0) {
      SchemeHangingTree ss(s, p);
      ss.build();
   } else {
      printf("mode not recognized: %s\n", mode);
      return 1;
   }
   printf("scheme built\n");

   Reductor redr(basis);
   float time;
   Polynomial res;
   qword t0 = nanoClock();
   profTimerStart(MonoEval, "reduction time");
   redr.reduce(res, s);
   qword t1 = nanoClock();
   profTimerStop(MonoEval);
   time = nanoHowManySeconds(t1 - t0);
   printf("\ttime: %.5lf s\n", time);
//   Cf f;
//   Ring::set(f, -1);
//   r1.add(r, NULL, &f);
//   r1.simplify();
   printf("\tresult: "), res.print(sout), printf("\n");
}

int main (int argc, const char** argv) {
   MP.setOrder(MonoPool::DRL);
//   printf("Press any key to start...");
//   scanf("*");
   Ring::init();
   Polynomial::_pool.reserve(2000000);

//   printf("%d\n", 720%31);
//   printf("%d\n", ((-11)%31+31)%31);
//   printf("%d\n", ((-44)%31+31)%31);

//   Cf a, b, c;
//   Ring::set(a, 5);
//   Ring::set(b, 26);
//   Ring::add(c, a, b);
//   printf("5 + 26 = "); Ring::print(sout, c); printf("\n");
//   Ring::mul(c, a, b);
//   printf("5 * 26 = "); Ring::print(sout, c); printf("\n");
//   Ring::div(c, a, b);
//   printf("5 / 26 = "); Ring::print(sout, c); printf("\n");
//
//   for (int i = 0; i < argc; ++i)
//      printf("%s\n", argv[i]);
   //testSing(argv[1], argv[2]);
   //profGetStatistics(sout, true);
   //printf("\tptime: %.5lf s\n", nanoHowManySeconds(tt));
   testSing("simple", "../sage/p31z1/");
   //testMaple("samples/p1.txt");
   //tests();
}
