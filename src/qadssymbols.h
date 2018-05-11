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
#ifndef QADSSYMBOLS_H
#define QADSSYMBOLS_H

#ifndef PADSSYMBOLNAME
#pragma	pack( push, 1)
////////////////////////////////////////////////////////////////////////////////
// ADS symbol information
typedef struct
{
    uint32_t    entryLength;    // length of complete symbol entry
    uint32_t    iGroup;         // indexGroup of symbol: input, output etc.
    uint32_t    iOffs;          // indexOffset of symbol
    uint32_t    size;           // size of symbol ( in bytes, 0 = bit )
    uint32_t    dataType;       // adsDataType of symbol
    uint32_t    flags;          // see above
    uint16_t    nameLength;     // length of symbol name (excl. \0)
    uint16_t    typeLength;     // length of type name (excl. \0)
    uint16_t    commentLength;  // length of comment (excl. \0)
} AdsSymbolEntry, *PAdsSymbolEntry, **PPAdsSymbolEntry;


#define	PADSSYMBOLNAME(p)			((char*)(((PAdsSymbolEntry)p)+1))
#define	PADSSYMBOLTYPE(p)			(((char*)(((PAdsSymbolEntry)p)+1))+((PAdsSymbolEntry)p)->nameLength+1)
#define	PADSSYMBOLCOMMENT(p)		(((char*)(((PAdsSymbolEntry)p)+1))+((PAdsSymbolEntry)p)->nameLength+1+((PAdsSymbolEntry)p)->typeLength+1)

#define	PADSNEXTSYMBOLENTRY(pEntry)	(*((unsigned long*)(((char*)pEntry)+((PAdsSymbolEntry)pEntry)->entryLength)) \
                        ? ((PAdsSymbolEntry)(((char*)pEntry)+((PAdsSymbolEntry)pEntry)->entryLength)): Q_NULLPTR)

////////////////////////////////////////////////////////////////////////////////
typedef struct
{
    uint32_t    nSymbols;
    uint32_t    nSymSize;
} AdsSymbolUploadInfo, *PAdsSymbolUploadInfo;

#pragma	pack( pop )
#endif

#endif // QADSSYMBOLS_H
