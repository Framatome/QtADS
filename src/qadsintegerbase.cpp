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
#include "qadsintegerbase.h"
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
/******************************** QADSUnsignedInteger8 **************************************/
/************************************************************************************************/
void QADSUnsignedInteger8CallbackClassTemplate::setValue(quint8 value)
{
    Q_EMIT valueChanged(value);
}

class QADSUnsignedInteger8::QADSUnsignedInteger8CallbackClass : public QADSUnsignedInteger8CallbackClassTemplate
{
public:
#ifdef USE_TWINCAT
    static void __stdcall Callback(AmsAddr*, AdsNotificationHeader*, unsigned long);
#else
    static void Callback(const AmsAddr*, const AdsNotificationHeader*, uint32_t);
#endif
    static QMutex m_pointerVectorMutex;
    static QVector<QADSUnsignedInteger8::QADSUnsignedInteger8CallbackClass*> m_pointerVector;
};

QMutex QADSUnsignedInteger8::QADSUnsignedInteger8CallbackClass::m_pointerVectorMutex;
QVector<QADSUnsignedInteger8::QADSUnsignedInteger8CallbackClass*> QADSUnsignedInteger8::QADSUnsignedInteger8CallbackClass::m_pointerVector;

// Callback-function
#ifdef USE_TWINCAT
void __stdcall QADSUnsignedInteger8::QADSUnsignedInteger8CallbackClass::Callback(AmsAddr* pAddr, AdsNotificationHeader* pNotification, unsigned long hUser)
{
    Q_UNUSED(pAddr);
    QMutexLocker locker(&m_pointerVectorMutex);
    Q_UNUSED(locker);
    quint8 value;
    QADSUnsignedInteger8::QADSUnsignedInteger8CallbackClass *sender = m_pointerVector[hUser];

    value = *(quint8 *)pNotification->data;
    if( sender != Q_NULLPTR )
    {
        sender->setValue(value);
    }
}
#else
void QADSUnsignedInteger8::QADSUnsignedInteger8CallbackClass::Callback(const AmsAddr* pAddr, const AdsNotificationHeader* pNotification, uint32_t hUser)
{
    Q_UNUSED(pAddr);
    QMutexLocker locker(&m_pointerVectorMutex);
    Q_UNUSED(locker);
    quint8 value;
    QADSUnsignedInteger8::QADSUnsignedInteger8CallbackClass *sender = m_pointerVector[hUser];

    const uint8_t* data = reinterpret_cast<const uint8_t*>(pNotification + 1);
    value = *(quint8 *)data;
    if( sender != Q_NULLPTR )
    {
        sender->setValue(value);
    }
}
#endif

QADSUnsignedInteger8::QADSUnsignedInteger8(QObject *parent, quint16 amsport, const QString &amsnetid, const QString &amshost,
                                           const QString &var, PLCVariableReadOperation readop, unsigned long cycletime) :
    QADSPLCVariable(parent,amsport,amsnetid,amshost,var,readop,cycletime),
    m_value(0),
    m_adsUnsignedInteger8Callback(Q_NULLPTR),
    m_variableCallBackNotificationHandle(0)
{
}

QADSUnsignedInteger8::~QADSUnsignedInteger8()
{
    resetVariableCallback();
}

void QADSUnsignedInteger8::setupVariableCallback()
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
            m_adsErrorString = tr("QADSUnsignedInteger8, Line ");
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
            if ( m_adsUnsignedInteger8Callback == Q_NULLPTR )
            {
                m_adsUnsignedInteger8Callback = new QADSUnsignedInteger8::QADSUnsignedInteger8CallbackClass();
                QADSUnsignedInteger8::QADSUnsignedInteger8CallbackClass::m_pointerVector.append(m_adsUnsignedInteger8Callback);
            }

            // Get pointer index.
            hUser = QADSUnsignedInteger8::QADSUnsignedInteger8CallbackClass::m_pointerVector.size()-1;

            // set the attributes of the notification
            adsNotificationAttrib.cbLength = sizeof(quint8);
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
                m_adsErrorString = tr("QADSUnsignedInteger8, Line ");
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
                        m_adsErrorString = tr("QADSUnsignedInteger8, Line ");
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
                                                             QADSUnsignedInteger8::QADSUnsignedInteger8CallbackClass::Callback, hUser,
                                                             &m_variableCallBackNotificationHandle);
#else
                    uint32_t handle = 0;
                    nErr = AdsSyncAddDeviceNotificationReqEx(adsPort(), pAddr, ADSIGRP_SYM_VALBYHND,
                                                             adsSymbolHandle(), &adsNotificationAttrib,
                                                             QADSUnsignedInteger8::QADSUnsignedInteger8CallbackClass::Callback, hUser,
                                                             &handle);
                    m_variableCallBackNotificationHandle = handle;
