#ifndef __TNIRPS_POLY_PRINTER_H_
#define __TNIRPS_POLY_PRINTER_H_

#include "obj_array.h"
#include "output.h"
#include "tnirps_scheme.h"
#include "tnirps_monopool.h"
#include "tnirps_bigint.h"

class Printer {
public:
   Printer () {}

   void evaluate (const Scheme& scheme) {
      SCHEME_CALLBACKS_SET(scheme);
      scheme.proceed(this);
   }

   SCHEME_CALLBACKS_DEFINE(Printer);

private:
   void printMonomial (const Array<Monomial>& mm, int i) {
      const MData& m = MP.get(mm[i]);
      ArrayOutput output(intermediateValues[i]);
      if (m.length() == 0)
         output.printf("1");
      for (int i = 0; i < m.length(); ++i) {
         if (i != 0)
            output.printf(" * ");
         output.printf("%s", MP.varName(m.var(i)));
         if (m.deg(i) > 1)
            output.printf("^%i", m.deg(i));
      }
      output.writeChar(0);
   }

   void init (const Array<Monomial>& mm, int total) {
      intermediateValues.clear();
      intermediateValues.resize(total);

      for (int i = 0; i < mm.size(); ++i)
         printMonomial(mm, i);
   }
   void add (int id, int a, int b) {
      ArrayOutput output(intermediateValues[id]);
      output.printf("(%s + %s)", intermediateValues[a].ptr(), intermediateValues[b].ptr());
      output.writeChar(0);
   }
   void mul (int id, int a, int b) {
      ArrayOutput output(intermediateValues[id]);
      output.printf("%s * %s", intermediateValues[a].ptr(), intermediateValues[b].ptr());
      output.writeChar(0);
   }
   void mulnum (int id, int a, int num) {
      ArrayOutput output(intermediateValues[id]);
      output.printf("%i * %s", num, intermediateValues[a].ptr());
      output.writeChar(0);
   }
   void yield (int id) {
      printf("%s", intermediateValues[id].ptr());
   }

//   Array<int> varValues;
   ObjArray< Array<char> > intermediateValues;
};


#endif /* __TNIRPS_POLY_EVALUATOR__ */

