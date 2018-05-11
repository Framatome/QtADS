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
#include "qadsuint.h"
#if defined(USE_TWINCAT)
#include "tcadsdef.h"
#else
#include <AdsDef.h>
#endif

static const QString TWINCAT2_UINT_TYPE_STRING = "UINT16";
static const QString TWINCAT3_UINT_TYPE_STRING = "UINT";

QADSUINT::QADSUINT(QObject *parent, quint16 amsport, const QString &amsnetid, const QString &amshost,
                   const QString &var, PLCVariableReadOperation readop, unsigned long cycletime) :
    QADSUnsignedInteger16(parent,amsport,amsnetid,amshost,var,readop,cycletime),
    m_uintTypeString((amsport==851)?TWINCAT3_UINT_TYPE_STRING:TWINCAT2_UINT_TYPE_STRING)
{
    parseVariableType();
    readValue();
    setupVariableCallback();
}

quint16 QADSUINT::value()
{
    // Read the value depending on the read operation mode.
    if( plcVariableReadOperation() == ON_DEMAND )
    {
        readValue();
    }
    return QADSUnsignedInteger16::value();
}

void QADSUINT::setValue(quint16 val)
{
    // Set value of variable if changed & no error
    if( !m_adsError && (val != value()) )
    {
        quint16 sendValue = val;
        syncWriteRequest(amsPort(), ADSIGRP_SYM_VALBYHND, adsSymbolHandle(), sizeof(sendValue), &sendValue);
    }
    if( !m_adsError && (plcVariableReadOperation() == ON_DEMAND) )
    {
      QADSUnsignedInteger16::setValue(val);
    }
}

void QADSUINT::setAMSPort(quint16 nPort)
{
    QADSObject::setAMSPort(nPort);
    m_uintTypeString = (nPort==851)?TWINCAT3_UINT_TYPE_STRING:TWINCAT2_UINT_TYPE_STRING;
}

void QADSUINT::parseVariableType()
{
    // Only works for the m_uintTypeString type!
    if( !m_adsError && (adsSymbolType() != m_uintTypeString) )
    {
        m_adsError = true;
        m_adsErrorString = tr("QADSUINT, Line ");
        m_adsErrorString += QString("%1").arg(__LINE__);
        m_adsErrorString += ": ";
        m_adsErrorString += tr("Error: %1 is a %2 type instead of %3 as required by this class. Please check the PLC declaration.")
                .arg(plcVariableName()).arg(adsSymbolType()).arg(m_uintTypeString);
        Q_EMIT adsErrorChanged();
        Q_EMIT adsErrorStringChanged();
    }
}