#endif
                    if (nErr)
                    {
                        m_adsError = true;
                        m_adsErrorString = tr("QADSUnsignedInteger8, Line ");
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
                        connect(m_adsUnsignedInteger8Callback, SIGNAL(valueChanged(quint8)),
                                this, SLOT(privSetValue(quint8)), static_cast<Qt::ConnectionType>(Qt::QueuedConnection | Qt::UniqueConnection));
                    }
                }
            }
        }
    }
}

void QADSUnsignedInteger8::resetVariableCallback()
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
    if( m_adsUnsignedInteger8Callback )
    {
        QMutexLocker locker(&QADSUnsignedInteger8::QADSUnsignedInteger8CallbackClass::m_pointerVectorMutex);
        Q_UNUSED(locker);
        int index = QADSUnsignedInteger8::QADSUnsignedInteger8CallbackClass::m_pointerVector.indexOf(m_adsUnsignedInteger8Callback);
        if (index >= 0)
        {
            QADSUnsignedInteger8::QADSUnsignedInteger8CallbackClass::m_pointerVector[index] = Q_NULLPTR;
        }
        delete m_adsUnsignedInteger8Callback;
        m_adsUnsignedInteger8Callback = Q_NULLPTR;
    }
}

void QADSUnsignedInteger8::readValue()
{
    if( !m_adsError )
    {
        quint8 returnValue = m_value;
        syncReadRequest(amsPort(), ADSIGRP_SYM_VALBYHND, adsSymbolHandle(), sizeof(returnValue), &returnValue, Q_NULLPTR);
        if ( !m_adsError )
        {
            privSetValue(returnValue);
        }
    }
}

quint8 QADSUnsignedInteger8::value()
{
    return m_value;
}

void QADSUnsignedInteger8::initializeADSConnection()
{
    QADSPLCVariable::initializeADSConnection();
    parseVariableType();
    resetVariableCallback();
    setupVariableCallback();
}

void QADSUnsignedInteger8::setValue(quint8 value)
{
    privSetValue(value);
}

void QADSUnsignedInteger8::privSetValue(quint8 value)
{
    if( value != m_value )
    {
        m_value = value;
        Q_EMIT valueChanged();
    }
}

/************************************************************************************************/
/******************************** QADSInteger8 **********************************************/
/************************************************************************************************/

void QADSInteger8CallbackClassTemplate::setValue(qint8 value)
{
    Q_EMIT valueChanged(value);
}

class QADSInteger8::QADSInteger8CallbackClass : public QADSInteger8CallbackClassTemplate
{
public:
#ifdef USE_TWINCAT
    static void __stdcall Callback(AmsAddr*, AdsNotificationHeader*, unsigned long);
#else
    static void Callback(const AmsAddr*, const AdsNotificationHeader*, uint32_t);
#endif
    static QMutex m_pointerVectorMutex;
    static QVector<QADSInteger8::QADSInteger8CallbackClass*> m_pointerVector;
};

QMutex QADSInteger8::QADSInteger8CallbackClass::m_pointerVectorMutex;
QVector<QADSInteger8::QADSInteger8CallbackClass*> QADSInteger8::QADSInteger8CallbackClass::m_pointerVector;

// Callback-function
#ifdef USE_TWINCAT
void __stdcall QADSInteger8::QADSInteger8CallbackClass::Callback(AmsAddr* pAddr, AdsNotificationHeader* pNotification, unsigned long hUser)
{
    Q_UNUSED(pAddr);
    QMutexLocker locker(&m_pointerVectorMutex);
    Q_UNUSED(locker);
    qint8 value;
    QADSInteger8::QADSInteger8CallbackClass *sender = m_pointerVector[hUser];

    value = *(qint8 *)pNotification->data;
    if( sender != Q_NULLPTR )
    {
        sender->setValue(value);
    }
}
#else
void QADSInteger8::QADSInteger8CallbackClass::Callback(const AmsAddr* pAddr, const AdsNotificationHeader* pNotification, uint32_t hUser)
{
    Q_UNUSED(pAddr);
    QMutexLocker locker(&m_pointerVectorMutex);
    Q_UNUSED(locker);
    qint8 value;
    QADSInteger8::QADSInteger8CallbackClass *sender = m_pointerVector[hUser];

    const uint8_t* data = reinterpret_cast<const uint8_t*>(pNotification + 1);
    value = *(qint8 *)data;
    if( sender != Q_NULLPTR )
    {
        sender->setValue(value);
    }
}
#endif

QADSInteger8::QADSInteger8(QObject *parent, quint16 amsport, const QString &amsnetid, const QString &amshost,
                           const QString &var, PLCVariableReadOperation readop, unsigned long cycletime) :
    QADSPLCVariable(parent,amsport,amsnetid,amshost,var,readop,cycletime),
    m_value(0),
    m_adsInteger8Callback(Q_NULLPTR),
    m_variableCallBackNotificationHandle(0)
{
}

QADSInteger8::~QADSInteger8()
{
    resetVariableCallback();
}

