#ifndef __TNIRPS_POLY_PRINTER_H__
#define __TNIRPS_POLY_PRINTER_H__

#include "obj_array.h"
#include "output.h"
#include "tnirps_scheme.h"
#include "tnirps_monopool.h"
#include "tnirps_bigint.h"

class Printer {
public:
   Printer () {}

   void evaluate (Output* output, const Scheme& scheme) {
      _output = output;
      SCHEME_CALLBACKS_SET(scheme);
      scheme.proceed(this);
   }

   SCHEME_CALLBACKS_DEFINE(Printer);

private:
   void printMonomial (const Array<Monomial>& mm, int i) {
      ArrayOutput output(intermediateValues[i]);
      MP.print(output, mm[i], 1);
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
   void mulnum (int id, int a, const Cf& num) {
      ArrayOutput output(intermediateValues[id]);
      Ring::print(output, num);
      output.printf(" * %s", intermediateValues[a].ptr());
      output.writeChar(0);
   }
   void yield (int id) {
      _output->printf("%s", intermediateValues[id].ptr());
   }

   Output *_output;
   ObjArray< Array<char> > intermediateValues;
};


#endif /* __TNIRPS_POLY_PRINTER_H__ */

