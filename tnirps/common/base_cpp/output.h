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

#ifndef __output_h__
#define __output_h__

#include <stdio.h>

#include "base_cpp/array.h"

class Output
{
public:
   DEF_ERROR("output");

   explicit Output ();
   virtual ~Output ();

   virtual void write (const void *data, int size) = 0;
   virtual void seek  (int offset, int from) = 0;
   virtual int  tell  () = 0;
   virtual void flush () = 0;

   void writeByte        (byte value);
   void writeChar        (char value);
   void writeBinaryInt   (int   value);
   void writeBinaryDword (dword value);
   void writeBinaryWord  (word value);
   void writeBinaryFloat (float value);
   void writePackedShort (short value);
   void writeString      (const char *string);
   void writeStringCR    (const char *string);
   void writeCR          ();
   void writeArray       (const Array<char> &data);
   
   void skip             (int count);

   void printf   (const char *format, ...);
   void vprintf  (const char *format, va_list args);
   void printfCR (const char *format, ...);
};

class FileOutput : public Output
{
public:
   explicit FileOutput (const char *format, ...);
   explicit FileOutput (bool append, const char *format, ...);
   virtual ~FileOutput ();

   virtual void write (const void *data, int size);
   virtual void seek  (int offset, int from);
   virtual int  tell  ();
   virtual void flush ();
   
protected:
   FILE *_file;
};

class ArrayOutput : public Output
{
public:
   explicit ArrayOutput (Array<char> &arr);
   virtual ~ArrayOutput ();

   virtual void write (const void *data, int size);
   virtual void seek  (int offset, int from);
   virtual int  tell  ();
   virtual void flush ();

   void clear ();

protected:
   Array<char> &_arr;
};

class StandardOutput : public Output
{
public:
   explicit StandardOutput ();
   virtual ~StandardOutput ();

   virtual void write (const void *data, int size);
   virtual void seek  (int offset, int from);
   virtual int  tell  ();
   virtual void flush ();
protected:
   int _count;
};

void bprintf (Array<char>& buf, const char *format, ...);

#endif
