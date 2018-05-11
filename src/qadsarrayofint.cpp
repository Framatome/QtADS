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
#include "qadsarrayofint.h"
#include <QStringList>
#if defined(USE_TWINCAT)
#include <tcadsdef.h>
#else
#include <AdsDef.h>
#endif

QADSARRAYOFINT::QADSARRAYOFINT(QObject *parent, quint16 amsport, const QString &amsnetid, const QString &amshost,
                               const QString &var, PLCVariableReadOperation readop, unsigned long cycletime) :
    QADSInteger16Array(parent,amsport,amsnetid,amshost,var,readop,cycletime)
{
    baseParseArrayParameters();
    readValue();
    setupVariableCallback();
}

void QADSARRAYOFINT::parseArrayParameters()
{
    QADSInteger16Array::parseArrayParameters();
    baseParseArrayParameters();
}

void QADSARRAYOFINT::baseParseArrayParameters()
{
    size_t totalArraySize = 0;

    // Only works for the ARRAY[] OF INT type!
    bool tempError = false;
    // Split string by OF should produce 2 strings ARRAY[] and INT.
    QStringList splitSymbolName = adsSymbolType().split(QString("OF"),QString::SkipEmptyParts);
    if( splitSymbolName.count() != 2 )
    {
        tempError = true;
    }
    else
    {
        if( splitSymbolName[1].trimmed() != "INT" )
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
                        QADSQINT16ARRAY tempArray;
                        if( sizes[2] != 0 )
                        {
                            tempArray = QADSQINT16ARRAY(sizes[0],sizes[1],sizes[2]);

                        }
                        else if( sizes[1] != 0 )
                        {
                            tempArray = QADSQINT16ARRAY(sizes[0],sizes[1]);
                        }
                        else if( sizes[0] != 0 )
                        {
                            tempArray = QADSQINT16ARRAY(sizes[0]);
                        }
                        else
                        {
                            tempError = true;
                        }
                        if( !tempError )
                        {
                            totalArraySize = tempArray.sizeInBytes();
                            QADSInteger16Array::setValue(tempArray);
                        }
                    }
                }
            }
        }
    }
    if( !m_adsError && tempError )
    {
        m_adsError = true;
        m_adsErrorString = tr("QADSARRAYOFINT, Line ");
        m_adsErrorString += QString("%1").arg(__LINE__);
        m_adsErrorString += ": ";
        m_adsErrorString += tr("Error: %1 is a %2 type instead of an ARRAY[] OF INT as required by this class. Please check the PLC declaration.").arg(adsSymbolName()).arg(adsSymbolType());
        Q_EMIT adsErrorChanged();
        Q_EMIT adsErrorStringChanged();
    }
    if( !m_adsError && (adsSymbolSize() != totalArraySize) )
    {
        m_adsError = true;
        m_adsErrorString = tr("QADSARRAYOFINT, Line ");
        m_adsErrorString += QString("%1").arg(__LINE__);
        m_adsErrorString += ": ";
        m_adsErrorString += tr("Error: ADS symbol size = %1 does not equal the parsed array data size = %2.").arg(adsSymbolSize()).arg(totalArraySize);
        Q_EMIT adsErrorChanged();
        Q_EMIT adsErrorStringChanged();
    }
}

const QADSQINT16ARRAY &QADSARRAYOFINT::value()
{
    // Read the value depending on the read operation mode.
    if( plcVariableReadOperation() == ON_DEMAND )
    {
        readValue();
    }
    return QADSInteger16Array::value();
}

qint16 QADSARRAYOFINT::value(int x)
{
    return value()[x];
}

qint16 QADSARRAYOFINT::value(int y, int x)
{
    return value()(y,x);
}

qint16 QADSARRAYOFINT::value(int z, int y, int x)
{
    return value()(z,y,x);
}

void QADSARRAYOFINT::setValue(const QADSQINT16ARRAY &val)
{
    // Set value of variable if changed & no error
    if( !m_adsError && (val != value()) )
    {
        syncWriteRequest(amsPort(), ADSIGRP_SYM_VALBYHND, adsSymbolHandle(), sizeof(qint16)*val.count(), val.array());
    }
    if( !m_adsError && (plcVariableReadOperation() == ON_DEMAND) )
    {
       QADSInteger16Array::setValue(val);
    }
}

void QADSARRAYOFINT::setValue(qint16 val, int x)
{
    // Set value of variable if changed & no error
    if( !m_adsError && (val != value()[x]) )
    {
        QADSQINT16ARRAY tempValue = value();
        tempValue[x] = val;
        setValue(tempValue);
    }
}

void QADSARRAYOFINT::setValue(qint16 val, int y, int x)
{
    // Set value of variable if changed & no error
    if( !m_adsError && (val != value()(y,x)) )
    {
        QADSQINT16ARRAY tempValue = value();
        tempValue(y,x) = val;
        setValue(tempValue);
    }
}

void QADSARRAYOFINT::setValue(qint16 val, int z, int y, int x)
{
    // Set value of variable if changed & no error
    if( !m_adsError && (val != value()(z,y,x)) )
    {
        QADSQINT16ARRAY tempValue = value();
        tempValue(z,y,x) = val;
        setValue(tempValue);
    }
}

