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
#include "qadsstring.h"
#include <QByteArray>
#include <QVector>
#include <QMutexLocker>
#if defined(USE_TWINCAT)
#include <windows.h>
#include <tcadsdef.h>
#include <tcadsapi.h>
#else
#include <AdsLib.h>
#endif

void QADSSTRINGCallbackClassTemplate::setValue(const QString &value)
{
    Q_EMIT valueChanged(value);
}

class QADSSTRING::QADSSTRINGCallbackClass : public QADSSTRINGCallbackClassTemplate
{
public:
#ifdef USE_TWINCAT
    static void __stdcall Callback(AmsAddr*, AdsNotificationHeader*, unsigned long);
#else
    static void Callback(const AmsAddr*, const AdsNotificationHeader*, uint32_t);
#endif
    static QMutex m_pointerVectorMutex;
    static QVector<QADSSTRING::QADSSTRINGCallbackClass*> m_pointerVector;
};

QMutex QADSSTRING::QADSSTRINGCallbackClass::m_pointerVectorMutex;
QVector<QADSSTRING::QADSSTRINGCallbackClass*> QADSSTRING::QADSSTRINGCallbackClass::m_pointerVector;

// Callback-function
#ifdef USE_TWINCAT
void __stdcall QADSSTRING::QADSSTRINGCallbackClass::Callback(AmsAddr* pAddr, AdsNotificationHeader* pNotification, unsigned long hUser)
{
    Q_UNUSED(pAddr);
    QMutexLocker locker(&m_pointerVectorMutex);
    Q_UNUSED(locker);
    QString value;
    QADSSTRING::QADSSTRINGCallbackClass *sender = m_pointerVector[hUser];

    value = QString::fromLatin1(QByteArray(reinterpret_cast<const char *>(pNotification->data), pNotification->cbSampleSize));

    if( sender != Q_NULLPTR )
    {
        sender->setValue(value);
    }
}
#else
void QADSSTRING::QADSSTRINGCallbackClass::Callback(const AmsAddr* pAddr, const AdsNotificationHeader* pNotification, uint32_t hUser)
{
    Q_UNUSED(pAddr);
    QMutexLocker locker(&m_pointerVectorMutex);
    Q_UNUSED(locker);
    QString value;
    QADSSTRING::QADSSTRINGCallbackClass *sender = m_pointerVector[hUser];

    const uint8_t* data = reinterpret_cast<const uint8_t*>(pNotification + 1);
    value = QString::fromLatin1(QByteArray(reinterpret_cast<const char *>(data), pNotification->cbSampleSize));

    if( sender != Q_NULLPTR )
    {
        sender->setValue(value);
    }
}
#endif

QADSSTRING::QADSSTRING(QObject *parent, quint16 amsport, const QString &amsnetid, const QString &amshost,
                       const QString &var, PLCVariableReadOperation readop, unsigned long cycletime) :
    QADSPLCVariable(parent,amsport,amsnetid,amshost,var,readop,cycletime),
    m_value(""),
    m_adsSTRINGCallback(Q_NULLPTR),
    m_variableCallBackNotificationHandle(0)
{
    parseStringSize();
    readValue();
    setupVariableCallback();
}

QADSSTRING::~QADSSTRING()
{
    resetVariableCallback();
}

void QADSSTRING::parseStringSize()
{
    // Only works for the STRING type!
    QString stringSymbol = adsSymbolType().trimmed().section('(',0,0);

    if( !m_adsError && (stringSymbol != "STRING") )
    {
        m_adsError = true;
        m_adsErrorString = tr("QADSSTRING, Line ");
        m_adsErrorString += QString("%1").arg(__LINE__);
        m_adsErrorString += ": ";
        m_adsErrorString += tr("Error: %1 is a %2 type instead of STRING as required by this class. Please check the PLC declaration.").arg(plcVariableName()).arg(adsSymbolType());
        Q_EMIT adsErrorChanged();
        Q_EMIT adsErrorStringChanged();
    }
    else
    {
        m_parsedStringSize = adsSymbolType().trimmed().section('(',1,1).section(')',0,0).toInt();
        if( m_parsedStringSize )
        {
            ++m_parsedStringSize;
        }
        if( m_parsedStringSize != adsSymbolSize() )
        {
            m_adsError = true;
            m_adsErrorString = tr("QADSSTRING, Line ");
            m_adsErrorString += QString("%1").arg(__LINE__);
            m_adsErrorString += ": ";
            m_adsErrorString += tr("Error: Parsed string size = %1 does not equal the ADS symbol size = %2").arg(m_parsedStringSize).arg(adsSymbolSize());
            Q_EMIT adsErrorChanged();
            Q_EMIT adsErrorStringChanged();
        }
    }
}

