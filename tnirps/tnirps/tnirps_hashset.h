#ifndef __TNIRPS_HASHSET_H__
#define	__TNIRPS_HASHSET_H__

#include "base_cpp/red_black.h"
#include "base_cpp/list.h"

typedef unsigned long HASHTYPE;

// This class is to identify whether two items are similiar or distinct
//    by means of a hash value and a comparsion function provided.
class HashSet {
public:
   HashSet () {}

   int (*eq) (int id, int* data, void* context);
   int (*alloc) (int* data, void* context);
   void* context;

   int findOrAdd (int* data, HASHTYPE hash) {
      Array<int>& arr = map.findOrInsert(hash);
      for (int i = 0; i < arr.size(); ++i)
         if (eq(arr[i], data, context) == 0)
            return arr[i];
      int a = alloc(data, context);
      arr.push(a);
      return a;
   }

   RedBlackObjMap<HASHTYPE, Array<int> > map;
};


#endif	/* __TNIRPS_HASHSET_H__ */

