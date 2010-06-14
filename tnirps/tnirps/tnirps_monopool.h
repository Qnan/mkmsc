#ifndef __TNIRPS_MONOPOOL_H__
#define __TNIRPS_MONOPOOL_H__

#include "output.h"
#include "obj_pool.h"
#include "tnirps_hashset.h"
#include "tnirps_common.h"
#include "tnirps_var_map.h"
class Exception;
class VarMap;

typedef int Monomial;

class MonoPool {
public:
   struct Var {
      Var (int i, int d) : idx(i), deg(d) {}
      int idx;
      int deg;
   };

   int init (int* degs)
   {
      int tmp[len];
      memcpy(tmp+1, degs, dim * sizeof(int));
      _setDeg(tmp);
      return resolve(tmp);
   }

   void print (Output& output, Monomial id) const
   {
      bool first = true;
      int* data = _get(id);
      for (int i = 0; i < dim; ++i) {
         int deg = data[i+1];
         if (deg == 0)
            continue;
         if (!first)
            output.printf("*");
         first = false;
         output.printf("%s", _varMap.name(i));
         if (deg > 1)
            output.printf("^%i", deg);
      }
   }

   Monomial mul (Monomial a, Monomial b)
   {
      int tmp[len];
      _clear(tmp);
      int *da = _get(a), *db = _get(b);
      for (int i = 0; i < dim + 1; ++i)
         tmp[i] = da[i] + db[i];
      return resolve(tmp);
   }

   Monomial div (Monomial a, Monomial b)
   {
      int tmp[len];
      _clear(tmp);
      int *da = _get(a), *db = _get(b);
      for (int i = 0; i < dim + 1; ++i) {
         tmp[i] = da[i] - db[i];
         if (tmp[i] < 0)
            throw Exception("Monomes don't divide");
      }
      return resolve(tmp);
   }

   Monomial gcd (Monomial a, Monomial b)
   {
      int tmp[len];
      _clear(tmp);
      int *da = _get(a), *db = _get(b);
      for (int i = 1; i <= dim; ++i)
         tmp[i] = __min(da[i], db[i]);
      _setDeg(tmp);
      return resolve(tmp);
   }
   
   int countHash(int id) const {
      countHash(_get(id));
   }

   bool divides (Monomial a, Monomial b)
   {
      int *da = _get(a), *db = _get(b);
      for (int i = 0; i < dim + 1; ++i)
         if (da[i] < db[i])
            return false;
      return true;
   }

   bool equals (Monomial a, Monomial b)
   {
      int *da = _get(a), *db = _get(b);
      for (int i = 0; i < dim + 1; ++i)
         if (da[i] != db[i])
            return false;
      return true;
   }

   static int cmpLex (int* da, int* db, int dim) {
      for (int i = 0; i < dim; ++i) {
         int diff = da[i+1] - db[i+1];
         if (diff != 0)
            return diff;
      }
      return 0;      
   }

   static int cmpRevLex (int* da, int* db, int dim) {
      for (int i = dim - 1; i >= 0; --i) {
         int diff = da[i+1] - db[i+1];
         if (diff != 0)
            return diff;
      }
      return 0;
   }

   static int cmpDegRevLex (int* da, int* db, int dim) {
      int diff = da[0] - db[0];
      if (diff != 0)
         return diff;
      for (int i = dim - 1; i >= 0; --i) {
         int diff = db[i+1] - da[i+1];
         if (diff != 0)
            return diff;
      }
      return 0;
   }

   int* getDegs (Monomial m) const {
      return _get(m)+1;
   }

   int* getAll (Monomial m) const {
      return _get(m);
   }

   int deg (Monomial m, int i) const {
      return _get(m)[i+1];
   }

   int totalDeg (Monomial m) const {
      return _get(m)[0];
   }

   int isUnit (Monomial m) const {
      return _get(m)[0] == 0;
   }

   int nvars () const {
      return dim;
   }

   static MonoPool& inst () {
      return _inst;
   }
   enum ORDER {LEX, DRL};
   typedef int (*mcmp_t) (int* da, int* db, int dim);

