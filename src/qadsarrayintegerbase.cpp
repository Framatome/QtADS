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
#include "qadsarrayintegerbase.h"
#include <QStringList>
#include <QVector>
#include <QMutexLocker>
#if defined(USE_TWINCAT)
#include <windows.h>
#include <tcadsdef.h>
#include <tcadsapi.h>
#else
#include <AdsLib.h>
#endif

/************************************************************************************************/
/******************************** QADSUnsignedInteger16Array ********************************/
/************************************************************************************************/
void QADSUnsignedInteger16ArrayCallbackClassTemplate::setValue(const QADSQUINT16ARRAY &val)
{
    Q_EMIT valueChanged(val);
}

class QADSUnsignedInteger16Array::QADSUnsignedInteger16ArrayCallbackClass : public QADSUnsignedInteger16ArrayCallbackClassTemplate
{
public:
    QADSUnsignedInteger16ArrayCallbackClass(unsigned int z, unsigned int y, unsigned x)
        :QADSUnsignedInteger16ArrayCallbackClassTemplate(z,y,x)
    {}
#ifdef USE_TWINCAT
    static void __stdcall Callback(AmsAddr*, AdsNotificationHeader*, unsigned long);
#else
    static void Callback(const AmsAddr*, const AdsNotificationHeader*, uint32_t);
#endif
    static QMutex m_pointerVectorMutex;
    static QVector<QADSUnsignedInteger16Array::QADSUnsignedInteger16ArrayCallbackClass*> m_pointerVector;
};

QMutex QADSUnsignedInteger16Array::QADSUnsignedInteger16ArrayCallbackClass::m_pointerVectorMutex;
QVector<QADSUnsignedInteger16Array::QADSUnsignedInteger16ArrayCallbackClass*> QADSUnsignedInteger16Array::QADSUnsignedInteger16ArrayCallbackClass::m_pointerVector;

// Callback-function
#ifdef USE_TWINCAT
void __stdcall QADSUnsignedInteger16Array::QADSUnsignedInteger16ArrayCallbackClass::Callback(AmsAddr* pAddr, AdsNotificationHeader* pNotification, unsigned long hUser)
{
    Q_UNUSED(pAddr);
    QMutexLocker locker(&m_pointerVectorMutex);
    Q_UNUSED(locker);
    QADSUnsignedInteger16Array::QADSUnsignedInteger16ArrayCallbackClass *sender = m_pointerVector[hUser];

    if( sender != Q_NULLPTR )
    {
        QADSQUINT16ARRAY tempValue(sender->z(), sender->y(), sender->x());

        Q_ASSERT((pNotification->cbSampleSize/sizeof(quint16)) == sender->count());
        for(unsigned int i=0; i<tempValue.count(); i++)
        {
            tempValue[i] = *(((quint16 *)pNotification->data)+i);
        }
        sender->setValue(tempValue);
    }
}
#else
void QADSUnsignedInteger16Array::QADSUnsignedInteger16ArrayCallbackClass::Callback(const AmsAddr* pAddr, const AdsNotificationHeader* pNotification, uint32_t hUser)
{
    Q_UNUSED(pAddr);
    QMutexLocker locker(&m_pointerVectorMutex);
    Q_UNUSED(locker);
    QADSUnsignedInteger16Array::QADSUnsignedInteger16ArrayCallbackClass *sender = m_pointerVector[hUser];

    if( sender != Q_NULLPTR )
    {
        QADSQUINT16ARRAY tempValue(sender->z(), sender->y(), sender->x());

        Q_ASSERT((pNotification->cbSampleSize/sizeof(quint16)) == sender->count());
        for(unsigned int i=0; i<tempValue.count(); i++)
        {
            const uint8_t* data = reinterpret_cast<const uint8_t*>(pNotification + 1);
            tempValue[i] = *(((quint16 *)data)+i);
        }
        sender->setValue(tempValue);
    }
}

#endif

int QADSUnsignedInteger16Array::m_metaTypeId = -1;

QADSUnsignedInteger16Array::QADSUnsignedInteger16Array(QObject *parent, quint16 amsport, const QString &amsnetid, const QString &amshost,
                                                       const QString &var, PLCVariableReadOperation readop, unsigned long cycletime) :
    QADSPLCVariable(parent,amsport,amsnetid,amshost,var,readop,cycletime),
    m_adsUnsignedInteger16ArrayCallback(Q_NULLPTR),
    m_variableCallBackNotificationHandle(0)
{
    if( !QMetaType::isRegistered(m_metaTypeId) )
    {
        m_metaTypeId = qRegisterMetaType<QADSQUINT16ARRAY>("QADSQUINT16ARRAY");
    }

    baseParseArrayParameters();
}

QADSUnsignedInteger16Array::~QADSUnsignedInteger16Array()
{
    resetVariableCallback();
}

void QADSUnsignedInteger16Array::parseArrayParameters()
{
    baseParseArrayParameters();
}