void QADSInteger8::setupVariableCallback()
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
            m_adsErrorString = tr("QADSInteger8, Line ");
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
            if ( m_adsInteger8Callback == Q_NULLPTR )
            {
                m_adsInteger8Callback = new QADSInteger8::QADSInteger8CallbackClass();
                QADSInteger8::QADSInteger8CallbackClass::m_pointerVector.append(m_adsInteger8Callback);
            }

            // Get pointer index.
            hUser = QADSInteger8::QADSInteger8CallbackClass::m_pointerVector.size()-1;

            // set the attributes of the notification
            adsNotificationAttrib.cbLength = sizeof(qint8);
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
                m_adsErrorString = tr("QADSInteger8, Line ");
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
                        m_adsErrorString = tr("QADSInteger8, Line ");
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
                                                             QADSInteger8::QADSInteger8CallbackClass::Callback, hUser,
                                                             &m_variableCallBackNotificationHandle);
#else
                    uint32_t handle = 0;
                    nErr = AdsSyncAddDeviceNotificationReqEx(adsPort(), pAddr, ADSIGRP_SYM_VALBYHND,
                                                             adsSymbolHandle(), &adsNotificationAttrib,
                                                             QADSInteger8::QADSInteger8CallbackClass::Callback, hUser,
                                                             &handle);
                    m_variableCallBackNotificationHandle = handle;
#endif
                    if (nErr)
                    {
                        m_adsError = true;
                        m_adsErrorString = tr("QADSInteger8, Line ");
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
                        connect(m_adsInteger8Callback, SIGNAL(valueChanged(qint8)),
                                this, SLOT(privSetValue(qint8)), static_cast<Qt::ConnectionType>(Qt::QueuedConnection | Qt::UniqueConnection));
                    }
                }
            }
        }
    }
}

void QADSInteger8::resetVariableCallback()
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
    if( m_adsInteger8Callback )
    {
        QMutexLocker locker(&QADSInteger8::QADSInteger8CallbackClass::m_pointerVectorMutex);
        Q_UNUSED(locker);
        int index = QADSInteger8::QADSInteger8CallbackClass::m_pointerVector.indexOf(m_adsInteger8Callback);
        if (index >= 0)
        {
            QADSInteger8::QADSInteger8CallbackClass::m_pointerVector[index] = Q_NULLPTR;
        }
        delete m_adsInteger8Callback;
        m_adsInteger8Callback = Q_NULLPTR;
    }
}

void QADSInteger8::readValue()
{
    // Read the value depending on the read operation mode.
    if( !m_adsError )
    {
        qint8 returnValue = m_value;
        syncReadRequest(amsPort(), ADSIGRP_SYM_VALBYHND, adsSymbolHandle(), sizeof(returnValue), &returnValue, Q_NULLPTR);
        if ( !m_adsError )
        {
            privSetValue(returnValue);
        }
    }
}

qint8 QADSInteger8::value()
{
    return m_value;
}

void QADSInteger8::initializeADSConnection()
{
    QADSPLCVariable::initializeADSConnection();
    parseVariableType();
    resetVariableCallback();
    setupVariableCallback();
}

void QADSInteger8::setValue(qint8 value)
{
    privSetValue(value);
}

void QADSInteger8::privSetValue(qint8 value)
{
    if( value != m_value )
    {
        m_value = value;
        Q_EMIT valueChanged();
    }
}

/************************************************************************************************/
/******************************** QADSUnsignedInteger16 *************************************/
/************************************************************************************************/
void QADSUnsignedInteger16CallbackClassTemplate::setValue(quint16 value)
{
    Q_EMIT valueChanged(value);
}

class QADSUnsignedInteger16::QADSUnsignedInteger16CallbackClass : public QADSUnsignedInteger16CallbackClassTemplate
{
public:
#ifdef USE_TWINCAT
    static void __stdcall Callback(AmsAddr*, AdsNotificationHeader*, unsigned long);
#else
    static void Callback(const AmsAddr*, const AdsNotificationHeader*, uint32_t);
#endif
    static QMutex m_pointerVectorMutex;
    static QVector<QADSUnsignedInteger16::QADSUnsignedInteger16CallbackClass*> m_pointerVector;
};

QMutex QADSUnsignedInteger16::QADSUnsignedInteger16CallbackClass::m_pointerVectorMutex;
QVector<QADSUnsignedInteger16::QADSUnsignedInteger16CallbackClass*> QADSUnsignedInteger16::QADSUnsignedInteger16CallbackClass::m_pointerVector;