   void setOrder (ORDER o) {
      _order = o;
      _cmp = getCmp(o);
   }

   ORDER getOrder () {
      return _order;
   }

   mcmp_t getCmp (ORDER o) const {
      if (o == LEX)
         return cmpLex;
      else if (o == DRL)
         return cmpDegRevLex;
      else
         throw Exception("Order unknown");
   }
   Monomial unit () {
      return _munit;
   }

   Monomial single (int var) {
      return _msingle[var];
   }

   Monomial init (const Array<int>& vars, const Array<int>& degs) {
      int tmp[len];
      _clear(tmp);
      for (int i = 0; i < vars.size(); ++i)
         tmp[vars[i]+1] = degs[i];
      _setDeg(tmp);
      return resolve(tmp);
   }

   Monomial init (const char* expr, int begin = 0, int end = 0) {
      static Array<int> vars, degs;

      if (end <= 0)
         end = strlen(expr);
      BufferScanner sc(expr + begin, end - begin);
      static Array<char> buf;
      vars.clear();
      degs.clear();
      while (!sc.isEOF()) {
         sc.skipSpace();
         sc.readWord(buf, "*^");
         buf.rstrip();
         if (strlen(buf.ptr()) == 0)
            throw Exception("Error parsing expression %s, starting %s", expr, expr+begin);
         buf.push(0);
         int id = _varMap.id(buf.ptr());

         int deg = 1;
         char c;
         if (!sc.isEOF()) {
            c = sc.lookNext();
            if (c == '^') {
               sc.readChar();
               sc.skipSpace();
               deg = sc.readInt();
               if (deg <= 0)
                  throw Exception("Degree must be strictly positive");
            }
            sc.skipSpace();
         }
         sc.skipSpace();
         if (!sc.isEOF()) {
            c = sc.readChar();
            if (c != '*')
               throw Exception("Invalid monomial string, expected \"*\"");
         }
         vars.push(id);
         degs.push(deg);
      }
      return init(vars, degs);
   }

   void alloc (Monomial id) {
      refInc(id);
   }

   void release (Monomial id) {
      refDec(id);
   }

   void toStr(Array<char>& buf, Monomial id) const {
      ArrayOutput output(buf);
      print(output, id);
      output.writeByte(0);
   }

   void print(Output& output, Monomial id, int coeff) const {
      bool empty = totalDeg(id) == 0;
      if (coeff != 1 || empty)
         output.printf(empty ? "%i" : "%i*", coeff);
      print(output, id);
   }

   int cmp (Monomial id1, Monomial id2, ORDER o) const {
      return getCmp(o)(_get(id1), _get(id2), dim);
   }

   int cmp (Monomial id1, Monomial id2) const {
      return _cmp(_get(id1), _get(id2), dim);
   }

   int resolve (int* data) {
      return _uniq.findOrAdd(data, countHash(data));
   }

   int refInc (int id) {
      return ++refcnt.findOrInsert(id, 0);
   }

   int refDec (int id) {
      int& v = refcnt.at(id);
      if (v <= 0)
         throw Exception("Reference counter less than zero");
      return --v;
   }

   int checkLeaks (bool printem) {
      int total = 0;
      bool none = true;
      printf("\n\nLeaks:\n");
      for (int i = refcnt.begin(); i < refcnt.end(); i = refcnt.next(i)) {
         int rc = refcnt.value(i);
         if (rc == 0)
            continue;
         none = false;
         total += rc;
         if (printem)
            print(sout, refcnt.key(i), 1),printf(": %d\n", rc);
      }
      if (none)
         printf("\tnone.\n");
   }

   int printRefs () {
      printf("\n\nReferences:\n");
      for (int i = refcnt.begin(); i < refcnt.end(); i = refcnt.next(i)) {
         print(sout, refcnt.key(i), 1),printf(": %d\n", refcnt.value(i));
      }
   }

//   int collectUnused () {
//      for (int i = refcnt.begin(); i < refcnt.end(); i = refcnt.next(i)) {
//         if (refcnt.value(i) == 0) {
//            int id = refcnt.key(i);
//            _uniq.remove(id, _pool.at(id).countHash());
//            _pool.remove(id);
//         }
//      }
//   }

