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
#include "qadsfloatingpointbase.h"
#include <QMutexLocker>
#include <QVector>
#if defined(USE_TWINCAT)
#include <windows.h>
#include "tcadsdef.h"
#include "tcadsapi.h"
#else
#include <AdsLib.h>
#endif

/************************************************************************************************/
/******************************** QADSFloatingPoint64 ***************************************/
/************************************************************************************************/

void QADSFloatingPoint64CallbackClassTemplate::setValue(double value)
{
    Q_EMIT valueChanged(value);
}

class QADSFloatingPoint64::QADSFloatingPoint64CallbackClass : public QADSFloatingPoint64CallbackClassTemplate
{
public:
#ifdef USE_TWINCAT
    static void __stdcall Callback(AmsAddr*, AdsNotificationHeader*, unsigned long);
#else
    static void Callback(const AmsAddr*, const AdsNotificationHeader*, uint32_t);
#endif
    static QMutex m_pointerVectorMutex;
    static QVector<QADSFloatingPoint64::QADSFloatingPoint64CallbackClass*> m_pointerVector;
};

QMutex QADSFloatingPoint64::QADSFloatingPoint64CallbackClass::m_pointerVectorMutex;
QVector<QADSFloatingPoint64::QADSFloatingPoint64CallbackClass*> QADSFloatingPoint64::QADSFloatingPoint64CallbackClass::m_pointerVector;

// Callback-function
#ifdef USE_TWINCAT
void __stdcall QADSFloatingPoint64::QADSFloatingPoint64CallbackClass::Callback(AmsAddr* pAddr, AdsNotificationHeader* pNotification, unsigned long hUser)
{
    Q_UNUSED(pAddr);
    QMutexLocker locker(&m_pointerVectorMutex);
    Q_UNUSED(locker);
    double value;
    QADSFloatingPoint64::QADSFloatingPoint64CallbackClass *sender = m_pointerVector[hUser];

    value = *(double *)pNotification->data;
    if( sender != Q_NULLPTR )
    {
        sender->setValue(value);
    }
}
#else
void QADSFloatingPoint64::QADSFloatingPoint64CallbackClass::Callback(const AmsAddr* pAddr, const AdsNotificationHeader* pNotification, uint32_t hUser)
{
    Q_UNUSED(pAddr);
    QMutexLocker locker(&m_pointerVectorMutex);
    Q_UNUSED(locker);
    double value;
    QADSFloatingPoint64::QADSFloatingPoint64CallbackClass *sender = m_pointerVector[hUser];

    const uint8_t* data = reinterpret_cast<const uint8_t*>(pNotification + 1);
    value = *(double *)data;
    if( sender != Q_NULLPTR )
    {
        sender->setValue(value);
    }
}
#endif

QADSFloatingPoint64::QADSFloatingPoint64(QObject *parent, quint16 amsport, const QString &amsnetid, const QString &amshost,
                                         const QString &var, PLCVariableReadOperation readop, unsigned long cycletime) :
    QADSPLCVariable(parent,amsport,amsnetid,amshost,var,readop,cycletime),
    m_value(0.0),
    m_adsFloatingPoint64Callback(Q_NULLPTR),
    m_variableCallBackNotificationHandle(0)
{
}

QADSFloatingPoint64::~QADSFloatingPoint64()
{
    resetVariableCallback();
}

void QADSFloatingPoint64::setupVariableCallback()
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
            m_adsErrorString = tr("QADSFloatingPoint64, Line ");
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
            if ( m_adsFloatingPoint64Callback == Q_NULLPTR )
            {
                m_adsFloatingPoint64Callback = new QADSFloatingPoint64::QADSFloatingPoint64CallbackClass();
                QADSFloatingPoint64::QADSFloatingPoint64CallbackClass::m_pointerVector.append(m_adsFloatingPoint64Callback);
            }

            // Get pointer index.
            hUser = QADSFloatingPoint64::QADSFloatingPoint64CallbackClass::m_pointerVector.size()-1;

            // set the attributes of the notification
            adsNotificationAttrib.cbLength = sizeof(double);
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
                m_adsErrorString = tr("QADSFloatingPoint64, Line ");
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
                        m_adsErrorString = tr("QADSFloatingPoint64, Line ");
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
                                                             QADSFloatingPoint64::QADSFloatingPoint64CallbackClass::Callback, hUser,
                                                             &m_variableCallBackNotificationHandle);