// Callback-function
#ifdef USE_TWINCAT
void __stdcall QADSUnsignedInteger16::QADSUnsignedInteger16CallbackClass::Callback(AmsAddr* pAddr, AdsNotificationHeader* pNotification, unsigned long hUser)
{
    Q_UNUSED(pAddr);
    QMutexLocker locker(&m_pointerVectorMutex);
    Q_UNUSED(locker);
    quint16 value;
    QADSUnsignedInteger16::QADSUnsignedInteger16CallbackClass *sender = m_pointerVector[hUser];

    value = *(quint16 *)pNotification->data;
    if( sender != Q_NULLPTR )
    {
        sender->setValue(value);
    }
}
#else
void QADSUnsignedInteger16::QADSUnsignedInteger16CallbackClass::Callback(const AmsAddr* pAddr, const AdsNotificationHeader* pNotification, uint32_t hUser)
{
    Q_UNUSED(pAddr);
    QMutexLocker locker(&m_pointerVectorMutex);
    Q_UNUSED(locker);
    quint16 value;
    QADSUnsignedInteger16::QADSUnsignedInteger16CallbackClass *sender = m_pointerVector[hUser];

    const uint8_t* data = reinterpret_cast<const uint8_t*>(pNotification + 1);
    value = *(quint16 *)data;
    if( sender != Q_NULLPTR )
    {
        sender->setValue(value);
    }
}
#endif

QADSUnsignedInteger16::QADSUnsignedInteger16(QObject *parent, quint16 amsport, const QString &amsnetid, const QString &amshost,
                                             const QString &var, PLCVariableReadOperation readop, unsigned long cycletime) :
    QADSPLCVariable(parent,amsport,amsnetid,amshost,var,readop,cycletime),
    m_value(0),
    m_adsUnsignedInteger16Callback(Q_NULLPTR),
    m_variableCallBackNotificationHandle(0)
{
}

QADSUnsignedInteger16::~QADSUnsignedInteger16()
{
    resetVariableCallback();
}

void QADSUnsignedInteger16::setupVariableCallback()
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
            m_adsErrorString = tr("QADSUnsignedInteger16, Line ");
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
            if ( m_adsUnsignedInteger16Callback == Q_NULLPTR )
            {
                m_adsUnsignedInteger16Callback = new QADSUnsignedInteger16::QADSUnsignedInteger16CallbackClass();
                QADSUnsignedInteger16::QADSUnsignedInteger16CallbackClass::m_pointerVector.append(m_adsUnsignedInteger16Callback);
            }

            // Get pointer index.
            hUser = QADSUnsignedInteger16::QADSUnsignedInteger16CallbackClass::m_pointerVector.size()-1;

            // set the attributes of the notification
            adsNotificationAttrib.cbLength = sizeof(quint16);
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
                m_adsErrorString = tr("QADSUnsignedInteger16, Line ");
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
                        m_adsErrorString = tr("QADSUnsignedInteger16, Line ");
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
                                                             QADSUnsignedInteger16::QADSUnsignedInteger16CallbackClass::Callback, hUser,
                                                             &m_variableCallBackNotificationHandle);
#else
                    uint32_t handle = 0;
                    nErr = AdsSyncAddDeviceNotificationReqEx(adsPort(), pAddr, ADSIGRP_SYM_VALBYHND,
                                                             adsSymbolHandle(), &adsNotificationAttrib,
                                                             QADSUnsignedInteger16::QADSUnsignedInteger16CallbackClass::Callback, hUser,
                                                             &handle);
                    m_variableCallBackNotificationHandle = handle;
#endif
                    if (nErr)
                    {
                        m_adsError = true;
                        m_adsErrorString = tr("QADSUnsignedInteger16, Line ");
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
                        connect(m_adsUnsignedInteger16Callback, SIGNAL(valueChanged(quint16)),
                                this, SLOT(privSetValue(quint16)), static_cast<Qt::ConnectionType>(Qt::QueuedConnection | Qt::UniqueConnection));
                    }
                }
            }
        }
    }
}

void QADSUnsignedInteger16::resetVariableCallback()
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
    if( m_adsUnsignedInteger16Callback )
    {
        QMutexLocker locker(&QADSUnsignedInteger16::QADSUnsignedInteger16CallbackClass::m_pointerVectorMutex);
        Q_UNUSED(locker);
        int index = QADSUnsignedInteger16::QADSUnsignedInteger16CallbackClass::m_pointerVector.indexOf(m_adsUnsignedInteger16Callback);
        if (index >= 0)
        {
            QADSUnsignedInteger16::QADSUnsignedInteger16CallbackClass::m_pointerVector[index] = Q_NULLPTR;
        }
        delete m_adsUnsignedInteger16Callback;
        m_adsUnsignedInteger16Callback = Q_NULLPTR;
    }
}

void QADSUnsignedInteger16::readValue()
{
    if( !m_adsError )
    {
        quint16 returnValue = m_value;
        syncReadRequest(amsPort(), ADSIGRP_SYM_VALBYHND, adsSymbolHandle(), sizeof(returnValue), &returnValue, Q_NULLPTR);
        if ( !m_adsError )
        {
            privSetValue(returnValue);
        }
    }
}

quint16 QADSUnsignedInteger16::value()
{
    return m_value;
}

