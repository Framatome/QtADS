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
#include "qadsarrayfloatingpointbase.h"
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
/******************************** QADSFloatingPoint64Array **********************************/
/************************************************************************************************/
void QADSFloatingPoint64ArrayCallbackClassTemplate::setValue(const QADSDOUBLEARRAY &val)
{
    Q_EMIT valueChanged(val);
}

class QADSFloatingPoint64Array::QADSFloatingPoint64ArrayCallbackClass : public QADSFloatingPoint64ArrayCallbackClassTemplate
{
public:
    QADSFloatingPoint64ArrayCallbackClass(unsigned int z, unsigned int y, unsigned x)
        :QADSFloatingPoint64ArrayCallbackClassTemplate(z,y,x)
    {}
#ifdef USE_TWINCAT
    static void __stdcall Callback(AmsAddr*, AdsNotificationHeader*, unsigned long);
#else
    static void Callback(const AmsAddr*, const AdsNotificationHeader*, uint32_t);
#endif
    static QMutex m_pointerVectorMutex;
    static QVector<QADSFloatingPoint64Array::QADSFloatingPoint64ArrayCallbackClass*> m_pointerVector;
};

QMutex QADSFloatingPoint64Array::QADSFloatingPoint64ArrayCallbackClass::m_pointerVectorMutex;
QVector<QADSFloatingPoint64Array::QADSFloatingPoint64ArrayCallbackClass*> QADSFloatingPoint64Array::QADSFloatingPoint64ArrayCallbackClass::m_pointerVector;

// Callback-function
#ifdef USE_TWINCAT
void __stdcall QADSFloatingPoint64Array::QADSFloatingPoint64ArrayCallbackClass::Callback(AmsAddr* pAddr, AdsNotificationHeader* pNotification, unsigned long hUser)
{
    Q_UNUSED(pAddr);
    QMutexLocker locker(&m_pointerVectorMutex);
    Q_UNUSED(locker);
    QADSFloatingPoint64Array::QADSFloatingPoint64ArrayCallbackClass *sender = m_pointerVector[hUser];

    if( sender != Q_NULLPTR )
    {
        QADSDOUBLEARRAY tempValue(sender->z(), sender->y(), sender->x());

        Q_ASSERT((pNotification->cbSampleSize/sizeof(double)) == sender->count());
        for(unsigned int i=0; i<tempValue.count(); i++)
        {
            tempValue[i] = *(((double *)pNotification->data)+i);
        }
        sender->setValue(tempValue);
    }
}
#else
void QADSFloatingPoint64Array::QADSFloatingPoint64ArrayCallbackClass::Callback(const AmsAddr* pAddr, const AdsNotificationHeader* pNotification, uint32_t hUser)
{
    Q_UNUSED(pAddr);
    QMutexLocker locker(&m_pointerVectorMutex);
    Q_UNUSED(locker);
    QADSFloatingPoint64Array::QADSFloatingPoint64ArrayCallbackClass *sender = m_pointerVector[hUser];

    if( sender != Q_NULLPTR )
    {
        QADSDOUBLEARRAY tempValue(sender->z(), sender->y(), sender->x());

        Q_ASSERT((pNotification->cbSampleSize/sizeof(double)) == sender->count());
        for(unsigned int i=0; i<tempValue.count(); i++)
        {
            const uint8_t* data = reinterpret_cast<const uint8_t*>(pNotification + 1);
            tempValue[i] = *(((double *)data)+i);
        }
        sender->setValue(tempValue);
    }
}
#endif

int QADSFloatingPoint64Array::m_metaTypeId = -1;

