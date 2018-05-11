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
#include "qadstime.h"
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

void QADSTIMECallbackClassTemplate::setValue(const QTime &value)
{
    Q_EMIT valueChanged(value);
}

class QADSTIME::QADSTIMECallbackClass : public QADSTIMECallbackClassTemplate
{
public:
#ifdef USE_TWINCAT
    static void __stdcall Callback(AmsAddr*, AdsNotificationHeader*, unsigned long);
#else
    static void Callback(const AmsAddr*, const AdsNotificationHeader*, uint32_t);
#endif
    static QMutex m_pointerVectorMutex;
    static QVector<QADSTIME::QADSTIMECallbackClass*> m_pointerVector;
};

QMutex QADSTIME::QADSTIMECallbackClass::m_pointerVectorMutex;
QVector<QADSTIME::QADSTIMECallbackClass*> QADSTIME::QADSTIMECallbackClass::m_pointerVector;

// Callback-function
#ifdef USE_TWINCAT
void __stdcall QADSTIME::QADSTIMECallbackClass::Callback(AmsAddr* pAddr, AdsNotificationHeader* pNotification, unsigned long hUser)
{
    Q_UNUSED(pAddr);
    QMutexLocker locker(&m_pointerVectorMutex);
    Q_UNUSED(locker);
    QTime value = QTime(0,0);
    DWORD tempVal = 0;
    QADSTIME::QADSTIMECallbackClass *sender = m_pointerVector[hUser];

    tempVal = *((DWORD *)pNotification->data);
    value = value.addMSecs(tempVal);

    if( sender != Q_NULLPTR )
    {
        sender->setValue(value);
    }
}
#else
void QADSTIME::QADSTIMECallbackClass::Callback(const AmsAddr* pAddr, const AdsNotificationHeader* pNotification, uint32_t hUser)
{
    Q_UNUSED(pAddr);
    QMutexLocker locker(&m_pointerVectorMutex);
    Q_UNUSED(locker);
    QTime value = QTime(0,0);
    uint32_t tempVal = 0;
    QADSTIME::QADSTIMECallbackClass *sender = m_pointerVector[hUser];

    const uint8_t* data = reinterpret_cast<const uint8_t*>(pNotification + 1);
    tempVal = *((uint32_t *)data);
    value = value.addMSecs(tempVal);

    if( sender != Q_NULLPTR )
    {
        sender->setValue(value);
    }
}
#endif

int QADSTIME::m_qtimeMetaTypeId = -1;

QADSTIME::QADSTIME(QObject *parent, quint16 amsport, const QString &amsnetid, const QString &amshost,
                   const QString &var, PLCVariableReadOperation readop, unsigned long cycletime) :
    QADSPLCVariable(parent,amsport,amsnetid,amshost,var,readop,cycletime),
    m_value(0,0),
    m_adsTIMECallback(Q_NULLPTR),
    m_variableCallBackNotificationHandle(0)
{
    // Register new meta type if not already registered.
    if( !QMetaType::isRegistered(m_qtimeMetaTypeId) )
    {
        m_qtimeMetaTypeId = qRegisterMetaType<QTime>("QTime");
    }
    parseVariableType();
    readValue();
    setupVariableCallback();
}

QADSTIME::~QADSTIME()
{
    resetVariableCallback();
}

void QADSTIME::setupVariableCallback()
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
            m_adsErrorString = tr("QADSTIME, Line ");
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
            if ( m_adsTIMECallback == Q_NULLPTR )
            {
                m_adsTIMECallback = new QADSTIME::QADSTIMECallbackClass();
                QADSTIME::QADSTIMECallbackClass::m_pointerVector.append(m_adsTIMECallback);
            }

            // Get pointer index.
            hUser = QADSTIME::QADSTIMECallbackClass::m_pointerVector.size()-1;

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
                m_adsErrorString = tr("QADSTIME, Line ");
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
                        m_adsErrorString = tr("QADSTIME, Line ");
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
                                                             QADSTIME::QADSTIMECallbackClass::Callback, hUser,
                                                             &m_variableCallBackNotificationHandle);
#else
                    uint32_t handle = 0;
                    nErr = AdsSyncAddDeviceNotificationReqEx(adsPort(), pAddr, ADSIGRP_SYM_VALBYHND,
                                                             adsSymbolHandle(), &adsNotificationAttrib,
                                                             QADSTIME::QADSTIMECallbackClass::Callback, hUser,
                                                             &handle);
                    m_variableCallBackNotificationHandle = handle;
#endif
                    if (nErr)
                    {
                        m_adsError = true;
                        m_adsErrorString = tr("QADSTIME, Line ");
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
                        connect(m_adsTIMECallback, SIGNAL(valueChanged(QTime)),
                                this, SLOT(privSetValue(QTime)), static_cast<Qt::ConnectionType>(Qt::QueuedConnection | Qt::UniqueConnection));
                    }
                }
            }
        }
    }
}

void QADSTIME::resetVariableCallback()
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
    if( m_adsTIMECallback )
    {
        QMutexLocker locker(&QADSTIME::QADSTIMECallbackClass::m_pointerVectorMutex);
        Q_UNUSED(locker);
        int index = QADSTIME::QADSTIMECallbackClass::m_pointerVector.indexOf(m_adsTIMECallback);
        if (index >= 0)
        {
            QADSTIME::QADSTIMECallbackClass::m_pointerVector[index] = Q_NULLPTR;
        }
        delete m_adsTIMECallback;
        m_adsTIMECallback = Q_NULLPTR;
    }
}

void QADSTIME::parseVariableType()
{

}

void QADSTIME::protCatchADSStateChange()
{
    // The parent implementation must be called first
    QADSPLCVariable::protCatchADSStateChange();
    parseVariableType();
    readValue();
    resetVariableCallback();
    setupVariableCallback();
}

void QADSTIME::protCatchADSSymbolTableChanged()
{
    // The parent implementation must be called first
    QADSPLCVariable::protCatchADSSymbolTableChanged();
    parseVariableType();
    readValue();
    resetVariableCallback();
    setupVariableCallback();
}

void QADSTIME::readValue()
{
    if( !m_adsError )
    {
        QTime returnValue = QTime(0,0);
        quint32 tempVal = 0;
        syncReadRequest(amsPort(), ADSIGRP_SYM_VALBYHND, adsSymbolHandle(), adsSymbolSize(), &tempVal, Q_NULLPTR);
        returnValue = returnValue.addMSecs(tempVal);
        if ( !m_adsError )
        {
            privSetValue(returnValue);
        }
    }
}

const QTime &QADSTIME::value()
{
    // Read the value depending on the read operation mode.
    if( plcVariableReadOperation() == ON_DEMAND )
    {
        readValue();
    }
    return m_value;
}

void QADSTIME::initializeADSConnection()
{
    QADSPLCVariable::initializeADSConnection();
    parseVariableType();
    readValue();
    resetVariableCallback();
    setupVariableCallback();
}

void QADSTIME::setValue(const QTime &val)
{
    // Set value of variable if changed & no error
    if( !m_adsError && (val != value()) )
    {
        quint32 tempVal = QTime(0,0).msecsTo(val);
        syncWriteRequest(amsPort(), ADSIGRP_SYM_VALBYHND, adsSymbolHandle(), adsSymbolSize(), &tempVal);
    }
    if( !m_adsError && (plcVariableReadOperation() == ON_DEMAND) )
    {
       privSetValue(val);
    }
}

void QADSTIME::privSetValue(const QTime &val)
{
    if( val != m_value )
    {
        m_value = val;
        Q_EMIT valueChanged();
    }
}
