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
#include "qadsplcvariable.h"
#include <QByteArray>
#if defined(USE_TWINCAT)
#include <windows.h>
#include <tcadsdef.h>
#include <tcadsapi.h>
#else
#include <AdsLib.h>
#endif
#include "qadssymbols.h"

int QADSPLCVariable::m_plcVariableReadOperationMetaTypeId = -1;

QADSPLCVariable::QADSPLCVariable(QObject *parent, quint16 amsport, const QString &amsnetid, const QString &amshost,
                                 const QString &var, PLCVariableReadOperation readop, unsigned long cycletime) :
    QADSObject(parent, amsport, amsnetid, amshost),
    m_plcVariableName(var),
    m_plcVariableReadOperation(readop),
    m_plcVariableCycleTime(cycletime),
    m_adsSymbolName(tr("Unknown")),
    m_adsSymbolGroup(0),
    m_adsSymbolOffset(0),
    m_adsSymbolSize(0),
    m_adsSymbolType(tr("Unknown")),
    m_adsSymbolComment(QString("")),
    m_adsSymbolHandle(0)
{
    // Register new meta type if not already registered.
    if( !QMetaType::isRegistered(m_plcVariableReadOperationMetaTypeId) )
    {
        m_plcVariableReadOperationMetaTypeId = qRegisterMetaType<PLCVariableReadOperation>("QADSPLCVariable::PLCVariableReadOperation");
    }

    getPLCVariableInformation();

    // Monitor ADS state changes.
    connect(this, SIGNAL(adsStateChanged()),
            this, SLOT(protCatchADSStateChange()));
    connect(this, SIGNAL(listOfADSSymbolNamesChanged()),
            this, SLOT(protCatchADSSymbolTableChanged()));
}

QADSPLCVariable::~QADSPLCVariable()
{
    releaseHandle();
}

void QADSPLCVariable::initializeADSConnection()
{
    QADSObject::initializeADSConnection();
    releaseHandle();
    getPLCVariableInformation();
}

void QADSPLCVariable::getPLCVariableInformation()
{
    // Get the information for this variable if there are no errors.
    if( !m_adsError && !m_plcVariableName.isEmpty() && (adsState() == QADSObject::STATE_RUN) )
    {
        QByteArray szVariable = m_plcVariableName.toLatin1();
        quint8 buffer[0xFFFF];
        PAdsSymbolEntry pAdsSymbolEntry;
        unsigned long returnSize = 0;

        syncReadWriteRequest(amsPort(), ADSIGRP_SYM_INFOBYNAMEEX, 0, sizeof(buffer), buffer,
                             szVariable.count(), szVariable.data(), &returnSize);
        if ( returnSize && !m_adsError )
        {
            pAdsSymbolEntry = reinterpret_cast<PAdsSymbolEntry>(buffer);
            const char* name = PADSSYMBOLNAME(pAdsSymbolEntry);
            QByteArray nameArray = QByteArray(name, pAdsSymbolEntry->nameLength);
            if (m_adsSymbolName != QString::fromLatin1(nameArray))
            {
                m_adsSymbolName = QString::fromLatin1(nameArray);
                Q_EMIT adsSymbolNameChanged();
            }
            if (m_adsSymbolGroup != pAdsSymbolEntry->iGroup)
            {
                m_adsSymbolGroup = pAdsSymbolEntry->iGroup;
                Q_EMIT adsSymbolGroupChanged();
            }
            if (m_adsSymbolOffset != pAdsSymbolEntry->iOffs)
            {
                m_adsSymbolOffset = pAdsSymbolEntry->iOffs;
                Q_EMIT adsSymbolOffsetChanged();
            }
            if (m_adsSymbolSize != pAdsSymbolEntry->size)
            {
                m_adsSymbolSize = pAdsSymbolEntry->size;
                Q_EMIT adsSymbolSizeChanged();
            }
            const char *symbolType = PADSSYMBOLTYPE(pAdsSymbolEntry);
            nameArray = QByteArray(symbolType, pAdsSymbolEntry->typeLength);
            if (m_adsSymbolType != QString::fromLatin1(nameArray))
            {
                m_adsSymbolType = QString::fromLatin1(nameArray);
                Q_EMIT adsSymbolTypeChanged();
            }
            const char *symbolComment = PADSSYMBOLCOMMENT(pAdsSymbolEntry);
            nameArray = QByteArray(symbolComment, pAdsSymbolEntry->commentLength);
            if (m_adsSymbolComment != QString::fromLatin1(nameArray))
            {
                m_adsSymbolComment = QString::fromLatin1(nameArray);
                Q_EMIT adsSymbolCommentChanged();
            }

            // Get variable handle
            quint32 adsSymbolHandle = m_adsSymbolHandle;
            syncReadWriteRequest(amsPort(), ADSIGRP_SYM_HNDBYNAME, 0x0, sizeof(adsSymbolHandle),
                                 &adsSymbolHandle, szVariable.count(), szVariable.data(), Q_NULLPTR);
            if (adsSymbolHandle != m_adsSymbolHandle)
            {
                m_adsSymbolHandle = adsSymbolHandle;
                Q_EMIT adsSymbolHandleChanged();
            }
        }
        else
        {
            // Check for symbol not found error.
            if( m_adsErrorString.contains("0x710:") )
            {
                // Suggest list of possible symbols.
                QStringList listOfPossibleNames = listOfADSSymbolNames().filter(m_plcVariableName,Qt::CaseInsensitive);
                if( listOfPossibleNames.count() )
                {
                    QString listString = listOfPossibleNames.join(", ");
                    m_adsErrorString += tr(". Try using one of the following symbols: ");
                    m_adsErrorString += listString;
                }
                else
                {
                    m_adsErrorString += tr(". Symbol = %1").arg(m_plcVariableName);
                }
                Q_EMIT adsErrorStringChanged();
            }
        }
    }
}

void QADSPLCVariable::releaseHandle()
{
    if ( m_adsSymbolHandle != 0 )
    {
        quint32 adsSymbolHandle = m_adsSymbolHandle;
        syncWriteRequest(amsPort(), ADSIGRP_SYM_RELEASEHND, 0, sizeof(adsSymbolHandle), &adsSymbolHandle);
        if (!m_adsError)
        {
            m_adsSymbolHandle = 0;
            Q_EMIT adsSymbolHandleChanged();
        }
    }
}

void QADSPLCVariable::protCatchADSStateChange()
{
    releaseHandle();
    getPLCVariableInformation();
}

void QADSPLCVariable::protCatchADSSymbolTableChanged()
{
    releaseHandle();
    getPLCVariableInformation();
}

void QADSPLCVariable::setPLCVariableReadOperation(PLCVariableReadOperation readop)
{
    if (m_plcVariableReadOperation != readop)
    {
        m_plcVariableReadOperation = readop;
        Q_EMIT plcVariableReadOperationChanged();
    }
}

void QADSPLCVariable::setPLCVariableCycleTime(unsigned long cycletime)
{
    if (m_plcVariableCycleTime != cycletime)
    {
        m_plcVariableCycleTime = cycletime;
        Q_EMIT plcVariableCycleTimeChanged();
    }
}

void QADSPLCVariable::setPLCVariableName(const QString &var)
{
    if (m_plcVariableName != var)
    {
        m_plcVariableName = var;
        Q_EMIT plcVariableNameChanged();
    }
}
