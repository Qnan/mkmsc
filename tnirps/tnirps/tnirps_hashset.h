#ifndef __TNIRPS_HASHSET_H__
#define	__TNIRPS_HASHSET_H__

#include "base_cpp/red_black.h"
#include "base_cpp/list.h"

typedef unsigned long HASHTYPE;

template<typename T>
class HashSet {
public:
   HashSet () {}

   int (*eq) (int a, int b, void* context);

   int findOrAdd (int a, HASHTYPE hash, void *context) {
      Array<int>& arr = map.findOrInsert(hash);
      for (int i = 0; i < arr.size(); ++i)
         if (eq(a, arr[i], context) == 0)
            return arr[i];
      arr.push(a);
      return a;
   }

   RedBlackObjMap<HASHTYPE, Array<int> > map;
   //Pool<List<int>::Elem> listPool;
};


#endif	/* __TNIRPS_HASHSET_H__ */

