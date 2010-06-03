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

#ifndef __ptr_array__
#define __ptr_array__

#include "base_cpp/array.h"

template <typename T> struct DLLEXPORT PtrArray
{
public:
   explicit PtrArray ()
   {
   }

   virtual ~PtrArray ()
   {
      clear();
   }

   T & add (T *obj)
   {
      _ptrarray.push(obj);
      return *obj;
   }

   T * pop (void)
   {
      return _ptrarray.pop();
   }

   T * top (void)
   {
      return at(size() - 1);
   }

   void expand (int newsize)
   {
      while (_ptrarray.size() < newsize)
         _ptrarray.push(0);
   }

   void clear (void)
   {
      int i;

      for (i = 0; i < _ptrarray.size(); i++)
      {
         if (_ptrarray[i] == 0)
            continue;

         delete _ptrarray[i];
         _ptrarray[i] = 0;
      }
      _ptrarray.clear();
   }

   int size () const { return _ptrarray.size(); }

   void resize (const int newsize)
   {                             
      for (int i = newsize; i < size(); i++)
      {
         if (_ptrarray[i] == 0)
            continue;

         delete _ptrarray[i];
         _ptrarray[i] = 0;         
      }
      _ptrarray.resize(newsize);
   }

   void remove (int idx)
   {
      delete _ptrarray[idx];

      _ptrarray.remove(idx);
   }

   void qsort (int (*cmp)(T * const &, T * const &, const void *), const void *context)
   {
      _ptrarray.qsort(cmp, context);
   }

   
   const T *  operator[] (int index) const { return _ptrarray[index]; }
         T *& operator[] (int index)       { return _ptrarray[index]; }

   const T *  at (int index) const { return _ptrarray[index]; }
         T *& at (int index)       { return _ptrarray[index]; }

   const T * const* ptr () const { return _ptrarray.ptr(); }
               T ** ptr ()       { return _ptrarray.ptr(); }

protected:
   Array<T *> _ptrarray;
private:
   PtrArray (const PtrArray &); // no implicit copy
};

#endif
