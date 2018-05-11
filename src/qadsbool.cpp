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
#include "qadsbool.h"
#include <QMutexLocker>
#include <QVector>
#if defined(USE_TWINCAT)
#include <windows.h>
#include "tcadsdef.h"
#include "tcadsapi.h"
#else
#include <AdsLib.h>
#endif

void QADSBOOLCallbackClassTemplate::setValue(bool val)
{
    Q_EMIT valueChanged(val);
}

class QADSBOOL::QADSBOOLCallbackClass : public QADSBOOLCallbackClassTemplate
{
public:
#ifdef USE_TWINCAT
    static void __stdcall Callback(AmsAddr*, AdsNotificationHeader*, unsigned long);
#else
    static void Callback(const AmsAddr*, const AdsNotificationHeader*, uint32_t);
#endif
    static QMutex m_pointerVectorMutex;
    static QVector<QADSBOOL::QADSBOOLCallbackClass*> m_pointerVector;
};

QMutex QADSBOOL::QADSBOOLCallbackClass::m_pointerVectorMutex;
QVector<QADSBOOL::QADSBOOLCallbackClass*> QADSBOOL::QADSBOOLCallbackClass::m_pointerVector;

// Callback-function
#ifdef USE_TWINCAT
void __stdcall QADSBOOL::QADSBOOLCallbackClass::Callback(AmsAddr* pAddr, AdsNotificationHeader* pNotification, unsigned long hUser)
{
    Q_UNUSED(pAddr);
    QMutexLocker locker(&m_pointerVectorMutex);
    Q_UNUSED(locker);
    quint8 returnValue = 0;
    bool value;
    QADSBOOL::QADSBOOLCallbackClass *sender = m_pointerVector[hUser];

    returnValue = *(quint8 *)pNotification->data;
    value = returnValue;

    if( sender != Q_NULLPTR )
    {
        sender->setValue(value);
    }
}
#else
void QADSBOOL::QADSBOOLCallbackClass::Callback(const AmsAddr* pAddr, const AdsNotificationHeader* pNotification, uint32_t hUser)
{
    Q_UNUSED(pAddr);
    QMutexLocker locker(&m_pointerVectorMutex);
    Q_UNUSED(locker);
    quint8 returnValue = 0;
    bool value;
    QADSBOOL::QADSBOOLCallbackClass *sender = m_pointerVector[hUser];

    const uint8_t* data = reinterpret_cast<const uint8_t*>(pNotification + 1);
    returnValue = *(quint8 *)data;
    value = (returnValue > 0);

    if( sender != Q_NULLPTR )
    {
        sender->setValue(value);
    }
}
#endif

QADSBOOL::QADSBOOL(QObject *parent, quint16 amsport, const QString &amsnetid, const QString &amshost,
                   const QString &var, PLCVariableReadOperation readop, unsigned long cycletime) :
    QADSPLCVariable(parent,amsport,amsnetid,amshost,var,readop,cycletime),
    m_value(false),
    m_adsBOOLCallback(Q_NULLPTR),
    m_variableCallBackNotificationHandle(0)
{
    parseVariableType();
    readValue();
    setupVariableCallback();
}

QADSBOOL::~QADSBOOL()
{
    resetVariableCallback();
}

void QADSBOOL::setupVariableCallback()
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
            m_adsErrorString = tr("QADSBOOL, Line ");
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
            if ( m_adsBOOLCallback == Q_NULLPTR )
            {
                m_adsBOOLCallback = new QADSBOOL::QADSBOOLCallbackClass();
                QADSBOOL::QADSBOOLCallbackClass::m_pointerVector.append(m_adsBOOLCallback);
            }

            // Get pointer index.
            hUser = QADSBOOL::QADSBOOLCallbackClass::m_pointerVector.size()-1;

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
                m_adsErrorString = tr("QADSBOOL, Line ");
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
                        m_adsErrorString = tr("QADSBOOL, Line ");
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
                                                             QADSBOOL::QADSBOOLCallbackClass::Callback, hUser,
                                                             &m_variableCallBackNotificationHandle);