void QADSUnsignedInteger16Array::baseParseArrayParameters()
{
    // Only works for the ARRAY[] type!
    bool tempError = false;
    // Split string by OF should produce 2 strings ARRAY[] and type.
    QStringList splitSymbolName = adsSymbolType().split(QString("OF"),QString::SkipEmptyParts);
    if( splitSymbolName.count() != 2 )
    {
        tempError = true;
    }
    else
    {
        if( !splitSymbolName[0].trimmed().contains("ARRAY") )
        {
            tempError = true;
        }
    }
    if( !m_adsError && tempError )
    {
        m_adsError = true;
        m_adsErrorString = tr("QADSUnsignedInteger16Array, Line ");
        m_adsErrorString += QString("%1").arg(__LINE__);
        m_adsErrorString += ": ";
        m_adsErrorString += tr("Error: %1 is a %2 type instead of an ARRAY as required by this class. Please check the PLC declaration.").arg(adsSymbolName()).arg(adsSymbolType());
        Q_EMIT adsErrorChanged();
        Q_EMIT adsErrorStringChanged();
    }
}

void QADSUnsignedInteger16Array::setupVariableCallback()
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
            m_adsErrorString = tr("QADSUnsignedInteger16Array, Line ");
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
            if ( m_adsUnsignedInteger16ArrayCallback == Q_NULLPTR )
            {
                m_adsUnsignedInteger16ArrayCallback = new QADSUnsignedInteger16Array::QADSUnsignedInteger16ArrayCallbackClass(m_value.z(),m_value.y(),m_value.x());
                QADSUnsignedInteger16Array::QADSUnsignedInteger16ArrayCallbackClass::m_pointerVector.append(m_adsUnsignedInteger16ArrayCallback);
            }

            // Get the pointer index.
            hUser = QADSUnsignedInteger16Array::QADSUnsignedInteger16ArrayCallbackClass::m_pointerVector.size()-1;

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
                m_adsErrorString = tr("QADSUnsignedInteger16Array, Line ");
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
                        m_adsErrorString = tr("QADSUnsignedInteger16Array, Line ");
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
                                                             QADSUnsignedInteger16Array::QADSUnsignedInteger16ArrayCallbackClass::Callback, hUser,
                                                             &m_variableCallBackNotificationHandle);
#else
                    uint32_t handle = 0;
                    nErr = AdsSyncAddDeviceNotificationReqEx(adsPort(), pAddr, ADSIGRP_SYM_VALBYHND,
                                                             adsSymbolHandle(), &adsNotificationAttrib,
                                                             QADSUnsignedInteger16Array::QADSUnsignedInteger16ArrayCallbackClass::Callback, hUser,
                                                             &handle);
                    m_variableCallBackNotificationHandle = handle;
#endif
                    if (nErr)
                    {
                        m_adsError = true;
                        m_adsErrorString = tr("QADSUnsignedInteger16Array, Line ");
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
                        connect(m_adsUnsignedInteger16ArrayCallback, SIGNAL(valueChanged(QADSQUINT16ARRAY)),
                                this, SLOT(privSetValue(QADSQUINT16ARRAY)), static_cast<Qt::ConnectionType>(Qt::QueuedConnection | Qt::UniqueConnection));
                    }
                }
            }
        }
    }
}

void QADSUnsignedInteger16Array::resetVariableCallback()
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
    if( m_adsUnsignedInteger16ArrayCallback )
    {
        QMutexLocker locker(&QADSUnsignedInteger16Array::QADSUnsignedInteger16ArrayCallbackClass::m_pointerVectorMutex);
        Q_UNUSED(locker);
        int index = QADSUnsignedInteger16Array::QADSUnsignedInteger16ArrayCallbackClass::m_pointerVector.indexOf(m_adsUnsignedInteger16ArrayCallback);
        if (index >= 0)
        {
            QADSUnsignedInteger16Array::QADSUnsignedInteger16ArrayCallbackClass::m_pointerVector[index] = Q_NULLPTR;
        }
        delete m_adsUnsignedInteger16ArrayCallback;
        m_adsUnsignedInteger16ArrayCallback = Q_NULLPTR;
    }
}

void QADSUnsignedInteger16Array::readValue()
{
    if( !m_adsError )
    {
        QADSQUINT16ARRAY temp = m_value;
        syncReadRequest(amsPort(), ADSIGRP_SYM_VALBYHND, adsSymbolHandle(), temp.count()*sizeof(quint16), temp.array(), Q_NULLPTR);
        if ( !m_adsError )
        {
            privSetValue(temp);
        }
    }
}

const QADSQUINT16ARRAY &QADSUnsignedInteger16Array::value()
{
    return m_value;
}

quint16 QADSUnsignedInteger16Array::value(int x)
{
    return m_value[x];
}

quint16 QADSUnsignedInteger16Array::value(int y, int x)
{
    return m_value(y,x);
}

quint16 QADSUnsignedInteger16Array::value(int z, int y, int x)
{
    return m_value(z,y,x);
}

void QADSUnsignedInteger16Array::initializeADSConnection()
{
    QADSPLCVariable::initializeADSConnection();
    parseArrayParameters();
    resetVariableCallback();
    setupVariableCallback();
}