void QADSSTRING::setupVariableCallback()
{
    // Setup callback if no error and required.
    if( !m_adsError && (plcVariableReadOperation() != ON_DEMAND) &&
            (adsState() == QADSObject::STATE_RUN) )
    {
        long nErr;
        AmsAddr Addr;
        AmsAddr *pAddr = &Addr;
        unsigned long hUser;
        AdsNotificationAttrib adsNotificationAttrib;

        Addr.port = amsPort();
        if( !stringToAddress(amsNetId(), Addr.netId.b) )
        {
            m_adsError = true;
            m_adsErrorString = tr("QADSSTRING, Line ");
            m_adsErrorString += QString("%1").arg(__LINE__);
            m_adsErrorString += ": ";
            m_adsErrorString += tr("Error: %1 is not a valid AMS Net ID. AMS Net ID's are of the form XXX.XXX.XXX.XXX.1.1, where XXX is a number between 1 and 255.").arg(amsNetId());
            Q_EMIT adsErrorChanged();
            Q_EMIT adsErrorStringChanged();
        }
        if ( !m_adsError )
        {
            // Create the callback class pointer if not already
            // created.
            if ( m_adsSTRINGCallback == Q_NULLPTR )
            {
                m_adsSTRINGCallback = new QADSSTRING::QADSSTRINGCallbackClass();
                QADSSTRING::QADSSTRINGCallbackClass::m_pointerVector.append(m_adsSTRINGCallback);
            }

            // Get pointer index.
            hUser = QADSSTRING::QADSSTRINGCallbackClass::m_pointerVector.size()-1;

            // set the attributes of the notification
            adsNotificationAttrib.cbLength = adsSymbolSize();
            adsNotificationAttrib.nMaxDelay = 0;
            switch(plcVariableReadOperation())
            {
            case CYCLICAL:
                adsNotificationAttrib.nTransMode = ADSTRANS_SERVERCYCLE;
                adsNotificationAttrib.nCycleTime = plcVariableCycleTime();
                break;
            case SYNCHRONIZED:
                adsNotificationAttrib.nTransMode = ADSTRANS_SERVERONCHA;
                adsNotificationAttrib.nCycleTime = 0;
                break;
            case ON_DEMAND:
            default:
                m_adsError = true;
                m_adsErrorString = tr("QADSSTRING, Line ");
                m_adsErrorString += QString("%1").arg(__LINE__);
                m_adsErrorString += ": ";
                m_adsErrorString += tr("Error: Received invalid/unknown read operation mode %1").arg(plcVariableReadOperation());
                Q_EMIT adsErrorChanged();
                Q_EMIT adsErrorStringChanged();
                break;
            }
            if( !m_adsError )
            {
                // Delete previous request if notification handle is not 0.
                if( m_variableCallBackNotificationHandle != 0 )
                {
                    nErr = AdsSyncDelDeviceNotificationReqEx(adsPort(), pAddr, m_variableCallBackNotificationHandle);
                    if (nErr)
                    {
                        m_adsError = true;
                        m_adsErrorString = tr("QADSSTRING, Line ");
                        m_adsErrorString += QString("%1").arg(__LINE__);
                        m_adsErrorString += ": ";
                        m_adsErrorString += plcVariableName();
                        m_adsErrorString += ": ";
                        m_adsErrorString += generateADSErrorString(nErr);
                        Q_EMIT adsErrorChanged();
                        Q_EMIT adsErrorStringChanged();
                    }
                }
                if( !m_adsError )
                {
                    // initiate the transmission of the PLC-variable
#ifdef USE_TWINCAT
                    nErr = AdsSyncAddDeviceNotificationReqEx(adsPort(), pAddr, ADSIGRP_SYM_VALBYHND,
                                                             adsSymbolHandle(), &adsNotificationAttrib,
                                                             QADSSTRING::QADSSTRINGCallbackClass::Callback, hUser,
                                                             &m_variableCallBackNotificationHandle);
#else
                    uint32_t handle = 0;
                    nErr = AdsSyncAddDeviceNotificationReqEx(adsPort(), pAddr, ADSIGRP_SYM_VALBYHND,
                                                             adsSymbolHandle(), &adsNotificationAttrib,
                                                             QADSSTRING::QADSSTRINGCallbackClass::Callback, hUser,
                                                             &handle);
                    m_variableCallBackNotificationHandle = handle;
#endif
                    if (nErr)
                    {
                        m_adsError = true;
                        m_adsErrorString = tr("QADSSTRING, Line ");
                        m_adsErrorString += QString("%1").arg(__LINE__);
                        m_adsErrorString += ": ";
                        m_adsErrorString += plcVariableName();
                        m_adsErrorString += ": ";
                        m_adsErrorString += generateADSErrorString(nErr);
                        Q_EMIT adsErrorChanged();
                        Q_EMIT adsErrorStringChanged();
                    }
                    else
                    {
                        // Connect to Qt since there were no errors
                        connect(m_adsSTRINGCallback, SIGNAL(valueChanged(QString)),
                                this, SLOT(privSetValue(QString)), static_cast<Qt::ConnectionType>(Qt::QueuedConnection | Qt::UniqueConnection));
                    }
                }
            }
        }
    }
}