#else
                    uint32_t handle = 0;
                    nErr = AdsSyncAddDeviceNotificationReqEx(adsPort(), pAddr, ADSIGRP_SYM_VALBYHND,
                                                             adsSymbolHandle(), &adsNotificationAttrib,
                                                             QADSBOOL::QADSBOOLCallbackClass::Callback, hUser,
                                                             &handle);
                    m_variableCallBackNotificationHandle = handle;
#endif
                    if (nErr)
                    {
                        m_adsError = true;
                        m_adsErrorString = tr("QADSBOOL, Line ");
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
                        connect(m_adsBOOLCallback, SIGNAL(valueChanged(bool)),
                                this, SLOT(privSetValue(bool)), static_cast<Qt::ConnectionType>(Qt::QueuedConnection | Qt::UniqueConnection));
                    }
                }
            }
        }
    }
}

void QADSBOOL::resetVariableCallback()
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
    if( m_adsBOOLCallback )
    {
        QMutexLocker locker(&QADSBOOL::QADSBOOLCallbackClass::m_pointerVectorMutex);
        Q_UNUSED(locker);
        int index = QADSBOOL::QADSBOOLCallbackClass::m_pointerVector.indexOf(m_adsBOOLCallback);
        if (index >= 0)
        {
            QADSBOOL::QADSBOOLCallbackClass::m_pointerVector[index] = Q_NULLPTR;
        }
        delete m_adsBOOLCallback;
        m_adsBOOLCallback = Q_NULLPTR;
    }
}

void QADSBOOL::parseVariableType()
{
    // Only works for the BOOL type!
    if( !m_adsError && (adsSymbolType() != "BOOL") )
    {
        m_adsError = true;
        m_adsErrorString = tr("QADSBOOL, Line ");
        m_adsErrorString += QString("%1").arg(__LINE__);
        m_adsErrorString += ": ";
        m_adsErrorString += tr("Error: %1 is a %2 type instead of BOOL as required by this class. Please check the PLC declaration.")
                .arg(plcVariableName()).arg(adsSymbolType());
        Q_EMIT adsErrorChanged();
        Q_EMIT adsErrorStringChanged();
    }
}

void QADSBOOL::protCatchADSStateChange()
{
    // The parent implementation must be called first
    QADSPLCVariable::protCatchADSStateChange();
    parseVariableType();
    readValue();
    resetVariableCallback();
    setupVariableCallback();
}

void QADSBOOL::protCatchADSSymbolTableChanged()
{
    // The parent implementation must be called first
    QADSPLCVariable::protCatchADSSymbolTableChanged();
    parseVariableType();
    readValue();
    resetVariableCallback();
    setupVariableCallback();
}

void QADSBOOL::readValue()
{
    if( !m_adsError )
    {
        quint8 returnValue = m_value;
        syncReadRequest(amsPort(), ADSIGRP_SYM_VALBYHND, adsSymbolHandle(), sizeof(returnValue), &returnValue, Q_NULLPTR);
        if ( !m_adsError )
        {
            privSetValue(returnValue > 0);
        }
    }
}

bool QADSBOOL::value()
{
    // Read the value depending on the read operation mode.
    if( plcVariableReadOperation() == ON_DEMAND )
    {
        readValue();
    }
    return m_value;
}

void QADSBOOL::initializeADSConnection()
{
    QADSPLCVariable::initializeADSConnection();
    parseVariableType();
    readValue();
    resetVariableCallback();
    setupVariableCallback();
}

void QADSBOOL::setValue(bool val)
{
    // Set value of variable if changed & no error
    if( !m_adsError && (val != value()) )
    {
        quint8 sendValue = val;
        syncWriteRequest(amsPort(), ADSIGRP_SYM_VALBYHND, adsSymbolHandle(), sizeof(sendValue), &sendValue);
    }
    if( !m_adsError && (plcVariableReadOperation() == ON_DEMAND) )
    {
       privSetValue(val);
    }
}

void QADSBOOL::privSetValue(bool val)
{
    if( val != m_value )
    {
        m_value = val;
        Q_EMIT valueChanged();
    }
}