void QADSUnsignedInteger16Array::setValue(const QADSQUINT16ARRAY &val)
{
    privSetValue(val);
}

void QADSUnsignedInteger16Array::setValue(quint16 val, int x)
{
    m_value[x] = val;
}

void QADSUnsignedInteger16Array::setValue(quint16 val, int y, int x)
{
    m_value(y,x) = val;
}

void QADSUnsignedInteger16Array::setValue(quint16 val, int z, int y, int x)
{
    m_value(z,y,x) = val;
}

void QADSUnsignedInteger16Array::privSetValue(const QADSQUINT16ARRAY &val)
{
    if( val != m_value )
    {
        m_value = val;
        Q_EMIT valueChanged();
    }
}

/************************************************************************************************/
/******************************** QADSInteger16Array ****************************************/
/************************************************************************************************/
void QADSInteger16ArrayCallbackClassTemplate::setValue(const QADSQINT16ARRAY &val)
{
    Q_EMIT valueChanged(val);
}

class QADSInteger16Array::QADSInteger16ArrayCallbackClass : public QADSInteger16ArrayCallbackClassTemplate
{
public:
    QADSInteger16ArrayCallbackClass(unsigned int z, unsigned int y, unsigned x)
        :QADSInteger16ArrayCallbackClassTemplate(z,y,x)
    {}
#ifdef USE_TWINCAT
    static void __stdcall Callback(AmsAddr*, AdsNotificationHeader*, unsigned long);
#else
    static void Callback(const AmsAddr*, const AdsNotificationHeader*, uint32_t);
#endif
    static QMutex m_pointerVectorMutex;
    static QVector<QADSInteger16Array::QADSInteger16ArrayCallbackClass*> m_pointerVector;
};

QMutex QADSInteger16Array::QADSInteger16ArrayCallbackClass::m_pointerVectorMutex;
QVector<QADSInteger16Array::QADSInteger16ArrayCallbackClass*> QADSInteger16Array::QADSInteger16ArrayCallbackClass::m_pointerVector;

// Callback-function
#ifdef USE_TWINCAT
void __stdcall QADSInteger16Array::QADSInteger16ArrayCallbackClass::Callback(AmsAddr* pAddr, AdsNotificationHeader* pNotification, unsigned long hUser)
{
    Q_UNUSED(pAddr);
    QMutexLocker locker(&m_pointerVectorMutex);
    Q_UNUSED(locker);
    QADSInteger16Array::QADSInteger16ArrayCallbackClass *sender = m_pointerVector[hUser];

    if( sender != Q_NULLPTR )
    {
        QADSQINT16ARRAY tempValue(sender->z(), sender->y(), sender->x());

        Q_ASSERT((pNotification->cbSampleSize/sizeof(qint16)) == sender->count());
        for(unsigned int i=0; i<tempValue.count(); i++)
        {
            tempValue[i] = *(((qint16 *)pNotification->data)+i);
        }
        sender->setValue(tempValue);
    }
}
#else
void QADSInteger16Array::QADSInteger16ArrayCallbackClass::Callback(const AmsAddr* pAddr, const AdsNotificationHeader* pNotification, uint32_t hUser)
{
    Q_UNUSED(pAddr);
    QMutexLocker locker(&m_pointerVectorMutex);
    Q_UNUSED(locker);
    QADSInteger16Array::QADSInteger16ArrayCallbackClass *sender = m_pointerVector[hUser];

    if( sender != Q_NULLPTR )
    {
        QADSQINT16ARRAY tempValue(sender->z(), sender->y(), sender->x());

        Q_ASSERT((pNotification->cbSampleSize/sizeof(qint16)) == sender->count());
        for(unsigned int i=0; i<tempValue.count(); i++)
        {
            const uint8_t* data = reinterpret_cast<const uint8_t*>(pNotification + 1);
            tempValue[i] = *(((qint16 *)data)+i);
        }
        sender->setValue(tempValue);
    }
}
#endif

int QADSInteger16Array::m_metaTypeId = -1;

QADSInteger16Array::QADSInteger16Array(QObject *parent, quint16 amsport, const QString &amsnetid, const QString &amshost,
                                       const QString &var, PLCVariableReadOperation readop, unsigned long cycletime) :
    QADSPLCVariable(parent,amsport,amsnetid,amshost,var,readop,cycletime),
    m_adsInteger16ArrayCallback(Q_NULLPTR),
    m_variableCallBackNotificationHandle(0)
{
    // Register new meta type if not already registered.
    if( !QMetaType::isRegistered(m_metaTypeId) )
    {
        m_metaTypeId = qRegisterMetaType<QADSQINT16ARRAY>("QADSQINT16ARRAY");
    }

    baseParseArrayParameters();
}

QADSInteger16Array::~QADSInteger16Array()
{
    resetVariableCallback();
}

void QADSInteger16Array::parseArrayParameters()
{
    baseParseArrayParameters();
}

