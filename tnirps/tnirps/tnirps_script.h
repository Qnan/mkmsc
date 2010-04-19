#ifndef __TNIRPS_SCRIPT_H__
#define __TNIRPS_SCRIPT_H__

#include "scanner.h"
#include "obj_array.h"
#include "tnirps_scheme_gorner.h"
#include "tnirps_poly_evaluator.h"

class ScriptInterpreter {
public:
   ScriptInterpreter () {}

   int execute (Scanner& sc) {
      Array<char> line;
      int r = 0;
      while (!sc.isEOF() && r == 0) {
         sc.readWord(line, "\n\r");
         printf("%s\n", line.ptr());
         BufferScanner lsc(line);
         r = executeLine(lsc);
         sc.skipSpace();
      }
      return r;
   }

private:
   static const char* varName (int i) {
      return vars1[i].ptr();
   }
   int setVars (const Array<char>& str) {
      vars.copy(str);
      vars1.clear();
      for (int i = 0; i < strlen(vars.ptr()); ++i) {
         Array<char>& a = vars1.push();
         a.push(vars[i]);
         a.push(0);
      }
      MP.varName = varName;
      return 0;
   }
   int setPoly (const char* name, const char* body) {
      Polynomial& p = poly.value(poly.insert(name));
      p.init(body, 0, 0, vars.ptr());
      printf("%s = ", name), p.print(), printf("\n");
      return 0;
   }
   int buildScheme(const char* schname, const char* name, const char* arg) {
      if (scheme.find(schname)) {
         printf("scheme \"%s\" already defined\n", schname);
         return 1;
      }
      if (!poly.find(name)) {
         printf("polynomial \"%s\" not defined\n", name);
         return 1;
      }
      if (!strcmp(arg, "simple")) {
         printf("scheme not implemented\n");
         return 1;
      } else if (!strcmp(arg, "gorner")) {
         Scheme& s = scheme.value(scheme.insert(schname));
         SchemeGorner sg(s, poly.at(name));
         sg.build();
         printf("\n");
         printf("scheme built\n");
      } else if (!strcmp(arg, "mst")) {
         printf("scheme not implemented\n");
         return 1;
      } else {
         printf("scheme \"%s\" unknwon\n", arg);
         return 1;
      }
      return 0;
   }
   int evaluate(const char* schname, const char* arg) {
      if (!scheme.find(schname)) {
         printf("scheme \"%s\" not defined\n", schname);
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
      if (vvals.size() != strlen(vars.ptr())) {
         printf("invalid number of values provided for arguments, %i expected\n", strlen(vars.ptr()));
         return 1;
      }
      Evaluator eval;
      eval.evaluate(scheme.at(schname), vvals);
      return 0;
   }
   int executeLine (Scanner& sc) {

      Array<char> command, name, arg, schname;
      sc.skipSpace();
      sc.readWord(command, 0);
      printf("command: %s\n", command.ptr());
      sc.skipSpace();
      int r = 0;
      if (!strcmp(command.ptr(), "set")) {
         sc.readWord(name, 0);
         sc.skipSpace();
         sc.readWord(arg, "\n");
         r = setPoly(name.ptr(), arg.ptr());
      } else if (!strcmp(command.ptr(), "vars")) {
         sc.readWord(arg, 0);
         r = setVars(arg);
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
      } else if (!strcmp(command.ptr(), "prnp")) {
         //sc.readWord(name);
      } else {
         printf("command unknown: \"%s\"", command.ptr());
         return 1;
      }
      return r;
   }
private:
   Array<char> vars;
   static ObjArray<Array<char> > vars1; // TODO: remove the static hack
   RedBlackStringObjMap<Polynomial> poly;
   RedBlackStringObjMap<Scheme> scheme;
};

#endif /* __TNIRPS_SCRIPT_H__ */