void QADSSTRING::resetVariableCallback()
{
    // Delete request if notification handle is not 0.
    if( m_variableCallBackNotificationHandle != 0 )
    {
        AmsAddr Addr;
        AmsAddr *pAddr = &Addr;

        Addr.port = amsPort();
        if( stringToAddress(amsNetId(), Addr.netId.b) )
        {
            AdsSyncDelDeviceNotificationReqEx(adsPort(), pAddr, m_variableCallBackNotificationHandle);
            m_variableCallBackNotificationHandle = 0;
        }
    }
    if( m_adsSTRINGCallback )
    {
        QMutexLocker locker(&QADSSTRING::QADSSTRINGCallbackClass::m_pointerVectorMutex);
        Q_UNUSED(locker);
        int index = QADSSTRING::QADSSTRINGCallbackClass::m_pointerVector.indexOf(m_adsSTRINGCallback);
        if (index >= 0)
        {
            QADSSTRING::QADSSTRINGCallbackClass::m_pointerVector[index] = Q_NULLPTR;
        }
        delete m_adsSTRINGCallback;
        m_adsSTRINGCallback = Q_NULLPTR;
    }
}

void QADSSTRING::protCatchADSStateChange()
{
    // The parent implementation must be called first
    QADSPLCVariable::protCatchADSStateChange();
    parseStringSize();
    readValue();
    resetVariableCallback();
    setupVariableCallback();
}

void QADSSTRING::protCatchADSSymbolTableChanged()
{
    // The parent implementation must be called first
    QADSPLCVariable::protCatchADSSymbolTableChanged();
    parseStringSize();
    readValue();
    resetVariableCallback();
    setupVariableCallback();
}

void QADSSTRING::readValue()
{
    if( !m_adsError )
    {
        QByteArray returnValue = QByteArray(adsSymbolSize(),'\0');
        syncReadRequest(amsPort(), ADSIGRP_SYM_VALBYHND, adsSymbolHandle(), adsSymbolSize(), returnValue.data(), Q_NULLPTR);
        if ( !m_adsError )
        {
            privSetValue(returnValue);
        }
    }
}

const QString &QADSSTRING::value()
{
    // Read the value depending on the read operation mode.
    if( plcVariableReadOperation() == ON_DEMAND )
    {
        readValue();
    }
    return m_value;
}

void QADSSTRING::initializeADSConnection()
{
    QADSPLCVariable::initializeADSConnection();
    parseStringSize();
    readValue();
    resetVariableCallback();
    setupVariableCallback();
}

void QADSSTRING::setValue(const QString &val)
{
    // Set value of variable if changed & no error
    if( !m_adsError && (val != value()) )
    {
        QByteArray sendValue = QByteArray(adsSymbolSize(),'\0');
        sendValue.insert(0,val);
        syncWriteRequest(amsPort(), ADSIGRP_SYM_VALBYHND, adsSymbolHandle(), adsSymbolSize(), sendValue.data());
    }
    if( !m_adsError && (plcVariableReadOperation() == ON_DEMAND) )
    {
       privSetValue(val);
    }
}

void QADSSTRING::privSetValue(const QString &val)
{
    if( val != m_value )
    {
        m_value = val;
        Q_EMIT valueChanged();
    }
}
