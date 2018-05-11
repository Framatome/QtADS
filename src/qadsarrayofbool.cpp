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
#include "qadsarrayofbool.h"
#include <QStringList>
#include <QMutexLocker>
#include <QVector>
#if defined(USE_TWINCAT)
#include <windows.h>
#include "tcadsdef.h"
#include "tcadsapi.h"
#else
#include <AdsLib.h>
#endif

void QADSARRAYOFBOOLCallbackClassTemplate::setValue(const QADSBOOLARRAY &val)
{
    Q_EMIT valueChanged(val);
}

class QADSARRAYOFBOOL::QADSARRAYOFBOOLCallbackClass : public QADSARRAYOFBOOLCallbackClassTemplate
{
public:
    QADSARRAYOFBOOLCallbackClass(unsigned int z, unsigned int y, unsigned x)
        :QADSARRAYOFBOOLCallbackClassTemplate(z,y,x)
    {}
#ifdef USE_TWINCAT
    static void __stdcall Callback(AmsAddr*, AdsNotificationHeader*, unsigned long);
#else
    static void Callback(const AmsAddr*, const AdsNotificationHeader*, uint32_t);
#endif
    static QMutex m_pointerVectorMutex;
    static QVector<QADSARRAYOFBOOL::QADSARRAYOFBOOLCallbackClass*> m_pointerVector;
};

QMutex QADSARRAYOFBOOL::QADSARRAYOFBOOLCallbackClass::m_pointerVectorMutex;
QVector<QADSARRAYOFBOOL::QADSARRAYOFBOOLCallbackClass*> QADSARRAYOFBOOL::QADSARRAYOFBOOLCallbackClass::m_pointerVector;


// Callback-function
#ifdef USE_TWINCAT
void __stdcall QADSARRAYOFBOOL::QADSARRAYOFBOOLCallbackClass::Callback(AmsAddr* pAddr, AdsNotificationHeader* pNotification, unsigned long hUser)
{
    Q_UNUSED(pAddr);
    QMutexLocker locker(&m_pointerVectorMutex);
    Q_UNUSED(locker);
    QADSARRAYOFBOOL::QADSARRAYOFBOOLCallbackClass *sender = m_pointerVector[hUser];

    if( sender != Q_NULLPTR )
    {
        QADSBOOLARRAY value(sender->z(), sender->y(), sender->x());

        Q_ASSERT((pNotification->cbSampleSize/sizeof(quint8)) == sender->count());
        for(unsigned int i=0; i<value.count(); i++)
        {
            value[i] = (*(((quint8 *)pNotification->data)+i) > 0);
        }
        sender->setValue(value);
    }
}
#else
void QADSARRAYOFBOOL::QADSARRAYOFBOOLCallbackClass::Callback(const AmsAddr* pAddr, const AdsNotificationHeader* pNotification, uint32_t hUser)
{
    Q_UNUSED(pAddr);
    QMutexLocker locker(&m_pointerVectorMutex);
    Q_UNUSED(locker);
    QADSARRAYOFBOOL::QADSARRAYOFBOOLCallbackClass *sender = m_pointerVector[hUser];

    if( sender != Q_NULLPTR )
    {
        QADSBOOLARRAY value(sender->z(), sender->y(), sender->x());

        Q_ASSERT((pNotification->cbSampleSize/sizeof(quint8)) == sender->count());
        for(unsigned int i=0; i<value.count(); i++)
        {
            const uint8_t* data = reinterpret_cast<const uint8_t*>(pNotification + 1);
            value[i] = (*(((quint8 *)data)+i) > 0);
        }
        sender->setValue(value);
    }
}
#endif

int QADSARRAYOFBOOL::m_metaTypeId = -1;

QADSARRAYOFBOOL::QADSARRAYOFBOOL(QObject *parent, quint16 amsport, const QString &amsnetid, const QString &amshost,
                                 const QString &var, PLCVariableReadOperation readop, unsigned long cycletime) :
    QADSPLCVariable(parent,amsport,amsnetid,amshost,var,readop,cycletime),
    m_adsARRAYOFBOOLCallback(Q_NULLPTR),
    m_variableCallBackNotificationHandle(0)
{
    // Register new meta type if not already registered.
    if( !QMetaType::isRegistered(m_metaTypeId) )
    {
        m_metaTypeId = qRegisterMetaType<QADSBOOLARRAY>("QADSBOOLARRAY");
    }

    parseArrayParameters();
    readValue();
    setupVariableCallback();
}

QADSARRAYOFBOOL::~QADSARRAYOFBOOL()
{
    resetVariableCallback();
}