QADSFloatingPoint64Array::QADSFloatingPoint64Array(QObject *parent, quint16 amsport, const QString &amsnetid, const QString &amshost,
                                                   const QString &var, PLCVariableReadOperation readop, unsigned long cycletime) :
    QADSPLCVariable(parent,amsport,amsnetid,amshost,var,readop,cycletime),
    m_adsFloatingPoint64ArrayCallback(Q_NULLPTR),
    m_variableCallBackNotificationHandle(0)
{
    // Register new meta type if not already registered.
    if( !QMetaType::isRegistered(m_metaTypeId) )
    {
        m_metaTypeId = qRegisterMetaType<QADSDOUBLEARRAY>("QADSDOUBLEARRAY");
    }

    baseParseArrayParameters();
}

QADSFloatingPoint64Array::~QADSFloatingPoint64Array()
{
    resetVariableCallback();
}

void QADSFloatingPoint64Array::parseArrayParameters()
{
    baseParseArrayParameters();
}

void QADSFloatingPoint64Array::baseParseArrayParameters()
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
        m_adsErrorString = tr("QADSFloatingPoint64Array, Line ");
        m_adsErrorString += QString("%1").arg(__LINE__);
        m_adsErrorString += ": ";
        m_adsErrorString += tr("Error: %1 is a %2 type instead of an ARRAY as required by this class. Please check the PLC declaration.").arg(adsSymbolName()).arg(adsSymbolType());
        Q_EMIT adsErrorChanged();
        Q_EMIT adsErrorStringChanged();
    }
}

void QADSFloatingPoint64Array::setupVariableCallback()
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
            m_adsErrorString = tr("QADSFloatingPoint64Array, Line ");
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
            if ( m_adsFloatingPoint64ArrayCallback == Q_NULLPTR )
            {
                m_adsFloatingPoint64ArrayCallback = new QADSFloatingPoint64Array::QADSFloatingPoint64ArrayCallbackClass(m_value.z(),m_value.y(),m_value.x());
                QADSFloatingPoint64Array::QADSFloatingPoint64ArrayCallbackClass::m_pointerVector.append(m_adsFloatingPoint64ArrayCallback);
            }

            // Get pointer index.
            hUser = QADSFloatingPoint64Array::QADSFloatingPoint64ArrayCallbackClass::m_pointerVector.size()-1;

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
                m_adsErrorString = tr("QADSFloatingPoint64Array, Line ");
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
                        m_adsErrorString = tr("QADSFloatingPoint64Array, Line ");
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
                                                             QADSFloatingPoint64Array::QADSFloatingPoint64ArrayCallbackClass::Callback, hUser,
                                                             &m_variableCallBackNotificationHandle);
#else
                    uint32_t handle = 0;
                    nErr = AdsSyncAddDeviceNotificationReqEx(adsPort(), pAddr, ADSIGRP_SYM_VALBYHND,
                                                             adsSymbolHandle(), &adsNotificationAttrib,
                                                             QADSFloatingPoint64Array::QADSFloatingPoint64ArrayCallbackClass::Callback, hUser,
                                                             &handle);
                    m_variableCallBackNotificationHandle = handle;
#endif
                    if (nErr)
                    {
                        m_adsError = true;
                        m_adsErrorString = tr("QADSFloatingPoint64Array, Line ");
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
                        connect(m_adsFloatingPoint64ArrayCallback, SIGNAL(valueChanged(QADSDOUBLEARRAY)),
                                this, SLOT(privSetValue(QADSDOUBLEARRAY)), static_cast<Qt::ConnectionType>(Qt::QueuedConnection | Qt::UniqueConnection));
                    }
                }
            }
        }
    }
}

void QADSFloatingPoint64Array::resetVariableCallback()
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
    if( m_adsFloatingPoint64ArrayCallback )
    {
        QMutexLocker locker(&QADSFloatingPoint64Array::QADSFloatingPoint64ArrayCallbackClass::m_pointerVectorMutex);
        Q_UNUSED(locker);
        int index = QADSFloatingPoint64Array::QADSFloatingPoint64ArrayCallbackClass::m_pointerVector.indexOf(m_adsFloatingPoint64ArrayCallback);
        if (index >= 0)
        {
            QADSFloatingPoint64Array::QADSFloatingPoint64ArrayCallbackClass::m_pointerVector[index] = Q_NULLPTR;
        }
        delete m_adsFloatingPoint64ArrayCallback;
        m_adsFloatingPoint64ArrayCallback = Q_NULLPTR;
    }
}