void QADSInteger16Array::baseParseArrayParameters()
{
    // Only works for the ARRAY[] type!
    bool tempError = false;
    // Split string by OF should produce 2 strings ARRAY[] and type.
    QStringList splitSymbolName = adsSymbolType().split(QString("OF"),QString::SkipEmptyParts);
    if( splitSymbolName.count() != 2 )
    {
        tempError = true;
    }
    else
    {
        if( !splitSymbolName[0].trimmed().contains("ARRAY") )
        {
            tempError = true;
        }
    }
    if( !m_adsError && tempError )
    {
        m_adsError = true;
        m_adsErrorString = tr("QADSInteger16Array, Line ");
        m_adsErrorString += QString("%1").arg(__LINE__);
        m_adsErrorString += ": ";
        m_adsErrorString += tr("Error: %1 is a %2 type instead of an ARRAY as required by this class. Please check the PLC declaration.").arg(adsSymbolName()).arg(adsSymbolType());
        Q_EMIT adsErrorChanged();
        Q_EMIT adsErrorStringChanged();
    }
}

void QADSInteger16Array::setupVariableCallback()
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
            m_adsErrorString = tr("QADSInteger16Array, Line ");
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
            if ( m_adsInteger16ArrayCallback == Q_NULLPTR )
            {
                m_adsInteger16ArrayCallback = new QADSInteger16Array::QADSInteger16ArrayCallbackClass(m_value.z(),m_value.y(),m_value.x());
                QADSInteger16Array::QADSInteger16ArrayCallbackClass::m_pointerVector.append(m_adsInteger16ArrayCallback);
            }

            // Get pointer index.
            hUser = QADSInteger16Array::QADSInteger16ArrayCallbackClass::m_pointerVector.size()-1;

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
                m_adsErrorString = tr("QADSInteger16Array, Line ");
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
                        m_adsErrorString = tr("QADSInteger16Array, Line ");
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
                                                             QADSInteger16Array::QADSInteger16ArrayCallbackClass::Callback, hUser,
                                                             &m_variableCallBackNotificationHandle);
#else
                    uint32_t handle = 0;
                    nErr = AdsSyncAddDeviceNotificationReqEx(adsPort(), pAddr, ADSIGRP_SYM_VALBYHND,
                                                             adsSymbolHandle(), &adsNotificationAttrib,
                                                             QADSInteger16Array::QADSInteger16ArrayCallbackClass::Callback, hUser,
                                                             &handle);
                    m_variableCallBackNotificationHandle = handle;
#endif
                    if (nErr)
                    {
                        m_adsError = true;
                        m_adsErrorString = tr("QADSInteger16Array, Line ");
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
                        connect(m_adsInteger16ArrayCallback, SIGNAL(valueChanged(QADSQINT16ARRAY)),
                                this, SLOT(privSetValue(QADSQINT16ARRAY)), static_cast<Qt::ConnectionType>(Qt::QueuedConnection | Qt::UniqueConnection));
                    }
                }
            }
        }
    }
}

void QADSInteger16Array::resetVariableCallback()
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
    if( m_adsInteger16ArrayCallback )
    {
        QMutexLocker locker(&QADSInteger16Array::QADSInteger16ArrayCallbackClass::m_pointerVectorMutex);
        Q_UNUSED(locker);
        int index = QADSInteger16Array::QADSInteger16ArrayCallbackClass::m_pointerVector.indexOf(m_adsInteger16ArrayCallback);
        if (index >= 0)
        {
            QADSInteger16Array::QADSInteger16ArrayCallbackClass::m_pointerVector[index] = Q_NULLPTR;
        }
        delete m_adsInteger16ArrayCallback;
        m_adsInteger16ArrayCallback = Q_NULLPTR;
    }
}

void QADSInteger16Array::readValue()
{
    if( !m_adsError )
    {
        QADSQINT16ARRAY temp = m_value;
        syncReadRequest(amsPort(), ADSIGRP_SYM_VALBYHND, adsSymbolHandle(), temp.count()*sizeof(qint16), temp.array(), Q_NULLPTR);
        if ( !m_adsError )
        {
            privSetValue(temp);
        }
    }
}

const QADSQINT16ARRAY &QADSInteger16Array::value()
{
    return m_value;
}

qint16 QADSInteger16Array::value(int x)
{
    return m_value[x];
}

qint16 QADSInteger16Array::value(int y, int x)
{
    return m_value(y,x);
}

qint16 QADSInteger16Array::value(int z, int y, int x)
{
    return m_value(z,y,x);
}

void QADSInteger16Array::initializeADSConnection()
{
    QADSPLCVariable::initializeADSConnection();
    parseArrayParameters();
    resetVariableCallback();
    setupVariableCallback();
}

void QADSInteger16Array::setValue(const QADSQINT16ARRAY &val)
{
    privSetValue(val);
}

void QADSInteger16Array::setValue(qint16 val, int x)
{
    m_value[x] = val;
}

void QADSInteger16Array::setValue(qint16 val, int y, int x)
{
    m_value(y,x) = val;
}

void QADSInteger16Array::setValue(qint16 val, int z, int y, int x)
{
    m_value(z,y,x) = val;
}

