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

#ifndef __exception_h__
#define __exception_h__

#include <stdarg.h>

#include "base_c/defs.h"

class DLLEXPORT Exception
{
public:
   explicit Exception (const char *format, ...);
   virtual ~Exception ();

   int code () { return _code; }
   const char * message () { return _message; }

   virtual Exception * clone      ();
   virtual void        throwSelf  ();

protected:
   explicit Exception ();

   void _init (const char *format, va_list args);
   void _init (const char *prefix, const char *format, va_list args);

   void _cloneTo (Exception *dest);

   int  _code;
   char _message[1024];
};

#define DEF_ERROR(error_prefix) \
   class Error : public Exception                     \
   {                                                  \
   public:                                            \
      explicit Error (const char *format, ...)        \
      {                                               \
         va_list args;                                \
                                                      \
         va_start(args, format);                      \
         _init(error_prefix, format, args);           \
         va_end(args);                                \
      }                                               \
                                                      \
      virtual ~Error () {}                            \
      virtual Exception* clone ()                     \
      {                                               \
         Error *error = new Error("");                \
         _cloneTo(error);                             \
         return error;                                \
      }                                               \
      virtual void throwSelf ()                       \
      {                                               \
         throw *this;                                 \
      }                                               \
   }


#endif // __exception_h__
