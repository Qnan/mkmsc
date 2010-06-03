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

#ifndef __defs_h__
#define __defs_h__

#if !defined(__max)
#define __max(a,b)  (((a) > (b)) ? (a) : (b))
#endif
#if !defined(__min)
#define __min(a,b)  (((a) < (b)) ? (a) : (b))
#endif
#if !defined(__min3)
#define __min3(a,b,c)  (__min(a, __min(b, c)))
#endif
#if !defined(__max3)
#define __max3(a,b,c)  (__max(a, __max(b, c)))
#endif
#if !defined(__sign)
#define __sign(a)  (a > 0 ? 1 : (a < 0 ? -1 : 0))
#endif

#define __swap(a, b, tmp) ((tmp) = (b), (b) = (a), (a) = (tmp))

#if defined(_WIN32)
//#define vsnprintf _vsnprintf
#define snprintf _snprintf
#define strcasecmp _stricmp
#define strncasecmp _strnicmp
#endif

#ifndef M_PI
#define M_PI       3.14159265358979323846
#endif

#define NELEM(arr) ((int)(sizeof(arr) / sizeof(arr[0])))

#ifndef dword
typedef unsigned int dword;
#endif
typedef unsigned char byte;

#ifdef _WIN32
#define DLLEXPORT __declspec(dllexport)
#else
#define DLLEXPORT
#endif

#ifndef __cplusplus
   #define CEXPORT DLLEXPORT
#else
   #define CEXPORT extern "C" DLLEXPORT
#endif

//#define CEXPORT extern "C" DLLEXPORT

#ifdef _WIN32
#define qword unsigned _int64
#else
#define qword unsigned long long
#endif

typedef unsigned short word;

#if !defined(NULL)
#define NULL 0L
#endif

#endif