void QADSInteger16Array::privSetValue(const QADSQINT16ARRAY &val)
{
    if( val != m_value )
    {
        m_value = val;
        Q_EMIT valueChanged();
    }
}

/************************************************************************************************/
/******************************** QADSUnsignedInteger32Array ********************************/
/************************************************************************************************/
void QADSUnsignedInteger32ArrayCallbackClassTemplate::setValue(const QADSQUINT32ARRAY &val)
{
    Q_EMIT valueChanged(val);
}

class QADSUnsignedInteger32Array::QADSUnsignedInteger32ArrayCallbackClass : public QADSUnsignedInteger32ArrayCallbackClassTemplate
{
public:
    QADSUnsignedInteger32ArrayCallbackClass(unsigned int z, unsigned int y, unsigned x)
        :QADSUnsignedInteger32ArrayCallbackClassTemplate(z,y,x)
    {}
#ifdef USE_TWINCAT
    static void __stdcall Callback(AmsAddr*, AdsNotificationHeader*, unsigned long);
#else
    static void Callback(const AmsAddr*, const AdsNotificationHeader*, uint32_t);
#endif
    static QMutex m_pointerVectorMutex;
    static QVector<QADSUnsignedInteger32Array::QADSUnsignedInteger32ArrayCallbackClass*> m_pointerVector;
};

QMutex QADSUnsignedInteger32Array::QADSUnsignedInteger32ArrayCallbackClass::m_pointerVectorMutex;
QVector<QADSUnsignedInteger32Array::QADSUnsignedInteger32ArrayCallbackClass*> QADSUnsignedInteger32Array::QADSUnsignedInteger32ArrayCallbackClass::m_pointerVector;

// Callback-function
#ifdef USE_TWINCAT
void __stdcall QADSUnsignedInteger32Array::QADSUnsignedInteger32ArrayCallbackClass::Callback(AmsAddr* pAddr, AdsNotificationHeader* pNotification, unsigned long hUser)
{
    Q_UNUSED(pAddr);
    QMutexLocker locker(&m_pointerVectorMutex);
    Q_UNUSED(locker);
    QADSUnsignedInteger32Array::QADSUnsignedInteger32ArrayCallbackClass *sender = m_pointerVector[hUser];

    if( sender != Q_NULLPTR )
    {
        QADSQUINT32ARRAY tempValue(sender->z(), sender->y(), sender->x());

        Q_ASSERT((pNotification->cbSampleSize/sizeof(quint32)) == sender->count());
        for(unsigned int i=0; i<tempValue.count(); i++)
        {
            tempValue[i] = *(((quint32 *)pNotification->data)+i);
        }
        sender->setValue(tempValue);
    }
}
#else
void QADSUnsignedInteger32Array::QADSUnsignedInteger32ArrayCallbackClass::Callback(const AmsAddr* pAddr, const AdsNotificationHeader* pNotification, uint32_t hUser)
{
    Q_UNUSED(pAddr);
    QMutexLocker locker(&m_pointerVectorMutex);
    Q_UNUSED(locker);
    QADSUnsignedInteger32Array::QADSUnsignedInteger32ArrayCallbackClass *sender = m_pointerVector[hUser];

    if( sender != Q_NULLPTR )
    {
        QADSQUINT32ARRAY tempValue(sender->z(), sender->y(), sender->x());

        Q_ASSERT((pNotification->cbSampleSize/sizeof(quint32)) == sender->count());
        for(unsigned int i=0; i<tempValue.count(); i++)
        {
            const uint8_t* data = reinterpret_cast<const uint8_t*>(pNotification + 1);
            tempValue[i] = *(((quint32 *)data)+i);
        }
        sender->setValue(tempValue);
    }
}

#endif

int QADSUnsignedInteger32Array::m_metaTypeId = -1;

QADSUnsignedInteger32Array::QADSUnsignedInteger32Array(QObject *parent, quint16 amsport, const QString &amsnetid, const QString &amshost,
                                                       const QString &var, PLCVariableReadOperation readop, unsigned long cycletime) :
    QADSPLCVariable(parent,amsport,amsnetid,amshost,var,readop,cycletime),
    m_adsUnsignedInteger32ArrayCallback(Q_NULLPTR),
    m_variableCallBackNotificationHandle(0)
{
    if( !QMetaType::isRegistered(m_metaTypeId) )
    {
        m_metaTypeId = qRegisterMetaType<QADSQUINT32ARRAY>("QADSQUINT32ARRAY");
    }

    baseParseArrayParameters();
}

QADSUnsignedInteger32Array::~QADSUnsignedInteger32Array()
{
    resetVariableCallback();
}

void QADSUnsignedInteger32Array::parseArrayParameters()
{
    baseParseArrayParameters();
}