void QADSUnsignedInteger16::initializeADSConnection()
{
    QADSPLCVariable::initializeADSConnection();
    parseVariableType();
    resetVariableCallback();
    setupVariableCallback();
}

void QADSUnsignedInteger16::setValue(quint16 value)
{
    privSetValue(value);
}

void QADSUnsignedInteger16::privSetValue(quint16 value)
{
    if( value != m_value )
    {
        m_value = value;
        Q_EMIT valueChanged();
    }
}

/************************************************************************************************/
/******************************** QADSInteger16 *********************************************/
/************************************************************************************************/
void QADSInteger16CallbackClassTemplate::setValue(qint16 value)
{
    Q_EMIT valueChanged(value);
}

class QADSInteger16::QADSInteger16CallbackClass : public QADSInteger16CallbackClassTemplate
{
public:
#ifdef USE_TWINCAT
    static void __stdcall Callback(AmsAddr*, AdsNotificationHeader*, unsigned long);
#else
    static void Callback(const AmsAddr*, const AdsNotificationHeader*, uint32_t);
#endif
    static QMutex m_pointerVectorMutex;
    static QVector<QADSInteger16::QADSInteger16CallbackClass*> m_pointerVector;
};

QMutex QADSInteger16::QADSInteger16CallbackClass::m_pointerVectorMutex;
QVector<QADSInteger16::QADSInteger16CallbackClass*> QADSInteger16::QADSInteger16CallbackClass::m_pointerVector;

// Callback-function
#ifdef USE_TWINCAT
void __stdcall QADSInteger16::QADSInteger16CallbackClass::Callback(AmsAddr* pAddr, AdsNotificationHeader* pNotification, unsigned long hUser)
{
    Q_UNUSED(pAddr);
    QMutexLocker locker(&m_pointerVectorMutex);
    Q_UNUSED(locker);
    qint16 value;
    QADSInteger16::QADSInteger16CallbackClass *sender = m_pointerVector[hUser];

    value = *(qint16 *)pNotification->data;
    if( sender != Q_NULLPTR )
    {
        sender->setValue(value);
    }
}
#else
void QADSInteger16::QADSInteger16CallbackClass::Callback(const AmsAddr* pAddr, const AdsNotificationHeader* pNotification, uint32_t hUser)
{
    Q_UNUSED(pAddr);
    qint16 value;
    QADSInteger16::QADSInteger16CallbackClass *sender = m_pointerVector[hUser];

    const uint8_t* data = reinterpret_cast<const uint8_t*>(pNotification + 1);
    value = *(qint16 *)data;
    if( sender != Q_NULLPTR )
    {
        sender->setValue(value);
    }
}
#endif

QADSInteger16::QADSInteger16(QObject *parent, quint16 amsport, const QString &amsnetid, const QString &amshost,
                             const QString &var, PLCVariableReadOperation readop, unsigned long cycletime) :
    QADSPLCVariable(parent,amsport,amsnetid,amshost,var,readop,cycletime),
    m_value(0),
    m_adsInteger16Callback(Q_NULLPTR),
    m_variableCallBackNotificationHandle(0)
{
}

QADSInteger16::~QADSInteger16()
{
    resetVariableCallback();
}

void QADSInteger16::setupVariableCallback()
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
            m_adsErrorString = tr("QADSInteger16, Line ");
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
            if ( m_adsInteger16Callback == Q_NULLPTR )
            {
                m_adsInteger16Callback = new QADSInteger16::QADSInteger16CallbackClass();
                QADSInteger16::QADSInteger16CallbackClass::m_pointerVector.append(m_adsInteger16Callback);
            }

            // Get pointer index.
            hUser = QADSInteger16::QADSInteger16CallbackClass::m_pointerVector.size()-1;

            // set the attributes of the notification
            adsNotificationAttrib.cbLength = sizeof(qint16);
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
                m_adsErrorString = tr("QADSInteger16, Line ");
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
                        m_adsErrorString = tr("QADSInteger16, Line ");
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
                                                             QADSInteger16::QADSInteger16CallbackClass::Callback, hUser,
                                                             &m_variableCallBackNotificationHandle);
#else
                    uint32_t handle = 0;
                    nErr = AdsSyncAddDeviceNotificationReqEx(adsPort(), pAddr, ADSIGRP_SYM_VALBYHND,
                                                             adsSymbolHandle(), &adsNotificationAttrib,
                                                             QADSInteger16::QADSInteger16CallbackClass::Callback, hUser,
                                                             &handle);
                    m_variableCallBackNotificationHandle = handle;
#endif
                    if (nErr)
                    {
                        m_adsError = true;
                        m_adsErrorString = tr("QADSInteger16, Line ");
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
                        connect(m_adsInteger16Callback, SIGNAL(valueChanged(qint16)),
                                this, SLOT(privSetValue(qint16)), static_cast<Qt::ConnectionType>(Qt::QueuedConnection | Qt::UniqueConnection));
                    }
                }
            }
        }
    }
}