void QADSFloatingPoint64Array::readValue()
{
    if( !m_adsError )
    {
        QADSDOUBLEARRAY temp = m_value;
        syncReadRequest(amsPort(), ADSIGRP_SYM_VALBYHND, adsSymbolHandle(), temp.count()*sizeof(double), temp.array(), Q_NULLPTR);
        if ( !m_adsError )
        {
            privSetValue(temp);
        }
    }
}

const QADSDOUBLEARRAY &QADSFloatingPoint64Array::value()
{
    return m_value;
}

double QADSFloatingPoint64Array::value(int x)
{
    return m_value[x];
}

double QADSFloatingPoint64Array::value(int y, int x)
{
    return m_value(y,x);
}

double QADSFloatingPoint64Array::value(int z, int y, int x)
{
    return m_value(z,y,x);
}

void QADSFloatingPoint64Array::initializeADSConnection()
{
    QADSPLCVariable::initializeADSConnection();
    parseArrayParameters();
    resetVariableCallback();
    setupVariableCallback();
}

void QADSFloatingPoint64Array::setValue(const QADSDOUBLEARRAY &val)
{
    privSetValue(val);
}

void QADSFloatingPoint64Array::setValue(double val, int x)
{
    m_value[x] = val;
}

void QADSFloatingPoint64Array::setValue(double val, int y, int x)
{
    m_value(y,x) = val;
}

void QADSFloatingPoint64Array::setValue(double val, int z, int y, int x)
{
    m_value(z,y,x) = val;
}

void QADSFloatingPoint64Array::privSetValue(const QADSDOUBLEARRAY &val)
{
    if( val != m_value )
    {
        m_value = val;
        Q_EMIT valueChanged();
    }
}

/************************************************************************************************/
/******************************** QADSFloatingPoint32Array **********************************/
/************************************************************************************************/
void QADSFloatingPoint32ArrayCallbackClassTemplate::setValue(const QADSFLOATARRAY &val)
{
    Q_EMIT valueChanged(val);
}

class QADSFloatingPoint32Array::QADSFloatingPoint32ArrayCallbackClass : public QADSFloatingPoint32ArrayCallbackClassTemplate
{
public:
    QADSFloatingPoint32ArrayCallbackClass(unsigned int z, unsigned int y, unsigned x)
        :QADSFloatingPoint32ArrayCallbackClassTemplate(z,y,x)
    {}
#ifdef USE_TWINCAT
    static void __stdcall Callback(AmsAddr*, AdsNotificationHeader*, unsigned long);
#else
    static void Callback(const AmsAddr*, const AdsNotificationHeader*, uint32_t);
#endif
    static QMutex m_pointerVectorMutex;
    static QVector<QADSFloatingPoint32Array::QADSFloatingPoint32ArrayCallbackClass*> m_pointerVector;
};

QMutex QADSFloatingPoint32Array::QADSFloatingPoint32ArrayCallbackClass::m_pointerVectorMutex;
QVector<QADSFloatingPoint32Array::QADSFloatingPoint32ArrayCallbackClass*> QADSFloatingPoint32Array::QADSFloatingPoint32ArrayCallbackClass::m_pointerVector;