void QADSUnsignedInteger32Array::baseParseArrayParameters()
{
    // Only works for the ARRAY[] type!
    bool tempError = false;
    // Split string by OF should produce 2 strings ARRAY[] and type.
    QStringList splitSymbolName = adsSymbolType().split(QString("OF"),QString::SkipEmptyParts);
    if( splitSymbolName.count() != 2 )
    {
        tempError = true;
    }
    else
    {
        if( !splitSymbolName[0].trimmed().contains("ARRAY") )
        {
            tempError = true;
        }
    }
    if( !m_adsError && tempError )
    {
        m_adsError = true;
        m_adsErrorString = tr("QADSUnsignedInteger32Array, Line ");
        m_adsErrorString += QString("%1").arg(__LINE__);
        m_adsErrorString += ": ";
        m_adsErrorString += tr("Error: %1 is a %2 type instead of an ARRAY as required by this class. Please check the PLC declaration.").arg(adsSymbolName()).arg(adsSymbolType());
        Q_EMIT adsErrorChanged();
        Q_EMIT adsErrorStringChanged();
    }
}

void QADSUnsignedInteger32Array::setupVariableCallback()
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
            m_adsErrorString = tr("QADSUnsignedInteger32Array, Line ");
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
            if ( m_adsUnsignedInteger32ArrayCallback == Q_NULLPTR )
            {
                m_adsUnsignedInteger32ArrayCallback = new QADSUnsignedInteger32Array::QADSUnsignedInteger32ArrayCallbackClass(m_value.z(),m_value.y(),m_value.x());
                QADSUnsignedInteger32Array::QADSUnsignedInteger32ArrayCallbackClass::m_pointerVector.append(m_adsUnsignedInteger32ArrayCallback);
            }

            // Get pointer index.
            hUser = QADSUnsignedInteger32Array::QADSUnsignedInteger32ArrayCallbackClass::m_pointerVector.size()-1;

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
                m_adsErrorString = tr("QADSUnsignedInteger32Array, Line ");
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
                        m_adsErrorString = tr("QADSUnsignedInteger32Array, Line ");
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
                                                             QADSUnsignedInteger32Array::QADSUnsignedInteger32ArrayCallbackClass::Callback, hUser,
                                                             &m_variableCallBackNotificationHandle);
#else
                    uint32_t handle = 0;
                    nErr = AdsSyncAddDeviceNotificationReqEx(adsPort(), pAddr, ADSIGRP_SYM_VALBYHND,
                                                             adsSymbolHandle(), &adsNotificationAttrib,
                                                             QADSUnsignedInteger32Array::QADSUnsignedInteger32ArrayCallbackClass::Callback, hUser,
                                                             &handle);
                    m_variableCallBackNotificationHandle = handle;
#endif
                    if (nErr)
                    {
                        m_adsError = true;
                        m_adsErrorString = tr("QADSUnsignedInteger32Array, Line ");
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
                        connect(m_adsUnsignedInteger32ArrayCallback, SIGNAL(valueChanged(QADSQUINT32ARRAY)),
                                this, SLOT(privSetValue(QADSQUINT32ARRAY)), static_cast<Qt::ConnectionType>(Qt::QueuedConnection | Qt::UniqueConnection));
                    }
                }
            }
        }
    }
}

void QADSUnsignedInteger32Array::resetVariableCallback()
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
    if( m_adsUnsignedInteger32ArrayCallback )
    {
        QMutexLocker locker(&QADSUnsignedInteger32Array::QADSUnsignedInteger32ArrayCallbackClass::m_pointerVectorMutex);
        Q_UNUSED(locker);
        int index = QADSUnsignedInteger32Array::QADSUnsignedInteger32ArrayCallbackClass::m_pointerVector.indexOf(m_adsUnsignedInteger32ArrayCallback);
        if (index >= 0)
        {
            QADSUnsignedInteger32Array::QADSUnsignedInteger32ArrayCallbackClass::m_pointerVector[index] = Q_NULLPTR;
        }
        delete m_adsUnsignedInteger32ArrayCallback;
        m_adsUnsignedInteger32ArrayCallback = Q_NULLPTR;
    }
}

void QADSUnsignedInteger32Array::readValue()
{
    if( !m_adsError )
    {
        QADSQUINT32ARRAY temp = m_value;
        syncReadRequest(amsPort(), ADSIGRP_SYM_VALBYHND, adsSymbolHandle(), temp.count()*sizeof(quint32), temp.array(), Q_NULLPTR);
        if ( !m_adsError )
        {
            privSetValue(temp);
        }
    }
}

const QADSQUINT32ARRAY &QADSUnsignedInteger32Array::value()
{
    return m_value;
}

quint32 QADSUnsignedInteger32Array::value(int x)
{
    return m_value[x];
}

quint32 QADSUnsignedInteger32Array::value(int y, int x)
{
    return m_value(y,x);
}

quint32 QADSUnsignedInteger32Array::value(int z, int y, int x)
{
    return m_value(z,y,x);
}

void QADSUnsignedInteger32Array::initializeADSConnection()
{
    QADSPLCVariable::initializeADSConnection();
    parseArrayParameters();
    resetVariableCallback();
    setupVariableCallback();
}

void QADSUnsignedInteger32Array::setValue(const QADSQUINT32ARRAY &val)
{
    privSetValue(val);
}