void QADSARRAYOFBOOL::parseArrayParameters()
{
    // Only works for the ARRAY[] OF BOOL type!
    bool tempError = false;
    // Split string by OF should produce 2 strings ARRAY[] and BOOL.
    QStringList splitSymbolName = adsSymbolType().split(QString("OF"),QString::SkipEmptyParts);
    if( splitSymbolName.count() != 2 )
    {
        tempError = true;
    }
    else
    {
        if( splitSymbolName[1].trimmed() != "BOOL" )
        {
            tempError = true;
        }
        else
        {
            // Parse array dimensions.
            // Get string between [ and ].
            QString arrayIndicesString = splitSymbolName[0].trimmed().section('[',1,1).section(']',0,0);
            if( arrayIndicesString == "" )
            {
                tempError = true;
            }
            else
            {
                QStringList arrayIndices = arrayIndicesString.split(',');
                if( (arrayIndices.count() < 1) || (arrayIndices.count() > 3) )
                {
                    tempError = true;
                }
                else
                {
                    // Calculate size of each dimension.
                    int sizes[3] = {0,0,0};
                    for(int i=0; i<arrayIndices.count(); i++)
                    {
                        QStringList indices = arrayIndices[i].trimmed().split("..");
                        if( indices.count() != 2 )
                        {
                            tempError = true;
                            break;
                        }
                        else
                        {
                            sizes[i] = indices[1].toInt()-indices[0].toInt()+1;
                        }
                    }
                    if( !tempError )
                    {
                        if( sizes[2] != 0 )
                        {
                            m_value = QADSBOOLARRAY(sizes[0],sizes[1],sizes[2]);
                        }
                        else if( sizes[1] != 0 )
                        {
                            m_value = QADSBOOLARRAY(sizes[0],sizes[1]);
                        }
                        else if( sizes[0] != 0 )
                        {
                            m_value = QADSBOOLARRAY(sizes[0]);
                        }
                        else
                        {
                            tempError = true;
                        }
                    }
                }
            }
        }
    }
    if( !m_adsError && tempError )
    {
        m_adsError = true;
        m_adsErrorString = tr("QADSARRAYOFBOOL, Line ");
        m_adsErrorString += QString("%1").arg(__LINE__);
        m_adsErrorString += ": ";
        m_adsErrorString += tr("Error: %1 is a %2 type instead of an ARRAY[] OF BOOL as required by this class. Please check the PLC declaration.").arg(adsSymbolName()).arg(adsSymbolType());
        Q_EMIT adsErrorChanged();
        Q_EMIT adsErrorStringChanged();
    }
    if( !m_adsError && (adsSymbolSize() != (m_value.count()*sizeof(quint8))) )
    {
        m_adsError = true;
        m_adsErrorString = tr("QADSARRAYOFBOOL, Line ");
        m_adsErrorString += QString("%1").arg(__LINE__);
        m_adsErrorString += ": ";
        m_adsErrorString += tr("Error: ADS symbol size = %1 does not equal the parsed array data size = %2.").arg(adsSymbolSize()).arg(m_value.count()*sizeof(quint8));
        Q_EMIT adsErrorChanged();
        Q_EMIT adsErrorStringChanged();
    }
}

void QADSARRAYOFBOOL::setupVariableCallback()
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
            m_adsErrorString = tr("QADSARRAYOFBOOL, Line ");
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
            if ( m_adsARRAYOFBOOLCallback == Q_NULLPTR )
            {
                m_adsARRAYOFBOOLCallback = new QADSARRAYOFBOOL::QADSARRAYOFBOOLCallbackClass(m_value.z(),m_value.y(),m_value.x());
                QADSARRAYOFBOOL::QADSARRAYOFBOOLCallbackClass::m_pointerVector.append(m_adsARRAYOFBOOLCallback);
            }

            // Get pointer index.
            hUser = QADSARRAYOFBOOL::QADSARRAYOFBOOLCallbackClass::m_pointerVector.size()-1;

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
                m_adsErrorString = tr("QADSARRAYOFBOOL, Line ");
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
                        m_adsErrorString = tr("QADSARRAYOFBOOL, Line ");
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
                                                             QADSARRAYOFBOOL::QADSARRAYOFBOOLCallbackClass::Callback, hUser,
                                                             &m_variableCallBackNotificationHandle);
#else
                    uint32_t handle = 0;
                    nErr = AdsSyncAddDeviceNotificationReqEx(adsPort(), pAddr, ADSIGRP_SYM_VALBYHND,
                                                             adsSymbolHandle(), &adsNotificationAttrib,
                                                             QADSARRAYOFBOOL::QADSARRAYOFBOOLCallbackClass::Callback, hUser,
                                                             &handle);
                    m_variableCallBackNotificationHandle = handle;
#endif
                    if (nErr)
                    {
                        m_adsError = true;
                        m_adsErrorString = tr("QADSARRAYOFBOOL, Line ");
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
                        connect(m_adsARRAYOFBOOLCallback, SIGNAL(valueChanged(QADSBOOLARRAY)),
                                this, SLOT(privSetValue(QADSBOOLARRAY)), static_cast<Qt::ConnectionType>(Qt::QueuedConnection | Qt::UniqueConnection));
                    }
                }
            }
        }
    }
}

