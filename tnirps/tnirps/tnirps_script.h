#ifndef __TNIRPS_SCRIPT_H__
#define __TNIRPS_SCRIPT_H__

#include "scanner.h"
#include "obj_array.h"
#include "tnirps_scheme_gorner.h"
#include "tnirps_scheme_simple.h"
#include "tnirps_scheme_tree.h"
#include "tnirps_poly_evaluator.h"
#include "nano.h"
#include "tnirps_poly_reduction.h"

class VarMap;

class ScriptInterpreter {
public:
   ScriptInterpreter () {}

   int execute (Scanner& sc) {
      Array<char> line;
      int r = 0;
      while (!sc.isEOF() && r == 0) {
         sc.readWord(line, "\n\r");
         printf("\t%s\n", line.ptr());
         BufferScanner lsc(line);
         r = executeLine(lsc);
         sc.skipSpace();
      }
      return r;
   }

private:
   int setPoly (const char* name, const char* body) {
      Polynomial& p = poly.value(poly.insert(name));
      p.init(body);
      printf("\t%s = ", name), p.print(sout), printf("\n");
      return 0;
   }
   int buildScheme(const char* schname, const char* name, const char* arg) {
      if (scheme.find(schname)) {
         printf("\tERROR: scheme \"%s\" already defined\n", schname);
         return 1;
      }
      if (!poly.find(name)) {
         printf("\tERROR: polynomial \"%s\" not defined\n", name);
         return 1;
      }
      float time = 0.0f;
      Scheme& s = scheme.value(scheme.insert(schname));
      if (!strcmp(arg, "simple")) {
         SchemeSimple ss(s, poly.at(name));
         qword t0 = nanoClock();
         ss.build();
         qword t1 = nanoClock();
         time = 1000.0f * nanoHowManySeconds(t1 - t0);
      } else if (!strcmp(arg, "gorner")) {
         SchemeGorner sg(s, poly.at(name));
         qword t0 = nanoClock();
         sg.build();
         qword t1 = nanoClock();
         time = 1000.0f * nanoHowManySeconds(t1 - t0);
      } else if (!strcmp(arg, "tree")) {
         SchemeHangingTree sm(s, poly.at(name));
         qword t0 = nanoClock();
         sm.build();
         qword t1 = nanoClock();
         time = 1000.0f * nanoHowManySeconds(t1 - t0);
      } else {
         printf("\tERROR: scheme \"%s\" unknwon\n", arg);
         return 1;
      }
      printf("\n");
      printf("\ttime: %.3f ms\n", time);
      printf("\tscheme built\n");
      return 0;
   }
   int evaluate(const char* schname, const char* arg) {
      if (!scheme.find(schname)) {
         printf("\tERROR: scheme \"%s\" not defined\n", schname);
         return 1;
      }

      BufferScanner bs(arg);
      Array<int> vvals;
      while (!bs.isEOF()) {
         vvals.push(bs.readInt());
         while (!bs.isEOF()) {
            int next = bs.lookNext();
            if (!isspace(next) && next != ',')
               break;
            bs.readChar();
         }
      }
      int nvars = MP.getVarMap().size();
      if (vvals.size() != nvars) {
         printf("\tERROR: invalid number of values provided for arguments, %i expected\n", nvars);
         return 1;
      }
//      Evaluator eval;
//      float time;
//      NumPtr res;
//      qword t0 = nanoClock();
//      eval.evaluate(res, scheme.at(schname), vvals);
//      qword t1 = nanoClock();
//      time = 1000.0f * nanoHowManySeconds(t1 - t0);
//      printf("\ttime: %.3f ms\n", time);
//      printf("\tresult: "), NP.print(sout, res.get()), printf("\n");
      return 0;
   }
   int reduce(const char* schname, const char* arg) {
      if (!scheme.find(schname)) {
         printf("\tERROR: scheme \"%s\" not defined\n", schname);
         return 1;
      }

      BufferScanner bs(arg);
      Array<char> buf;
      ObjArray<Polynomial> basis;
      while (!bs.isEOF()) {
         bs.readWord(buf, ",");
         buf.push(0);
         basis.push().init(buf.ptr());
         if (!bs.isEOF())
            bs.readChar();
      }

      Reductor redr(basis);
      float time;
      Polynomial res;
      qword t0 = nanoClock();
      redr.reduce(res, scheme.at(schname));
      qword t1 = nanoClock();
      time = 1000.0f * nanoHowManySeconds(t1 - t0);
      printf("\ttime: %.3f ms\n", time);
      printf("\tresult: "), res.print(sout), printf("\n");
      return 0;
   }
   int executeLine (Scanner& sc) {

      Array<char> command, name, arg, schname;
      sc.skipSpace();
      sc.readWord(command, 0);
      sc.skipSpace();
      int r = 0;
      if (!strcmp(command.ptr(), "set")) {
         sc.readWord(name, 0);
         sc.skipSpace();
         sc.readWord(arg, "\n");
         r = setPoly(name.ptr(), arg.ptr());
      } else if (!strcmp(command.ptr(), "vars")) {
         sc.readWord(arg, 0);
         try {
            MP.setVarMap(arg.ptr());
            r = 0;
         } catch (Exception& ex) {
            printf("\tERROR: can't parse variable names: %s\n", arg.ptr());
            r = 1;
         }
      } else if (!strcmp(command.ptr(), "build")) {
         sc.readWord(schname, 0);
         sc.skipSpace();
         sc.readWord(name, 0);
         sc.skipSpace();
         sc.readWord(arg, 0);
         r = buildScheme(schname.ptr(), name.ptr(), arg.ptr());
      } else if (!strcmp(command.ptr(), "eval")) {
         sc.readWord(name, 0);
         sc.skipSpace();
         sc.readWord(arg, "\n");
         r = evaluate(name.ptr(), arg.ptr());
      } else if (!strcmp(command.ptr(), "reduce")) {
         sc.readWord(name, 0);
         sc.skipSpace();
         sc.readWord(arg, "\n");
         r = reduce(name.ptr(), arg.ptr());
      } else {
         printf("\tERROR: command unknown: \"%s\"", command.ptr());
         return 1;
      }
      return r;
   }
private:
   RedBlackStringObjMap<Polynomial> poly;
   RedBlackStringObjMap<Scheme> scheme;
};

#endif /* __TNIRPS_SCRIPT_H__ */