#else
                    uint32_t handle = 0;
                    nErr = AdsSyncAddDeviceNotificationReqEx(adsPort(), pAddr, ADSIGRP_SYM_VALBYHND,
                                                             adsSymbolHandle(), &adsNotificationAttrib,
                                                             QADSFloatingPoint64::QADSFloatingPoint64CallbackClass::Callback, hUser,
                                                             &handle);
                    m_variableCallBackNotificationHandle = handle;
#endif
                    if (nErr)
                    {
                        m_adsError = true;
                        m_adsErrorString = tr("QADSFloatingPoint64, Line ");
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
                        connect(m_adsFloatingPoint64Callback, SIGNAL(valueChanged(double)),
                                this, SLOT(privSetValue(double)), static_cast<Qt::ConnectionType>(Qt::QueuedConnection | Qt::UniqueConnection));
                    }
                }
            }
        }
    }
}

void QADSFloatingPoint64::resetVariableCallback()
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
    if( m_adsFloatingPoint64Callback )
    {
        QMutexLocker locker(&QADSFloatingPoint64::QADSFloatingPoint64CallbackClass::m_pointerVectorMutex);
        Q_UNUSED(locker);
        int index = QADSFloatingPoint64::QADSFloatingPoint64CallbackClass::m_pointerVector.indexOf(m_adsFloatingPoint64Callback);
        if (index >= 0)
        {
            QADSFloatingPoint64::QADSFloatingPoint64CallbackClass::m_pointerVector[index] = Q_NULLPTR;
        }
        delete m_adsFloatingPoint64Callback;
        m_adsFloatingPoint64Callback = Q_NULLPTR;
    }
}

void QADSFloatingPoint64::readValue()
{
    if( !m_adsError )
    {
        double returnValue = m_value;
        syncReadRequest(amsPort(), ADSIGRP_SYM_VALBYHND, adsSymbolHandle(), sizeof(returnValue), &returnValue, Q_NULLPTR);
        if ( !m_adsError )
        {
            privSetValue(returnValue);
        }
    }
}

double QADSFloatingPoint64::value()
{
    return m_value;
}

void QADSFloatingPoint64::initializeADSConnection()
{
    QADSPLCVariable::initializeADSConnection();
    parseVariableType();
    resetVariableCallback();
    setupVariableCallback();
}

void QADSFloatingPoint64::setValue(double value)
{
    privSetValue(value);
}

void QADSFloatingPoint64::privSetValue(double value)
{
    if( value != m_value )
    {
        m_value = value;
        Q_EMIT valueChanged();
    }
}

/************************************************************************************************/
/******************************** QADSFloatingPoint32 ***************************************/
/************************************************************************************************/

void QADSFloatingPoint32CallbackClassTemplate::setValue(float value)
{
    Q_EMIT valueChanged(value);
}

class QADSFloatingPoint32::QADSFloatingPoint32CallbackClass : public QADSFloatingPoint32CallbackClassTemplate
{
public:
#ifdef USE_TWINCAT
    static void __stdcall Callback(AmsAddr*, AdsNotificationHeader*, unsigned long);
#else
    static void Callback(const AmsAddr*, const AdsNotificationHeader*, uint32_t);
#endif
    static QMutex m_pointerVectorMutex;
    static QVector<QADSFloatingPoint32::QADSFloatingPoint32CallbackClass*> m_pointerVector;
};

QMutex QADSFloatingPoint32::QADSFloatingPoint32CallbackClass::m_pointerVectorMutex;
QVector<QADSFloatingPoint32::QADSFloatingPoint32CallbackClass*> QADSFloatingPoint32::QADSFloatingPoint32CallbackClass::m_pointerVector;

// Callback-function
#ifdef USE_TWINCAT
void __stdcall QADSFloatingPoint32::QADSFloatingPoint32CallbackClass::Callback(AmsAddr* pAddr, AdsNotificationHeader* pNotification, unsigned long hUser)
{
    Q_UNUSED(pAddr);
    QMutexLocker locker(&m_pointerVectorMutex);
    Q_UNUSED(locker);
    float value;
    QADSFloatingPoint32::QADSFloatingPoint32CallbackClass *sender = m_pointerVector[hUser];

    value = *(float *)pNotification->data;
    if( sender != Q_NULLPTR )
    {
        sender->setValue(value);
    }
}
#else
void QADSFloatingPoint32::QADSFloatingPoint32CallbackClass::Callback(const AmsAddr* pAddr, const AdsNotificationHeader* pNotification, uint32_t hUser)
{
    Q_UNUSED(pAddr);
    QMutexLocker locker(&m_pointerVectorMutex);
    Q_UNUSED(locker);
    float value;
    QADSFloatingPoint32::QADSFloatingPoint32CallbackClass *sender = m_pointerVector[hUser];

    const uint8_t* data = reinterpret_cast<const uint8_t*>(pNotification + 1);
    value = *(float *)data;
    if( sender != Q_NULLPTR )
    {
        sender->setValue(value);
    }
}
#endif

