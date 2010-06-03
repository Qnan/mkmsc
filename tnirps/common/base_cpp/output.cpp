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

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdarg.h>

#include "base_c/defs.h"
#include "base_cpp/output.h"
#include "base_cpp/array.h"
#include "base_cpp/tlscont.h"

Output::Output ()
{
}

Output::~Output ()
{
}

void Output::writeBinaryInt (int value)
{
   //value = htonl(value);
   write(&value, sizeof(int));
}

void Output::writeBinaryDword (dword value)
{
   write(&value, sizeof(dword));
}

void Output::writeBinaryFloat (float value)
{
   write(&value, sizeof(float));
}

void Output::writeByte (byte value)
{
   write(&value, 1);
}

void Output::writeChar (char value)
{
   write(&value, 1);
}

void Output::writeBinaryWord (word value)
{
   //value = htons(value);
   write(&value, sizeof(word));
}

void Output::printf (const char *format, ...)
{
   va_list args;
  
   va_start(args, format);
   vprintf(format, args);
   va_end(args);
}

void Output::vprintf (const char *format, va_list args)
{
   QS_DEF(Array<char>, str);
   if (str.size() < 2048)
      str.resize(2048);

   int n;
   while ((n = vsnprintf(str.ptr(), str.size(), format, args)) < 0)
      str.resize(str.size() * 2);

   write(str.ptr(), n);
}

void Output::writeArray (const Array<char> &data)
{
   write(data.ptr(), data.size());
}

void Output::writeCR ()
{
   writeChar('\n');
}

void Output::printfCR (const char *format, ...)
{
   va_list args;
  
   va_start(args, format);
   vprintf(format, args);
   va_end(args);

   writeCR();
}

void Output::writeString (const char *string)
{
   int n = (int)strlen(string);

   write(string, n);
}

void Output::writeStringCR (const char *string)
{
   writeString(string);
   writeCR();
}

void Output::writePackedShort (short value)
{
   byte low = value & 255;
   byte high = (value - low) >> 8;

   if (value < 128)
      writeByte(low);
   else
   {
      writeByte(high + 128);
      writeByte(low);
   }
}

void Output::skip (int count)
{
   seek(count, SEEK_CUR);
}

FileOutput::FileOutput (const char *format, ...)
{
   char filename[1024];

   va_list args;

   va_start(args, format);
   vsnprintf(filename, sizeof(filename), format, args);
   va_end(args);

   _file = fopen(filename, "wb");

   if (_file == NULL)
      throw Error("can't open file %s", filename);
}

FileOutput::FileOutput (bool append, const char *format, ...)
{
   char filename[1024];

   va_list args;

   va_start(args, format);
   vsnprintf(filename, sizeof(filename), format, args);
   va_end(args);

   if (append)
      _file = fopen(filename, "ab+");
   else
      _file = fopen(filename, "wb");

   if (_file == NULL)
      throw Error("can't open file %s", filename);
}

FileOutput::~FileOutput ()
{
   fclose(_file);
}

void FileOutput::write (const void *data, int size)
{
   if (size < 1)
      return;

   size_t res = fwrite(data, size, 1, _file);

   if (res != 1)
      throw Error("file write error in write()");
}

void FileOutput::flush ()
{
   fflush(_file);
}

void FileOutput::seek (int offset, int from)
{
   fseek(_file, offset, from);
}

int FileOutput::tell ()
{
   return ftell(_file);
}

ArrayOutput::ArrayOutput (Array<char> &arr) : _arr(arr)
{
   _arr.clear();
}

ArrayOutput::~ArrayOutput ()
{
}

void ArrayOutput::write (const void *data, int size)
{
   int old_size = _arr.size();

   _arr.resize(old_size + size);
   memcpy(_arr.ptr() + old_size, data, size);
}

int ArrayOutput::tell ()
{
   return _arr.size();
}

void ArrayOutput::flush ()
{
}

void ArrayOutput::seek (int offset, int from)
{
   throw Error("not implemented");
}

void ArrayOutput::clear ()
{
   _arr.clear();
}

StandardOutput::StandardOutput ()
{
   _count = 0;
}

StandardOutput::~StandardOutput ()
{
}

void StandardOutput::write (const void *data, int size)
{
   size_t res = fwrite(data, size, 1, stdout);

   if (res != 1)
      throw Error("error writing to standard output");
   
   _count += size;
}

void StandardOutput::seek (int offset, int from)
{
   throw Error("can not seek in standard output");
}

int StandardOutput::tell ()
{
   return _count;
}

void StandardOutput::flush()
{
   fflush(stdout);
}

void bprintf (Array<char>& buf, const char *format, ...)
{
   va_list args;
   va_start(args, format);
   ArrayOutput output(buf);
   output.vprintf(format, args);
   output.writeChar(0);
   va_end(args);
}