void QADSARRAYOFBOOL::resetVariableCallback()
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
    if( m_adsARRAYOFBOOLCallback )
    {
        QMutexLocker locker(&QADSARRAYOFBOOL::QADSARRAYOFBOOLCallbackClass::m_pointerVectorMutex);
        Q_UNUSED(locker);
        int index = QADSARRAYOFBOOL::QADSARRAYOFBOOLCallbackClass::m_pointerVector.indexOf(m_adsARRAYOFBOOLCallback);
        if (index >= 0)
        {
            QADSARRAYOFBOOL::QADSARRAYOFBOOLCallbackClass::m_pointerVector[index] = Q_NULLPTR;
        }
        delete m_adsARRAYOFBOOLCallback;
        m_adsARRAYOFBOOLCallback = Q_NULLPTR;
    }
}

void QADSARRAYOFBOOL::protCatchADSStateChange()
{
    // The parent implementation must be called first
    QADSPLCVariable::protCatchADSStateChange();
    parseArrayParameters();
    readValue();
    resetVariableCallback();
    setupVariableCallback();
}

void QADSARRAYOFBOOL::protCatchADSSymbolTableChanged()
{
    // The parent implementation must be called first
    QADSPLCVariable::protCatchADSSymbolTableChanged();
    parseArrayParameters();
    readValue();
    resetVariableCallback();
    setupVariableCallback();
}

void QADSARRAYOFBOOL::readValue()
{
    if( !m_adsError )
    {
        quint8 *returnValue = new quint8[m_value.count()];
        Q_ASSERT(returnValue);
        syncReadRequest(amsPort(), ADSIGRP_SYM_VALBYHND, adsSymbolHandle(), m_value.count()*sizeof(quint8), returnValue, Q_NULLPTR);
        if ( !m_adsError )
        {
            bool theValueChanged = false;
            bool value;
            for(unsigned int i=0; i<m_value.count(); i++)
            {
                value = (*(returnValue+i) > 0);
                if( value != m_value[i])
                {
                    m_value[i] = value;
                    theValueChanged = true;
                }
            }
            if (theValueChanged)
            {
                Q_EMIT valueChanged();
            }
        }
        delete[] returnValue;
    }
}

const QADSBOOLARRAY &QADSARRAYOFBOOL::value()
{
    // Read the value depending on the read operation mode.
    if( plcVariableReadOperation() == ON_DEMAND )
    {
        readValue();
    }
    return m_value;
}

bool QADSARRAYOFBOOL::value(int x)
{
    return value()[x];
}

bool QADSARRAYOFBOOL::value(int y, int x)
{
    return value()(y,x);
}

bool QADSARRAYOFBOOL::value(int z, int y, int x)
{
    return value()(z,y,x);
}

void QADSARRAYOFBOOL::initializeADSConnection()
{
    QADSPLCVariable::initializeADSConnection();
    parseArrayParameters();
    readValue();
    resetVariableCallback();
    setupVariableCallback();
}

void QADSARRAYOFBOOL::setValue(const QADSBOOLARRAY &val)
{
    // Set value of variable if changed & no error
    if( !m_adsError && (val != value()) )
    {
        quint8 *writeValue = new quint8[val.count()];
        for(unsigned int i=0; i<val.count(); i++)
        {
            *(writeValue+i) = val[i];
        }
        syncWriteRequest(amsPort(), ADSIGRP_SYM_VALBYHND, adsSymbolHandle(), sizeof(quint8)*val.count(), writeValue);
        delete[] writeValue;
    }
    if( !m_adsError && (plcVariableReadOperation() == ON_DEMAND) )
    {
       privSetValue(val);
    }
}

void QADSARRAYOFBOOL::setValue(bool val, int x)
{
    // Set value of variable if changed & no error
    if( !m_adsError && (val != value()[x]) )
    {
        QADSBOOLARRAY tempValue = value();
        tempValue[x] = val;
        setValue(tempValue);
    }
}

void QADSARRAYOFBOOL::setValue(bool val, int y, int x)
{
    // Set value of variable if changed & no error
    if( !m_adsError && (val != value()(y,x)) )
    {
        QADSBOOLARRAY tempValue = value();
        tempValue(y,x) = val;
        setValue(tempValue);
    }
}

void QADSARRAYOFBOOL::setValue(bool val, int z, int y, int x)
{
    // Set value of variable if changed & no error
    if( !m_adsError && (val != value()(z,y,x)) )
    {
        QADSBOOLARRAY tempValue = value();
        tempValue(z,y,x) = val;
        setValue(tempValue);
    }
}

void QADSARRAYOFBOOL::privSetValue(const QADSBOOLARRAY &val)
{
    if( val != m_value )
    {
        m_value = val;
        Q_EMIT valueChanged();
    }
}