   void setVarMap (const char* vars) {
      if (dim > 0)
         throw Exception("Already initialized. Reset first.");
      _varMap.set(vars);
      _init(_varMap.size());

   }

   const VarMap& getVarMap () {
      return _varMap;
   }

   void reset () {
      _uniq.map.clear();
      _uniq.context = NULL;
      _uniq.eq = NULL;
      _uniq.alloc = NULL;

      _munit = -1;
      _msingle.clear();

      if (memsz > 0)
         free(mem);
      mem = NULL;
      memsz = 0;

      dim = -1;
   }
private:
   int dim, sz, len;
   const int initialSize;

   int* mem;
   int memsz, memptr;
   int push () {
      if (memsz == memptr) {
         int cnt = (memsz == 0) ? initialSize : 2 * memsz;
         int* t = (int*)malloc(sz * cnt);
         if (memsz > 0) {
            memcpy(t, mem, memsz * sz);
            free(mem);
         }
         mem = t;
         memsz = cnt;
      }
      return memptr++;
   }
   int pop () {
      return memptr--;
   }

   int * _get(int id) const {
      return mem + id * len;
   }

   void _clear(int* data) const {
      memset(data, 0, sz);
   }

   void _copy(int* dst, int* src) const {
      memcpy(dst, src, sz);
   }

   void _setDeg(int* data) const {
      data[0] = 0;
      for (int i = 0; i < dim; ++i)
         data[0] += data[i+1];
   }

   int countHash(int* data) const
   {
      static long k[]={0,4,8,12,16,20,24,28};
      static int a = 0x7;
      int hash = 0;
      for(int i = 0; i < dim; ++i)
         hash ^= data[i+1] << k[i & a];
      return hash;
   }

   static int cb_cmp (int a, int* data, void* context) {
      MonoPool& mp = *(MonoPool*)context;
      return mp.cmpDegRevLex(mp._get(a), data, mp.dim);
   }
   static int cb_alloc (int* data, void* context) {
      MonoPool& mp = *(MonoPool*)context;
      int id = mp.push();
      mp._copy(mp._get(id), data);
      return id;
   }

   HashSet _uniq;
   RedBlackMap<int, int> refcnt;
   mcmp_t _cmp;
   ORDER _order;
   VarMap _varMap;
   static MonoPool _inst;
   Monomial _munit;
   Array<Monomial> _msingle;

   void _init (int d) {
      dim = d;
      len = dim + 1;
      sz = sizeof(int) * (dim+1);

      _uniq.context = this;
      _uniq.eq = cb_cmp;
      _uniq.alloc = cb_alloc;
      memptr = 0;

      int tmp[len];
      _clear(tmp);
      _munit = resolve(tmp);
      _msingle.clear();
      _msingle.resize(dim);
      for (int i = 0; i < dim; ++i) {
         _clear(tmp);
         tmp[0] = tmp[i+1] = 1;
         _msingle[i] = resolve(tmp);
      }
   }

   MonoPool () : memsz(0), initialSize(1048576) {
      reset();
   }
   MonoPool (const MonoPool&);
};

extern MonoPool& MP; // global!

class MonoPtr {
public:
   MonoPtr () : _m(-1) {
   }
   MonoPtr (Monomial m) : _m(-1) {
      set(m);
   }
   void reset () {
      set(-1);
   }
   void set (Monomial m) {
      if (_m >= 0)
         MP.release(_m);
      if (m >= 0)
         MP.alloc(m);
      _m = m;
   }
   Monomial get () const {
      return _m;
   }
   ~MonoPtr() {
      set(-1);
   }
private:
   Monomial _m;
   MonoPtr (const MonoPtr& mp);
};

#endif //__TNIRPS_MONOPOOL_H__