void QADSUnsignedInteger32Array::setValue(quint32 val, int x)
{
    m_value[x] = val;
}

void QADSUnsignedInteger32Array::setValue(quint32 val, int y, int x)
{
    m_value(y,x) = val;
}

void QADSUnsignedInteger32Array::setValue(quint32 val, int z, int y, int x)
{
    m_value(z,y,x) = val;
}

void QADSUnsignedInteger32Array::privSetValue(const QADSQUINT32ARRAY &val)
{
    if( val != m_value )
    {
        m_value = val;
        Q_EMIT valueChanged();
    }
}


/************************************************************************************************/
/******************************** QADSInteger32Array ****************************************/
/************************************************************************************************/
void QADSInteger32ArrayCallbackClassTemplate::setValue(const QADSQINT32ARRAY &val)
{
    Q_EMIT valueChanged(val);
}

class QADSInteger32Array::QADSInteger32ArrayCallbackClass : public QADSInteger32ArrayCallbackClassTemplate
{
public:
    QADSInteger32ArrayCallbackClass(unsigned int z, unsigned int y, unsigned x)
        :QADSInteger32ArrayCallbackClassTemplate(z,y,x)
    {}
#ifdef USE_TWINCAT
    static void __stdcall Callback(AmsAddr*, AdsNotificationHeader*, unsigned long);
#else
    static void Callback(const AmsAddr*, const AdsNotificationHeader*, uint32_t);
#endif
    static QMutex m_pointerVectorMutex;
    static QVector<QADSInteger32Array::QADSInteger32ArrayCallbackClass*> m_pointerVector;
};

QMutex QADSInteger32Array::QADSInteger32ArrayCallbackClass::m_pointerVectorMutex;
QVector<QADSInteger32Array::QADSInteger32ArrayCallbackClass*> QADSInteger32Array::QADSInteger32ArrayCallbackClass::m_pointerVector;

// Callback-function
#ifdef USE_TWINCAT
void __stdcall QADSInteger32Array::QADSInteger32ArrayCallbackClass::Callback(AmsAddr* pAddr, AdsNotificationHeader* pNotification, unsigned long hUser)
{
    Q_UNUSED(pAddr);
    QMutexLocker locker(&m_pointerVectorMutex);
    Q_UNUSED(locker);
    QADSInteger32Array::QADSInteger32ArrayCallbackClass *sender = m_pointerVector[hUser];

    if( sender != Q_NULLPTR )
    {
        QADSQINT32ARRAY tempValue(sender->z(), sender->y(), sender->x());

        Q_ASSERT((pNotification->cbSampleSize/sizeof(qint32)) == sender->count());
        for(unsigned int i=0; i<tempValue.count(); i++)
        {
            tempValue[i] = *(((qint32 *)pNotification->data)+i);
        }
        sender->setValue(tempValue);
    }
}
#else
void QADSInteger32Array::QADSInteger32ArrayCallbackClass::Callback(const AmsAddr* pAddr, const AdsNotificationHeader* pNotification, uint32_t hUser)
{
    Q_UNUSED(pAddr);
    QMutexLocker locker(&m_pointerVectorMutex);
    Q_UNUSED(locker);
    QADSInteger32Array::QADSInteger32ArrayCallbackClass *sender = m_pointerVector[hUser];

    if( sender != Q_NULLPTR )
    {
        QADSQINT32ARRAY tempValue(sender->z(), sender->y(), sender->x());

        Q_ASSERT((pNotification->cbSampleSize/sizeof(qint32)) == sender->count());
        for(unsigned int i=0; i<tempValue.count(); i++)
        {
            const uint8_t* data = reinterpret_cast<const uint8_t*>(pNotification + 1);
            tempValue[i] = *(((qint32 *)data)+i);
        }
        sender->setValue(tempValue);
    }
}
#endif

int QADSInteger32Array::m_metaTypeId = -1;

QADSInteger32Array::QADSInteger32Array(QObject *parent, quint16 amsport, const QString &amsnetid, const QString &amshost,
                                       const QString &var, PLCVariableReadOperation readop, unsigned long cycletime) :
    QADSPLCVariable(parent,amsport,amsnetid,amshost,var,readop,cycletime),
    m_adsInteger32ArrayCallback(Q_NULLPTR),
    m_variableCallBackNotificationHandle(0)
{
    // Register new meta type if not already registered.
    if( !QMetaType::isRegistered(m_metaTypeId) )
    {
        m_metaTypeId = qRegisterMetaType<QADSQINT32ARRAY>("QADSQINT32ARRAY");
    }

    baseParseArrayParameters();
}

QADSInteger32Array::~QADSInteger32Array()
{
    resetVariableCallback();
}

void QADSInteger32Array::parseArrayParameters()
{
    baseParseArrayParameters();
}

