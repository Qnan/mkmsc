#ifndef __TNIRPS_REDUCTION_H__
#define __TNIRPS_REDUCTION_H__

class SimpleReductor {
public:
   SimpleReductor (const ObjArray<Polynomial>& gb) : g(gb) {
   }

   bool reduceStep (Polynomial& r, const Polynomial& p);
private:
   const ObjArray<Polynomial>& g;
   SimpleReductor (const SimpleReductor&);
};

#endif /* __TNIRPS_REDUCTION_H__ */