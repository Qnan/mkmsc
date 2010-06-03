/****************************************************************************
 * Copyright (C) 2009-2010 SciTouch LLC
 * 
 * This file is part of Indigo toolkit.
 * 
 * This file may be distributed and/or modified under the terms of the
 * GNU General Public License version 3 as published by the Free Software
 * Foundation and appearing in the file LICENSE.GPL included in the
 * packaging of this file.
 * 
 * This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 * WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 ***************************************************************************/

#ifndef __objarray_h__
#define __objarray_h__

#include "base_cpp/array.h"

template <typename T> class ObjArray
{
public:
   explicit ObjArray ()
   {
   }

   ~ObjArray ()
   {
      while (size() > 0)
         pop();
   }

   const T & operator[] (int index) const
   {
      return _array[index];
   }

   T & operator[] (int index)
   {
      return _array[index];
   }

   const T & at (int index) const
   {
      return (*this)[index];
   }

   T & at (int index)
   {
      return (*this)[index];
   }

   int size (void) const { return _array.size(); }

   T & push ()
   {
      void *addr = &_array.push();

      new (addr) T();

      return _array.top();
   }

   template <typename A> T & push (A &a)
   {
      void *addr = &_array.push();

      new (addr) T(a);

      return _array.top();
   }

   template <typename A, typename B> T & push (const A &a, const B &b)
   {
      void *addr = &_array.push();

      new (addr) T(a, b);

      return _array.top();
   }

   void clear ()
   {
      while (size() > 0)
         pop();
   }

   void clearObjects ()
   {
      for (int i = 0; i < _array.size(); i++)
         _array[i].clear();
   }

   void resize (int newSize)
   {
      while  (newSize < size()) {
         pop();
      }

      while  (newSize > size()) {
         push();
      }
   }
   void reserve (int size)
   {
      _array.reserve(size);
   }

   void expand (int newSize)
   {
      while  (newSize > size()) {
         push();
      }
   }

   T & top ()
   {
      return _array.top();
   }

   const T & top () const
   {
      return _array.top();
   }

   void pop ()
   {
      _array.top().~T();
      _array.pop();
   }

   void qsort (int (*cmp)(const T &, const T &, const void *), const void *context)
   {
      _array.qsort(cmp, context);
   }
   
protected:

   Array<T> _array;
private:
   ObjArray (const ObjArray &); // no implicit copy
};

#endif