void QADSInteger32Array::baseParseArrayParameters()
{
    // Only works for the ARRAY[] type!
    bool tempError = false;
    // Split string by OF should produce 2 strings ARRAY[] and type.
    QStringList splitSymbolName = adsSymbolType().split(QString("OF"),QString::SkipEmptyParts);
    if( splitSymbolName.count() != 2 )
    {
        tempError = true;
    }
    else
    {
        if( !splitSymbolName[0].trimmed().contains("ARRAY") )
        {
            tempError = true;
        }
    }
    if( !m_adsError && tempError )
    {
        m_adsError = true;
        m_adsErrorString = tr("QADSInteger32Array, Line ");
        m_adsErrorString += QString("%1").arg(__LINE__);
        m_adsErrorString += ": ";
        m_adsErrorString += tr("Error: %1 is a %2 type instead of an ARRAY as required by this class. Please check the PLC declaration.").arg(adsSymbolName()).arg(adsSymbolType());
        Q_EMIT adsErrorChanged();
        Q_EMIT adsErrorStringChanged();
    }
}

void QADSInteger32Array::setupVariableCallback()
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
            m_adsErrorString = tr("QADSInteger32Array, Line ");
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
            if ( m_adsInteger32ArrayCallback == Q_NULLPTR )
            {
                m_adsInteger32ArrayCallback = new QADSInteger32Array::QADSInteger32ArrayCallbackClass(m_value.z(),m_value.y(),m_value.x());
                QADSInteger32Array::QADSInteger32ArrayCallbackClass::m_pointerVector.append(m_adsInteger32ArrayCallback);
            }

            // Get pointer index.
            hUser = QADSInteger32Array::QADSInteger32ArrayCallbackClass::m_pointerVector.size()-1;

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
                m_adsErrorString = tr("QADSInteger32Array, Line ");
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
                        m_adsErrorString = tr("QADSInteger32Array, Line ");
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
                                                             QADSInteger32Array::QADSInteger32ArrayCallbackClass::Callback, hUser,
                                                             &m_variableCallBackNotificationHandle);
#else
                    uint32_t handle = 0;
                    nErr = AdsSyncAddDeviceNotificationReqEx(adsPort(), pAddr, ADSIGRP_SYM_VALBYHND,
                                                             adsSymbolHandle(), &adsNotificationAttrib,
                                                             QADSInteger32Array::QADSInteger32ArrayCallbackClass::Callback, hUser,
                                                             &handle);
                    m_variableCallBackNotificationHandle = handle;
#endif
                    if (nErr)
                    {
                        m_adsError = true;
                        m_adsErrorString = tr("QADSInteger32Array, Line ");
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
                        connect(m_adsInteger32ArrayCallback, SIGNAL(valueChanged(QADSQINT32ARRAY)),
                                this, SLOT(privSetValue(QADSQINT32ARRAY)), static_cast<Qt::ConnectionType>(Qt::QueuedConnection | Qt::UniqueConnection));
                    }
                }
            }
        }
    }
}

void QADSInteger32Array::resetVariableCallback()
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
    if( m_adsInteger32ArrayCallback )
    {
        QMutexLocker locker(&QADSInteger32Array::QADSInteger32ArrayCallbackClass::m_pointerVectorMutex);
        Q_UNUSED(locker);
        int index = QADSInteger32Array::QADSInteger32ArrayCallbackClass::m_pointerVector.indexOf(m_adsInteger32ArrayCallback);
        if (index >= 0)
        {
            QADSInteger32Array::QADSInteger32ArrayCallbackClass::m_pointerVector[index] = Q_NULLPTR;
        }
        delete m_adsInteger32ArrayCallback;
        m_adsInteger32ArrayCallback = Q_NULLPTR;
    }
}

void QADSInteger32Array::readValue()
{
    if( !m_adsError )
    {
        QADSQINT32ARRAY temp = m_value;
        syncReadRequest(amsPort(), ADSIGRP_SYM_VALBYHND, adsSymbolHandle(), temp.count()*sizeof(qint32), temp.array(), Q_NULLPTR);
        if ( !m_adsError )
        {
            privSetValue(temp);
        }
    }
}

const QADSQINT32ARRAY &QADSInteger32Array::value()
{
    return m_value;
}

qint32 QADSInteger32Array::value(int x)
{
    return m_value[x];
}

qint32 QADSInteger32Array::value(int y, int x)
{
    return m_value(y,x);
}

qint32 QADSInteger32Array::value(int z, int y, int x)
{
    return m_value(z,y,x);
}

void QADSInteger32Array::initializeADSConnection()
{
    QADSPLCVariable::initializeADSConnection();
    parseArrayParameters();
    resetVariableCallback();
    setupVariableCallback();
}

void QADSInteger32Array::setValue(const QADSQINT32ARRAY &val)
{
    privSetValue(val);
}

void QADSInteger32Array::setValue(qint32 val, int x)
{
    m_value[x] = val;
}

void QADSInteger32Array::setValue(qint32 val, int y, int x)
{
    m_value(y,x) = val;
}

void QADSInteger32Array::setValue(qint32 val, int z, int y, int x)
{
    m_value(z,y,x) = val;
}

void QADSInteger32Array::privSetValue(const QADSQINT32ARRAY &val)
{
    if( val != m_value )
    {
        m_value = val;
        Q_EMIT valueChanged();
    }
}