void QADSInteger16::resetVariableCallback()
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
    if( m_adsInteger16Callback )
    {
        QMutexLocker locker(&QADSInteger16::QADSInteger16CallbackClass::m_pointerVectorMutex);
        Q_UNUSED(locker);
        int index = QADSInteger16::QADSInteger16CallbackClass::m_pointerVector.indexOf(m_adsInteger16Callback);
        if (index >= 0)
        {
            QADSInteger16::QADSInteger16CallbackClass::m_pointerVector[index] = Q_NULLPTR;
        }
        delete m_adsInteger16Callback;
        m_adsInteger16Callback = Q_NULLPTR;
    }
}

void QADSInteger16::readValue()
{
    if( !m_adsError )
    {
        qint16 returnValue = m_value;
        syncReadRequest(amsPort(), ADSIGRP_SYM_VALBYHND, adsSymbolHandle(), sizeof(returnValue), &returnValue, Q_NULLPTR);
        if ( !m_adsError )
        {
            privSetValue(returnValue);
        }
    }
}

qint16 QADSInteger16::value()
{
    return m_value;
}

void QADSInteger16::initializeADSConnection()
{
    QADSPLCVariable::initializeADSConnection();
    parseVariableType();
    resetVariableCallback();
    setupVariableCallback();
}

void QADSInteger16::setValue(qint16 value)
{
    privSetValue(value);
}

void QADSInteger16::privSetValue(qint16 value)
{
    if( value != m_value )
    {
        m_value = value;
        Q_EMIT valueChanged();
    }
}

/************************************************************************************************/
/******************************** QADSUnsignedInteger32 *************************************/
/************************************************************************************************/

void QADSUnsignedInteger32CallbackClassTemplate::setValue(quint32 value)
{
    Q_EMIT valueChanged(value);
}

class QADSUnsignedInteger32::QADSUnsignedInteger32CallbackClass : public QADSUnsignedInteger32CallbackClassTemplate
{
public:
#ifdef USE_TWINCAT
    static void __stdcall Callback(AmsAddr*, AdsNotificationHeader*, unsigned long);
#else
    static void Callback(const AmsAddr*, const AdsNotificationHeader*, uint32_t);
#endif
    static QMutex m_pointerVectorMutex;
    static QVector<QADSUnsignedInteger32::QADSUnsignedInteger32CallbackClass*> m_pointerVector;
};

QMutex QADSUnsignedInteger32::QADSUnsignedInteger32CallbackClass::m_pointerVectorMutex;
QVector<QADSUnsignedInteger32::QADSUnsignedInteger32CallbackClass*> QADSUnsignedInteger32::QADSUnsignedInteger32CallbackClass::m_pointerVector;

// Callback-function
#ifdef USE_TWINCAT
void __stdcall QADSUnsignedInteger32::QADSUnsignedInteger32CallbackClass::Callback(AmsAddr* pAddr, AdsNotificationHeader* pNotification, unsigned long hUser)
{
    Q_UNUSED(pAddr);
    QMutexLocker locker(&m_pointerVectorMutex);
    Q_UNUSED(locker);
    quint32 value;
    QADSUnsignedInteger32::QADSUnsignedInteger32CallbackClass *sender = m_pointerVector[hUser];

    value = *(quint32 *)pNotification->data;
    if( sender != Q_NULLPTR )
    {
        sender->setValue(value);
    }
}
#else
void QADSUnsignedInteger32::QADSUnsignedInteger32CallbackClass::Callback(const AmsAddr* pAddr, const AdsNotificationHeader* pNotification, uint32_t hUser)
{
    Q_UNUSED(pAddr);
    QMutexLocker locker(&m_pointerVectorMutex);
    Q_UNUSED(locker);
    quint32 value;
    QADSUnsignedInteger32::QADSUnsignedInteger32CallbackClass *sender = m_pointerVector[hUser];

    const uint8_t* data = reinterpret_cast<const uint8_t*>(pNotification + 1);
    value = *(quint32 *)data;
    if( sender != Q_NULLPTR )
    {
        sender->setValue(value);
    }
}
#endif

QADSUnsignedInteger32::QADSUnsignedInteger32(QObject *parent, quint16 amsport, const QString &amsnetid, const QString &amshost,
                                             const QString &var, PLCVariableReadOperation readop, unsigned long cycletime) :
    QADSPLCVariable(parent,amsport,amsnetid,amshost,var,readop,cycletime),
    m_value(0),
    m_adsUnsignedInteger32Callback(Q_NULLPTR),
    m_variableCallBackNotificationHandle(0)
{
}

QADSUnsignedInteger32::~QADSUnsignedInteger32()
{
    resetVariableCallback();
}

