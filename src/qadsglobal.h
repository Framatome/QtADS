/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * QtADS Library
 * Copyright (C) 2012-2018   Framatome, Inc.
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *****************************************************************************/
#ifndef QTADSGLOBAL_H
#define QTADSGLOBAL_H

#include <qglobal.h>

// QTADS_VERSION is (major << 16) + (minor << 8) + patch.

#define QTADS_VERSION       0x010000
#define QTADS_VERSION_STR   "1.0.0"

#ifdef QTADS_DLL

#if defined(QTADS_MAKEDLL)     // create a QtADS DLL library
#define QTADS_EXPORT  Q_DECL_EXPORT
#else                        // use a QtADS DLL library
#define QTADS_EXPORT  Q_DECL_IMPORT
#endif

#endif // QTADS_DLL

#ifndef QTADS_EXPORT
#define QTADS_EXPORT
#endif

#ifndef Q_DECL_OVERRIDE
#  if __cplusplus <= 199711L
#    define Q_DECL_OVERRIDE
#  else
#    define Q_DECL_OVERRIDE override
#  endif
#endif

#ifndef Q_NULLPTR
#  if __cplusplus <= 199711L
#    define Q_NULLPTR NULL
#  else
#    define Q_NULLPTR nullptr
#  endif
#endif

#endif //QTADSGLOBAL_H