// Callback-function
#ifdef USE_TWINCAT
void __stdcall QADSFloatingPoint32Array::QADSFloatingPoint32ArrayCallbackClass::Callback(AmsAddr* pAddr, AdsNotificationHeader* pNotification, unsigned long hUser)
{
    Q_UNUSED(pAddr);
    QMutexLocker locker(&m_pointerVectorMutex);
    Q_UNUSED(locker);
    QADSFloatingPoint32Array::QADSFloatingPoint32ArrayCallbackClass *sender = m_pointerVector[hUser];

    if( sender != Q_NULLPTR )
    {
        QADSFLOATARRAY tempValue(sender->z(), sender->y(), sender->x());

        Q_ASSERT((pNotification->cbSampleSize/sizeof(float)) == sender->count());
        for(unsigned int i=0; i<tempValue.count(); i++)
        {
            tempValue[i] = *(((float *)pNotification->data)+i);
        }
        sender->setValue(tempValue);
    }
}
#else
void QADSFloatingPoint32Array::QADSFloatingPoint32ArrayCallbackClass::Callback(const AmsAddr *pAddr, const AdsNotificationHeader *pNotification, uint32_t hUser)
{
    Q_UNUSED(pAddr);
    QMutexLocker locker(&m_pointerVectorMutex);
    Q_UNUSED(locker);
    QADSFloatingPoint32Array::QADSFloatingPoint32ArrayCallbackClass *sender = m_pointerVector[hUser];

    if( sender != Q_NULLPTR )
    {
        QADSFLOATARRAY tempValue(sender->z(), sender->y(), sender->x());

        Q_ASSERT((pNotification->cbSampleSize/sizeof(float)) == sender->count());
        for(unsigned int i=0; i<tempValue.count(); i++)
        {
            const uint8_t* data = reinterpret_cast<const uint8_t*>(pNotification + 1);
            tempValue[i] = *(((float *)data)+i);
        }
        sender->setValue(tempValue);
    }
}
#endif

int QADSFloatingPoint32Array::m_metaTypeId = -1;

QADSFloatingPoint32Array::QADSFloatingPoint32Array(QObject *parent, quint16 amsport, const QString &amsnetid, const QString &amshost,
                                                   const QString &var, PLCVariableReadOperation readop, unsigned long cycletime) :
    QADSPLCVariable(parent,amsport,amsnetid,amshost,var,readop,cycletime),
    m_adsFloatingPoint32ArrayCallback(Q_NULLPTR),
    m_variableCallBackNotificationHandle(0)
{
    // Register new meta type if not already registered.
    if( !QMetaType::isRegistered(m_metaTypeId) )
    {
        m_metaTypeId = qRegisterMetaType<QADSFLOATARRAY>("QADSFLOATARRAY");
    }

    baseParseArrayParameters();
}

QADSFloatingPoint32Array::~QADSFloatingPoint32Array()
{
    resetVariableCallback();
}

void QADSFloatingPoint32Array::parseArrayParameters()
{
    baseParseArrayParameters();
}

void QADSFloatingPoint32Array::baseParseArrayParameters()
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
        m_adsErrorString = tr("QADSFloatingPoint32Array, Line ");
        m_adsErrorString += QString("%1").arg(__LINE__);
        m_adsErrorString += ": ";
        m_adsErrorString += tr("Error: %1 is a %2 type instead of an ARRAY as required by this class. Please check the PLC declaration.").arg(adsSymbolName()).arg(adsSymbolType());
        Q_EMIT adsErrorChanged();
        Q_EMIT adsErrorStringChanged();
    }
}

void QADSFloatingPoint32Array::setupVariableCallback()
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
            m_adsErrorString = tr("QADSFloatingPoint32Array, Line ");
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
            if ( m_adsFloatingPoint32ArrayCallback == Q_NULLPTR )
            {
                m_adsFloatingPoint32ArrayCallback = new QADSFloatingPoint32Array::QADSFloatingPoint32ArrayCallbackClass(m_value.z(),m_value.y(),m_value.x());
                QADSFloatingPoint32Array::QADSFloatingPoint32ArrayCallbackClass::m_pointerVector.append(m_adsFloatingPoint32ArrayCallback);
            }

            // Get pointer index.
            hUser = QADSFloatingPoint32Array::QADSFloatingPoint32ArrayCallbackClass::m_pointerVector.size()-1;

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
                m_adsErrorString = tr("QADSFloatingPoint32Array, Line ");
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
                        m_adsErrorString = tr("QADSFloatingPoint32Array, Line ");
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
                                                             QADSFloatingPoint32Array::QADSFloatingPoint32ArrayCallbackClass::Callback, hUser,
                                                             &m_variableCallBackNotificationHandle);