void QADSUnsignedInteger32::setupVariableCallback()
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
            m_adsErrorString = tr("QADSUnsignedInteger32, Line ");
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
            if ( m_adsUnsignedInteger32Callback == Q_NULLPTR )
            {
                m_adsUnsignedInteger32Callback = new QADSUnsignedInteger32::QADSUnsignedInteger32CallbackClass();
                QADSUnsignedInteger32::QADSUnsignedInteger32CallbackClass::m_pointerVector.append(m_adsUnsignedInteger32Callback);
            }

            // Get pointer index.
            hUser = QADSUnsignedInteger32::QADSUnsignedInteger32CallbackClass::m_pointerVector.size()-1;

            // set the attributes of the notification
            adsNotificationAttrib.cbLength = sizeof(quint32);
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
                m_adsErrorString = tr("QADSUnsignedInteger32, Line ");
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
                        m_adsErrorString = tr("QADSUnsignedInteger32, Line ");
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
                                                             QADSUnsignedInteger32::QADSUnsignedInteger32CallbackClass::Callback, hUser,
                                                             &m_variableCallBackNotificationHandle);
#else
                    uint32_t handle = 0;
                    nErr = AdsSyncAddDeviceNotificationReqEx(adsPort(), pAddr, ADSIGRP_SYM_VALBYHND,
                                                             adsSymbolHandle(), &adsNotificationAttrib,
                                                             QADSUnsignedInteger32::QADSUnsignedInteger32CallbackClass::Callback, hUser,
                                                             &handle);
                    m_variableCallBackNotificationHandle = handle;
#endif
                    if (nErr)
                    {
                        m_adsError = true;
                        m_adsErrorString = tr("QADSUnsignedInteger32, Line ");
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
                        connect(m_adsUnsignedInteger32Callback, SIGNAL(valueChanged(quint32)),
                                this, SLOT(privSetValue(quint32)), static_cast<Qt::ConnectionType>(Qt::QueuedConnection | Qt::UniqueConnection));
                    }
                }
            }
        }
    }
}

void QADSUnsignedInteger32::resetVariableCallback()
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
    if( m_adsUnsignedInteger32Callback )
    {
        QMutexLocker locker(&QADSUnsignedInteger32::QADSUnsignedInteger32CallbackClass::m_pointerVectorMutex);
        Q_UNUSED(locker);
        int index = QADSUnsignedInteger32::QADSUnsignedInteger32CallbackClass::m_pointerVector.indexOf(m_adsUnsignedInteger32Callback);
        if (index >= 0)
        {
            QADSUnsignedInteger32::QADSUnsignedInteger32CallbackClass::m_pointerVector[index] = Q_NULLPTR;
        }
        delete m_adsUnsignedInteger32Callback;
        m_adsUnsignedInteger32Callback = Q_NULLPTR;
    }
}

void QADSUnsignedInteger32::readValue()
{
    if( !m_adsError )
    {
        quint32 returnValue = m_value;
        syncReadRequest(amsPort(), ADSIGRP_SYM_VALBYHND, adsSymbolHandle(), sizeof(returnValue), &returnValue, Q_NULLPTR);
        if ( !m_adsError )
        {
            privSetValue(returnValue);
        }
    }
}

quint32 QADSUnsignedInteger32::value()
{
    return m_value;
}

void QADSUnsignedInteger32::initializeADSConnection()
{
    QADSPLCVariable::initializeADSConnection();
    parseVariableType();
    resetVariableCallback();
    setupVariableCallback();
}

void QADSUnsignedInteger32::setValue(quint32 value)
{
    privSetValue(value);
}

void QADSUnsignedInteger32::privSetValue(quint32 value)
{
    if( value != m_value )
    {
        m_value = value;
        Q_EMIT valueChanged();
    }
}

/************************************************************************************************/
/******************************** QADSInteger32 *********************************************/
/************************************************************************************************/

void QADSInteger32CallbackClassTemplate::setValue(qint32 value)
{
    Q_EMIT valueChanged(value);
}

class QADSInteger32::QADSInteger32CallbackClass : public QADSInteger32CallbackClassTemplate
{
public:
#ifdef USE_TWINCAT
    static void __stdcall Callback(AmsAddr*, AdsNotificationHeader*, unsigned long);
#else
    static void Callback(const AmsAddr*, const AdsNotificationHeader*, uint32_t);
#endif
    static QMutex m_pointerVectorMutex;
    static QVector<QADSInteger32::QADSInteger32CallbackClass*> m_pointerVector;
};

QMutex QADSInteger32::QADSInteger32CallbackClass::m_pointerVectorMutex;
QVector<QADSInteger32::QADSInteger32CallbackClass*> QADSInteger32::QADSInteger32CallbackClass::m_pointerVector;

