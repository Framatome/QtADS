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
#include "qadsint.h"
#if defined(USE_TWINCAT)
#include "tcadsdef.h"
#else
#include <AdsDef.h>
#endif

static const QString TWINCAT2_INT_TYPE_STRING = "INT16";
static const QString TWINCAT3_INT_TYPE_STRING = "INT";

QADSINT::QADSINT(QObject *parent, quint16 amsport, const QString &amsnetid, const QString &amshost,
                 const QString &var, PLCVariableReadOperation readop, unsigned long cycletime) :
    QADSInteger16(parent,amsport,amsnetid,amshost,var,readop,cycletime),
    m_intTypeString((amsport==851)?TWINCAT3_INT_TYPE_STRING:TWINCAT2_INT_TYPE_STRING)
{
    parseVariableType();
    readValue();
    setupVariableCallback();
}

qint16 QADSINT::value()
{
    // Read the value depending on the read operation mode.
    if( plcVariableReadOperation() == ON_DEMAND )
    {
        readValue();
    }
    return QADSInteger16::value();
}

void QADSINT::setValue(qint16 val)
{
    // Set value of variable if changed & no error
    if( !m_adsError && (val != value()) )
    {
        qint16 sendValue = val;
        syncWriteRequest(amsPort(), ADSIGRP_SYM_VALBYHND, adsSymbolHandle(), sizeof(sendValue), &sendValue);
    }
    if( !m_adsError && (plcVariableReadOperation() == ON_DEMAND) )
    {
       QADSInteger16::setValue(val);
    }
}

void QADSINT::setAMSPort(quint16 nPort)
{
    QADSObject::setAMSPort(nPort);
    m_intTypeString = (nPort==851)?TWINCAT3_INT_TYPE_STRING:TWINCAT2_INT_TYPE_STRING;
}

void QADSINT::parseVariableType()
{
    // Only works for the m_intTypeString type!
    if( !m_adsError && (adsSymbolType() != m_intTypeString) )
    {
        m_adsError = true;
        m_adsErrorString = tr("QADSINT, Line ");
        m_adsErrorString += QString("%1").arg(__LINE__);
        m_adsErrorString += ": ";
        m_adsErrorString += tr("Error: %1 is a %2 type instead of %3 as required by this class. Please check the PLC declaration.")
                .arg(plcVariableName()).arg(adsSymbolType()).arg(m_intTypeString);
        Q_EMIT adsErrorChanged();
        Q_EMIT adsErrorStringChanged();
    }
}
