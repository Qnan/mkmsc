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

#ifndef __array_h__
#define __array_h__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>

#include "base_c/defs.h"
#include "base_cpp/exception.h"

template <typename T> class DLLEXPORT Array
{
public:
   DEF_ERROR("array");

   explicit Array ()
   {
      _reserved = 0;
      _length = 0;
      _array = NULL;
   }
   
   ~Array ()
   {
      if (_array != NULL)
      {
         free(_array);
         _array = NULL;
      }
   }

   void clear ()
   {
      _length = 0;
   }

   void reserve (int to_reserve)
   {
      if (to_reserve <= 0)
         throw Error("to_reserve = %d", to_reserve);

      if (to_reserve > _reserved)
      {
         if (_length < 1)
         {
            free(_array);
            _array = NULL;
         }

         T *oldptr = _array;

         _array = (T *)realloc(_array, sizeof(T) * to_reserve);
         if (_array == NULL)
         {
            _array = oldptr;
            throw Error("reserve(): no memory");
         }
         _reserved = to_reserve;
      }
   }

   void zerofill ()
   {
      if (_length > 0)
         memset(_array, 0, _length * sizeof(T));
   }

   void fffill ()
   {
      if (_length > 0)
         memset(_array, 0xFF, _length * sizeof(T));
   }

   const T * ptr () const
   {
      return _array;
   }

   T * ptr ()
   {
      return _array;
   }

   const T & operator [] (int index) const
   {                        
      if (index < 0 || _length - index <= 0)
         throw Error("invalid index %d (size=%d)", index, _length);

      return _array[index];
   }
   
   T & operator [] (int index)
   {                        
      if (index < 0 || _length - index <= 0)
         throw Error("invalid index %d (size=%d)", index, _length);
   
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

   int size        (void) const { return _length; }
   int sizeInBytes (void) const { return _length * sizeof(T); }

   void copy (const Array<T> &other)
   {
      copy(other._array, other._length);
   }

   void copy (const T *other, int count)
   {
      if (count > 0)
      {
         clear_resize(count);
         memcpy(_array, other, count * sizeof(T));
      }
      else
         _length = 0;
   }

   void concat (const Array<T> &other)
   {
      concat(other._array, other.size());
   }

   void concat (const T *other, int count)
   {
      if (count > 0)
      {
         int length = _length;
         resize(length + count);

         memcpy(_array + length, other, count * sizeof(T));
      }
   }

   int memcmp (const Array<T> &other) const
   {
      if (_length < other._length)
         return -1;
      if (_length > other._length)
         return -1;

      if (_length == 0)
         return 0;

      return ::memcmp(_array, other._array, _length * sizeof(T));
   }

   void remove (int idx)
   {
      if (idx < 0 || idx >= _length)
         throw Error("remove(): invalid index %d (size=%d)", idx, _length);

      if (idx < _length - 1)
         memmove(_array + idx, _array + idx + 1, sizeof(T) * (_length - idx - 1));

      _length--;
   }

   void remove_replace (int idx)
   {
      if (idx < 0 || idx >= _length)
         throw Error("remove_replace(): invalid index %d (size=%d)", idx, _length);

      if (idx < _length - 1)
         _array[idx] = _array[_length - 1];

      _length--;
   }

   int find (const T &value) const
   {
      for (int i = 0; i < _length; i++)
         if (_array[i] == value)
            return i;

       return -1;
   }

   void swap (int idx1, int idx2)
   {
      if (idx1 < 0 || idx1 >= _length)
         throw Error("swap(): invalid index %d (size=%d)", idx1, _length);

      if (idx2 < 0 || idx2 >= _length)
         throw Error("swap(): invalid index %d (size=%d)", idx2, _length);

      if (idx1 == idx2)
         return;

      T tmp;

      memcpy(&tmp, _array + idx1, sizeof(T));
      memcpy(_array + idx1, _array + idx2, sizeof(T));
      memcpy(_array + idx2, &tmp, sizeof(T));
   }

   void push (T elem)
   {
      resize(_length + 1);
      _array[_length - 1] = elem;
   }

   T & push ()
   {
      resize(_length + 1);
      return _array[_length - 1];
   }

   T & pop ()
   {
      if (_length <= 0)
         throw Error("stack underflow");

      return _array[--_length];
   }

   T & top ()
   {
      if (_length <= 0)
         throw Error("stack underflow");

      return _array[_length - 1];
   }

   const T & top () const
   {
      if (_length <= 0)
         throw Error("stack underflow");

      return _array[_length - 1];
   }

   void resize (int newsize)
   {
      if (newsize > _reserved)
         reserve((newsize + 1) * 2);
      _length = newsize;
   }

   void expand (int newsize)
   {
      if (_length < newsize)
         resize(newsize);
   }

   void clear_resize (int newsize)
   {
      if (_reserved < newsize)
      {
         _length = 0;
         reserve((newsize + 1) * 2);
      }
      _length = newsize;
   }

   void swap (Array<T> &other)
   {
      T *tmp_t;
      __swap(_array, other._array, tmp_t);
      int tmp_int;
      __swap(_reserved, other._reserved, tmp_int);
      __swap(_length, other._length, tmp_int);
   }

   void qsort (int (*cmp)(const T &, const T &, const void *), const void *context)
   {
      qsort(0, _length - 1, cmp, context);
   }

   void qsort (int start, int end, int (*cmp)(const T &, const T &, const void *), const void *context)
   {
      // Sort elements from start to end
      if (start >= end)
         return;

      struct
      {
         T *lo, *hi;
      } stack[32], *sp;

      char tmp[sizeof(T)]; // can't use T directly because it may have destructor

      sp = stack;

      // push our initial values onto the stack
      sp->lo = _array + start;
      sp->hi = _array + end + 1;
      sp++;

      while (sp > stack)
      {
         // pop lo and hi off the stack
         sp--;
         T *high = sp->hi, *low = sp->lo;
         T *hi = high - 1;
         T *lo = low;
         T *pivot = low;

         while (1)
         {
            while (lo < high && lo != pivot && cmp(*lo, *pivot, context) < 0)
               lo++;

            while (hi > low && (hi == pivot || cmp(*hi, *pivot, context) >= 0))
               hi--;

            if (lo < hi)
            {
               memcpy(&tmp, lo, sizeof(T));
               memcpy(lo, hi, sizeof(T));
               memcpy(hi, &tmp, sizeof(T));

               if (lo == pivot)
                  pivot = hi;
               else if (hi == pivot)
                  pivot = lo;

               hi--;
            }
            else
            {
               hi++;

               if (hi == high)
                  // done with this segment
                  break;

               // push the larger segment onto the stack and continue
               // sorting the smaller segment.
               if ((hi - low) > (high - hi))
               {
                  sp->lo = low;
                  sp->hi = hi;
                  sp++;

                  hi = high;
                  low = lo;
               }
               else
               {
                  sp->hi = high;
                  sp->lo = hi;
                  sp++;

                  high = hi;
                  lo = low;
               }

               pivot = lo;
               hi--;
            }
         }
      }
   }

   // Array<char>-specific

   void appendString (const char *str, bool keep_zero)
   {
      int len = (int)strlen(str);
      int initial_size = _length;

      if (initial_size > 0 && _array[initial_size - 1] == 0)
         initial_size--;

      resize(initial_size + len);
      memcpy(_array + initial_size, str, len);

      if (keep_zero)
         push(0);
   }

   void readString (const char *str, bool keep_zero)
   {
      clear();
      appendString(str, keep_zero);
   }

   void upper (const char *source)
   {
      clear();
      while (*source != 0)
         push(::toupper(*source++));
      push(0);
   }

   void lower (const char *source)
   {
      clear();
      while (*source != 0)
         push(::tolower(*source++));
      push(0);
   }

   void toupper ()
   {
      for (int i = 0; i < _length; i++)
         _array[i] = ::toupper(_array[i]);
   }

   void tolower ()
   {
      for (int i = 0; i < _length; i++)
         _array[i] = ::tolower(_array[i]);
   }

   void rstrip ()
   {
      if (size() > 0 && top() == 0)
         pop();
      while (size() > 0 && isspace(top()))
         pop();
      push(0);
   }

protected:
   T  *_array;

   int _reserved;
   int _length;

private:
   Array (const Array &); // no implicit copy
};

// operators defined here for use with ObjArray<> and ObjPool<>
template <typename T> void * operator new (size_t size, T *allocated_area)
{
  return allocated_area;
}

template <typename T> void operator delete (void *ptr, T *allocated_area)
{
}

#endif