// Callback-function
#ifdef USE_TWINCAT
void __stdcall QADSInteger32::QADSInteger32CallbackClass::Callback(AmsAddr* pAddr, AdsNotificationHeader* pNotification, unsigned long hUser)
{
    Q_UNUSED(pAddr);
    QMutexLocker locker(&m_pointerVectorMutex);
    Q_UNUSED(locker);
    qint32 value;
    QADSInteger32::QADSInteger32CallbackClass *sender = m_pointerVector[hUser];

    value = *(qint32 *)pNotification->data;
    if( sender != Q_NULLPTR )
    {
        sender->setValue(value);
    }
}
#else
void QADSInteger32::QADSInteger32CallbackClass::Callback(const AmsAddr* pAddr, const AdsNotificationHeader* pNotification, uint32_t hUser)
{
    Q_UNUSED(pAddr);
    QMutexLocker locker(&m_pointerVectorMutex);
    Q_UNUSED(locker);
    qint32 value;
    QADSInteger32::QADSInteger32CallbackClass *sender = m_pointerVector[hUser];

    const uint8_t* data = reinterpret_cast<const uint8_t*>(pNotification + 1);
    value = *(qint32 *)data;
    if( sender != Q_NULLPTR )
    {
        sender->setValue(value);
    }
}
#endif

QADSInteger32::QADSInteger32(QObject *parent, quint16 amsport, const QString &amsnetid, const QString &amshost,
                             const QString &var, PLCVariableReadOperation readop, unsigned long cycletime) :
    QADSPLCVariable(parent,amsport,amsnetid,amshost,var,readop,cycletime),
    m_value(0),
    m_adsInteger32Callback(Q_NULLPTR),
    m_variableCallBackNotificationHandle(0)
{
}

QADSInteger32::~QADSInteger32()
{
    resetVariableCallback();
}

void QADSInteger32::setupVariableCallback()
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
            m_adsErrorString = tr("QADSInteger32, Line ");
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
            if ( m_adsInteger32Callback == Q_NULLPTR )
            {
                m_adsInteger32Callback = new QADSInteger32::QADSInteger32CallbackClass();
                QADSInteger32::QADSInteger32CallbackClass::m_pointerVector.append(m_adsInteger32Callback);
            }

            // Get pointer index.
            hUser = QADSInteger32::QADSInteger32CallbackClass::m_pointerVector.size()-1;

            // set the attributes of the notification
            adsNotificationAttrib.cbLength = sizeof(qint32);
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
                m_adsErrorString = tr("QADSInteger32, Line ");
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
                        m_adsErrorString = tr("QADSInteger32, Line ");
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
                                                             QADSInteger32::QADSInteger32CallbackClass::Callback, hUser,
                                                             &m_variableCallBackNotificationHandle);
#else
                    uint32_t handle = 0;
                    nErr = AdsSyncAddDeviceNotificationReqEx(adsPort(), pAddr, ADSIGRP_SYM_VALBYHND,
                                                             adsSymbolHandle(), &adsNotificationAttrib,
                                                             QADSInteger32::QADSInteger32CallbackClass::Callback, hUser,
                                                             &handle);
                    m_variableCallBackNotificationHandle = handle;
#endif
                    if (nErr)
                    {
                        m_adsError = true;
                        m_adsErrorString = tr("QADSInteger32, Line ");
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
                        connect(m_adsInteger32Callback, SIGNAL(valueChanged(qint32)),
                                this, SLOT(privSetValue(qint32)), static_cast<Qt::ConnectionType>(Qt::QueuedConnection | Qt::UniqueConnection));
                    }
                }
            }
        }
    }
}

void QADSInteger32::resetVariableCallback()
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
    if( m_adsInteger32Callback )
    {
        QMutexLocker locker(&QADSInteger32::QADSInteger32CallbackClass::m_pointerVectorMutex);
        Q_UNUSED(locker);
        int index = QADSInteger32::QADSInteger32CallbackClass::m_pointerVector.indexOf(m_adsInteger32Callback);
        if (index >= 0)
        {
            QADSInteger32::QADSInteger32CallbackClass::m_pointerVector[index] = Q_NULLPTR;
        }
        delete m_adsInteger32Callback;
        m_adsInteger32Callback = Q_NULLPTR;
    }
}

void QADSInteger32::readValue()
{
    if( !m_adsError )
    {
        qint32 returnValue = m_value;
        syncReadRequest(amsPort(), ADSIGRP_SYM_VALBYHND, adsSymbolHandle(), sizeof(returnValue), &returnValue, Q_NULLPTR);
        if ( !m_adsError )
        {
            privSetValue(returnValue);
        }
    }
}

qint32 QADSInteger32::value()
{
    return m_value;
}

void QADSInteger32::initializeADSConnection()
{
    QADSPLCVariable::initializeADSConnection();
    parseVariableType();
    resetVariableCallback();
    setupVariableCallback();
}

void QADSInteger32::setValue(qint32 value)
{
    privSetValue(value);
}

void QADSInteger32::privSetValue(qint32 value)
{
    if( value != m_value )
    {
        m_value = value;
        Q_EMIT valueChanged();
    }
}

