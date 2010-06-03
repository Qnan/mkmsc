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

#ifndef __string_pool_h__
#define __string_pool_h__

#include "base_cpp/pool.h"

class DLLEXPORT StringPool
{
public:

   StringPool () {}
   ~StringPool () {}

   int  add (const char *str);
   int  add (int size);
   void remove (int idx);
   int  size () const;
   int  begin () const;
   int  end () const;
   int  next (int i) const;
   void clear ();

   char * at (int idx);
   const char * at (int idx) const;

protected:
   DEF_ERROR("string pool");

   struct Desc
   {
      int start;
      int length; // including the trailing zero
   };

   Pool<Desc>  _pool;
   Array<char> _storage;

private:
   StringPool (const StringPool &); // no implicit copy
};


#endif
