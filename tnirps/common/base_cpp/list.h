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

#ifndef __list_h__
#define __list_h__

#include "base_cpp/pool.h"

template <typename T> class DLLEXPORT List
{
public:
   struct Elem
   {
      int prev;
      int next;
      T   item;
   };

   explicit List () : _pool(new Pool<Elem>), _size(0), _head(-1), _tail(-1), _own_pool(true)
   {
   }
   
   explicit List (Pool<Elem> &pool) : _pool(&pool), _size(0), _head(-1), _tail(-1), _own_pool(false)
   {
   }

   ~List ()
   {
      clear();
      if (_own_pool)
         delete _pool;
   }

   void clearAndResetPool (Pool<Elem> &pool)
   {
      clear();
      if (_own_pool)
         delete _pool;
      _own_pool = false;
      _pool = &pool;
   }

   int add ()
   {
      if (_size == 0)
      {
         _head = _pool->add();
         _tail = _head;

         Elem &elem = _pool->at(_head);

         elem.prev = -1;
         elem.next = -1;
      }
      else
      {
         int idx = _pool->add();
         Elem &elem = _pool->at(idx);

         _pool->at(_tail).next = idx;
         elem.prev = _tail;
         elem.next = -1;
         _tail = idx;
      }

      _size++;
      return _tail;
   }

   int add (const T &item) 
   {
      int idx = add();

      _pool->at(idx).item = item;
      return idx;
   }

   void set (const Array<int>& ids)
   {
      int sz = ids.size();
      for (int i = 0; i < ids.size(); ++i) {
         Elem &elem = _pool->at(ids[i]);
         elem.next = i < sz - 1 ? ids[i+1] : -1;
         elem.prev = i > 0 ? ids[i-1] : -1;
      }
      _head = ids[0];
      _tail = ids[sz - 1];
   }

   int insertAfter (int existing)
   {
      _pool->at(existing); // will throw if the element does not exist

      int  idx = _pool->add();
      Elem &ex = _pool->at(existing);
      Elem &elem = _pool->at(idx);

      elem.next = ex.next;
      elem.prev = existing;
      ex.next = idx;

      if (elem.next != -1)
         _pool->at(elem.next).prev = idx;

      if (_tail == existing)
         _tail = idx;

      _size++;
      return idx;
   }

   int insertBefore (int existing)
   {
      _pool->at(existing); // will throw if the element does not exist
      
      int  idx = _pool->add();
      Elem &ex = _pool->at(existing);
      Elem &elem = _pool->at(idx);

      elem.prev = ex.prev;
      elem.next = existing;
      ex.prev = idx;

      if (elem.prev != -1)
         _pool->at(elem.prev).next = idx;

      if (_head == existing)
         _head = idx;

      _size++;
      return idx;
   }

   void remove (int idx)
   {
      Elem &elem = _pool->at(idx);

      if (elem.prev != -1)
         _pool->at(elem.prev).next = elem.next;
      else
         _head = elem.next;

      if (elem.next != -1)
         _pool->at(elem.next).prev = elem.prev;
      else
         _tail = elem.prev;

      _pool->remove(idx);
      _size--;
   }

   int size () const
   {
      return _size;
   }

   int begin () const
   {
      if (_head == -1)
         return _pool->end();

      return _head;
   }

   int end () const
   {
      return _pool->end();
   }

   int next (int idx) const
   {
      int res = _pool->at(idx).next;

      if (res == -1)
         return _pool->end();

      return res;
   }

   void swap (int i) // swap with next
   {
      Elem& a = _pool->at(i);
      int j = a.next;
      Elem& b = _pool->at(j);
      a.next = b.next;
      b.prev = a.prev;
      (a.prev >= 0 ? _pool->at(a.prev).next : _head) = j;
      (b.next >= 0 ? _pool->at(b.next).prev : _tail) = i;
      b.next = i;
      a.prev = j;
   }

   void bubbleSort (int (*cmp)(const T& a, const T& b))
   {
      for (int i = 0; i < size(); ++i)
         for (int j = begin(), k; (k = next(j)) < end();)
            if (cmp(_pool->at(j).item, _pool->at(k).item) < 0)
               swap(j);
            else
               j = k;
   }

   int prev (int idx) const
   {
      return _pool->at(idx).prev;
   }

   int tail () const
   {
      return _tail;
   }

   void clear ()
   {
      if (_own_pool)
         _pool->clear();
      // or there may be other lists using the same _pool
      else while (_tail != -1)
      {
         int iter = _tail;

         _tail = _pool->at(iter).prev;
         _pool->remove(iter);
      }
      
      _size = 0;
      _head = -1;
      _tail = -1;
   }

   T & operator [] (int index) const
   {                        
      return _pool->at(index).item;
   }

   T & at (int index) const
   {                        
      return _pool->at(index).item;
   }

protected:
   Pool<Elem> *_pool; // the element pool may be shared amongst lists
   int         _size;
   int         _head;
   int         _tail;
   bool        _own_pool;

private:

   List (const List<T> & ); // no implicit copy
};

#endif