#else
                    uint32_t handle = 0;
                    nErr = AdsSyncAddDeviceNotificationReqEx(adsPort(), pAddr, ADSIGRP_SYM_VALBYHND,
                                                             adsSymbolHandle(), &adsNotificationAttrib,
                                                             QADSFloatingPoint32Array::QADSFloatingPoint32ArrayCallbackClass::Callback, hUser,
                                                             &handle);
                    m_variableCallBackNotificationHandle = handle;
#endif
                    if (nErr)
                    {
                        m_adsError = true;
                        m_adsErrorString = tr("QADSFloatingPoint32Array, Line ");
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
                        connect(m_adsFloatingPoint32ArrayCallback, SIGNAL(valueChanged(QADSFLOATARRAY)),
                                this, SLOT(privSetValue(QADSFLOATARRAY)), static_cast<Qt::ConnectionType>(Qt::QueuedConnection | Qt::UniqueConnection));
                    }
                }
            }
        }
    }
}

void QADSFloatingPoint32Array::resetVariableCallback()
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
    if( m_adsFloatingPoint32ArrayCallback )
    {
        QMutexLocker locker(&QADSFloatingPoint32Array::QADSFloatingPoint32ArrayCallbackClass::m_pointerVectorMutex);
        Q_UNUSED(locker);
        int index = QADSFloatingPoint32Array::QADSFloatingPoint32ArrayCallbackClass::m_pointerVector.indexOf(m_adsFloatingPoint32ArrayCallback);
        if (index >= 0)
        {
            QADSFloatingPoint32Array::QADSFloatingPoint32ArrayCallbackClass::m_pointerVector[index] = Q_NULLPTR;
        }
        delete m_adsFloatingPoint32ArrayCallback;
        m_adsFloatingPoint32ArrayCallback = Q_NULLPTR;
    }
}

void QADSFloatingPoint32Array::readValue()
{
    if( !m_adsError )
    {
        QADSFLOATARRAY temp = m_value;
        syncReadRequest(amsPort(), ADSIGRP_SYM_VALBYHND, adsSymbolHandle(), temp.count()*sizeof(float), temp.array(), Q_NULLPTR);
        if ( !m_adsError )
        {
            privSetValue(temp);
        }
    }
}

const QADSFLOATARRAY &QADSFloatingPoint32Array::value()
{
    return m_value;
}

float QADSFloatingPoint32Array::value(int x)
{
    return m_value[x];
}

float QADSFloatingPoint32Array::value(int y, int x)
{
    return m_value(y,x);
}

float QADSFloatingPoint32Array::value(int z, int y, int x)
{
    return m_value(z,y,x);
}

void QADSFloatingPoint32Array::initializeADSConnection()
{
    QADSPLCVariable::initializeADSConnection();
    parseArrayParameters();
    resetVariableCallback();
    setupVariableCallback();
}

void QADSFloatingPoint32Array::setValue(const QADSFLOATARRAY &val)
{
    privSetValue(val);
}

void QADSFloatingPoint32Array::setValue(float val, int x)
{
    m_value[x] = val;
}

void QADSFloatingPoint32Array::setValue(float val, int y, int x)
{
    m_value(y,x) = val;
}

void QADSFloatingPoint32Array::setValue(float val, int z, int y, int x)
{
    m_value(z,y,x) = val;
}

void QADSFloatingPoint32Array::privSetValue(const QADSFLOATARRAY &val)
{
    if( val != m_value )
    {
        m_value = val;
        Q_EMIT valueChanged();
    }
}
