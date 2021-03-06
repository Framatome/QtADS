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
#include "qadsarrayofudint.h"
#include <QStringList>
#if defined(USE_TWINCAT)
#include <tcadsdef.h>
#else
#include <AdsDef.h>
#endif

QADSARRAYOFUDINT::QADSARRAYOFUDINT(QObject *parent, quint16 amsport, const QString &amsnetid, const QString &amshost,
                                   const QString &var, PLCVariableReadOperation readop, unsigned long cycletime) :
    QADSUnsignedInteger32Array(parent,amsport,amsnetid,amshost,var,readop,cycletime)
{
    baseParseArrayParameters();
    readValue();
    setupVariableCallback();
}

void QADSARRAYOFUDINT::parseArrayParameters()
{
    QADSUnsignedInteger32Array::parseArrayParameters();
    baseParseArrayParameters();
}

void QADSARRAYOFUDINT::baseParseArrayParameters()
{
    size_t totalArraySize = 0;

    // Only works for the ARRAY[] OF UDINT type!
    bool tempError = false;
    // Split string by OF should produce 2 strings ARRAY[] and UDINT.
    QStringList splitSymbolName = adsSymbolType().split(QString("OF"),QString::SkipEmptyParts);
    if( splitSymbolName.count() != 2 )
    {
        tempError = true;
    }
    else
    {
        if( splitSymbolName[1].trimmed() != "UDINT" )
        {
            tempError = true;
        }
        else
        {
            // Parse array dimensions.
            // Get string between [ and ].
            QString arrayIndicesString = splitSymbolName[0].trimmed().section('[',1,1).section(']',0,0);
            if( arrayIndicesString == "" )
            {
                tempError = true;
            }
            else
            {
                QStringList arrayIndices = arrayIndicesString.split(',');
                if( (arrayIndices.count() < 1) || (arrayIndices.count() > 3) )
                {
                    tempError = true;
                }
                else
                {
                    // Calculate size of each dimension.
                    int sizes[3] = {0,0,0};
                    for(int i=0; i<arrayIndices.count(); i++)
                    {
                        QStringList indices = arrayIndices[i].trimmed().split("..");
                        if( indices.count() != 2 )
                        {
                            tempError = true;
                            break;
                        }
                        else
                        {
                            sizes[i] = indices[1].toInt()-indices[0].toInt()+1;
                        }
                    }
                    if( !tempError )
                    {
                        QADSQUINT32ARRAY tempArray;
                        if( sizes[2] != 0 )
                        {
                            tempArray = QADSQUINT32ARRAY(sizes[0],sizes[1],sizes[2]);

                        }
                        else if( sizes[1] != 0 )
                        {
                            tempArray = QADSQUINT32ARRAY(sizes[0],sizes[1]);
                        }
                        else if( sizes[0] != 0 )
                        {
                            tempArray = QADSQUINT32ARRAY(sizes[0]);
                        }
                        else
                        {
                            tempError = true;
                        }
                        if( !tempError )
                        {
                            totalArraySize = tempArray.sizeInBytes();
                            QADSUnsignedInteger32Array::setValue(tempArray);
                        }
                    }
                }
            }
        }
    }
    if( !m_adsError && tempError )
    {
        m_adsError = true;
        m_adsErrorString = tr("QADSARRAYOFUDINT, Line ");
        m_adsErrorString += QString("%1").arg(__LINE__);
        m_adsErrorString += ": ";
        m_adsErrorString += tr("Error: %1 is a %2 type instead of an ARRAY[] OF UINT as required by this class. Please check the PLC declaration.").arg(adsSymbolName()).arg(adsSymbolType());
        Q_EMIT adsErrorChanged();
        Q_EMIT adsErrorStringChanged();
    }
    if( !m_adsError && (adsSymbolSize() != totalArraySize) )
    {
        m_adsError = true;
        m_adsErrorString = tr("QADSARRAYOFUDINT, Line ");
        m_adsErrorString += QString("%1").arg(__LINE__);
        m_adsErrorString += ": ";
        m_adsErrorString += tr("Error: ADS symbol size = %1 does not equal the parsed array data size = %2.").arg(adsSymbolSize()).arg(totalArraySize);
        Q_EMIT adsErrorChanged();
        Q_EMIT adsErrorStringChanged();
    }
}

const QADSQUINT32ARRAY &QADSARRAYOFUDINT::value()
{
    // Read the value depending on the read operation mode.
    if( plcVariableReadOperation() == ON_DEMAND )
    {
        readValue();
    }
    return QADSUnsignedInteger32Array::value();
}

quint32 QADSARRAYOFUDINT::value(int x)
{
    return value()[x];
}

quint32 QADSARRAYOFUDINT::value(int y, int x)
{
    return value()(y,x);
}

quint32 QADSARRAYOFUDINT::value(int z, int y, int x)
{
    return value()(z,y,x);
}

void QADSARRAYOFUDINT::setValue(const QADSQUINT32ARRAY &val)
{
    // Set value of variable if changed & no error
    if( !m_adsError && (val != value()) )
    {
        syncWriteRequest(amsPort(), ADSIGRP_SYM_VALBYHND, adsSymbolHandle(), sizeof(quint32)*val.count(), val.array());
    }
    if( !m_adsError && (plcVariableReadOperation() == ON_DEMAND) )
    {
       QADSUnsignedInteger32Array::setValue(val);
    }
}

void QADSARRAYOFUDINT::setValue(quint32 val, int x)
{
    // Set value of variable if changed & no error
    if( !m_adsError && (val != value()[x]) )
    {
        QADSQUINT32ARRAY tempValue = value();
        tempValue[x] = val;
        setValue(tempValue);
    }
}

void QADSARRAYOFUDINT::setValue(quint32 val, int y, int x)
{
    // Set value of variable if changed & no error
    if( !m_adsError && (val != value()(y,x)) )
    {
        QADSQUINT32ARRAY tempValue = value();
        tempValue(y,x) = val;
        setValue(tempValue);
    }
}

void QADSARRAYOFUDINT::setValue(quint32 val, int z, int y, int x)
{
    // Set value of variable if changed & no error
    if( !m_adsError && (val != value()(z,y,x)) )
    {
        QADSQUINT32ARRAY tempValue = value();
        tempValue(z,y,x) = val;
        setValue(tempValue);
    }
}
