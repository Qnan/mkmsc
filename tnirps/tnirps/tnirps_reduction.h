class SimpleReductor {
public:
   SimpleReductor (const ObjArray<Polynomial>& gb) : g(gb) {
   }

   bool reduceStep (Polynomial& r, const Polynomial& p);
private:
   const ObjArray<Polynomial>& g;
   SimpleReductor (const SimpleReductor&);
};