QADSFloatingPoint32::QADSFloatingPoint32(QObject *parent, quint16 amsport, const QString &amsnetid, const QString &amshost,
                                         const QString &var, PLCVariableReadOperation readop, unsigned long cycletime) :
    QADSPLCVariable(parent,amsport,amsnetid,amshost,var,readop,cycletime),
    m_value(0.0),
    m_adsFloatingPoint32Callback(Q_NULLPTR),
    m_variableCallBackNotificationHandle(0)
{
}

QADSFloatingPoint32::~QADSFloatingPoint32()
{
    resetVariableCallback();
}

void QADSFloatingPoint32::setupVariableCallback()
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
            m_adsErrorString = tr("QADSFloatingPoint32, Line ");
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
            if ( m_adsFloatingPoint32Callback == Q_NULLPTR )
            {
                m_adsFloatingPoint32Callback = new QADSFloatingPoint32::QADSFloatingPoint32CallbackClass();
                QADSFloatingPoint32::QADSFloatingPoint32CallbackClass::m_pointerVector.append(m_adsFloatingPoint32Callback);
            }

            // Get pointer index.
            hUser = QADSFloatingPoint32::QADSFloatingPoint32CallbackClass::m_pointerVector.size()-1;

            // set the attributes of the notification
            adsNotificationAttrib.cbLength = sizeof(float);
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
                m_adsErrorString = tr("QADSFloatingPoint32, Line ");
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
                        m_adsErrorString = tr("QADSFloatingPoint32, Line ");
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
                                                             QADSFloatingPoint32::QADSFloatingPoint32CallbackClass::Callback, hUser,
                                                             &m_variableCallBackNotificationHandle);
#else
                    uint32_t handle = 0;
                    nErr = AdsSyncAddDeviceNotificationReqEx(adsPort(), pAddr, ADSIGRP_SYM_VALBYHND,
                                                             adsSymbolHandle(), &adsNotificationAttrib,
                                                             QADSFloatingPoint32::QADSFloatingPoint32CallbackClass::Callback, hUser,
                                                             &handle);
                    m_variableCallBackNotificationHandle = handle;
#endif
                    if (nErr)
                    {
                        m_adsError = true;
                        m_adsErrorString = tr("QADSFloatingPoint32, Line ");
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
                        connect(m_adsFloatingPoint32Callback, SIGNAL(valueChanged(float)),
                                this, SLOT(privSetValue(float)), static_cast<Qt::ConnectionType>(Qt::QueuedConnection | Qt::UniqueConnection));
                    }
                }
            }
        }
    }
}

void QADSFloatingPoint32::resetVariableCallback()
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
    if( m_adsFloatingPoint32Callback )
    {
        QMutexLocker locker(&QADSFloatingPoint32::QADSFloatingPoint32CallbackClass::m_pointerVectorMutex);
        Q_UNUSED(locker);
        int index = QADSFloatingPoint32::QADSFloatingPoint32CallbackClass::m_pointerVector.indexOf(m_adsFloatingPoint32Callback);
        if (index >= 0)
        {
            QADSFloatingPoint32::QADSFloatingPoint32CallbackClass::m_pointerVector[index] = Q_NULLPTR;
        }
        delete m_adsFloatingPoint32Callback;
        m_adsFloatingPoint32Callback = Q_NULLPTR;
    }
}

void QADSFloatingPoint32::readValue()
{
    if( !m_adsError )
    {
        float returnValue = m_value;
        syncReadRequest(amsPort(), ADSIGRP_SYM_VALBYHND, adsSymbolHandle(), sizeof(returnValue), &returnValue, Q_NULLPTR);
        if ( !m_adsError )
        {
            privSetValue(returnValue);
        }
    }
}

float QADSFloatingPoint32::value()
{
    return m_value;
}

void QADSFloatingPoint32::initializeADSConnection()
{
    QADSPLCVariable::initializeADSConnection();
    parseVariableType();
    resetVariableCallback();
    setupVariableCallback();
}

void QADSFloatingPoint32::setValue(float value)
{
    privSetValue(value);
}

void QADSFloatingPoint32::privSetValue(float value)
{
    if( value != m_value )
    {
        m_value = value;
        Q_EMIT valueChanged();
    }
}


