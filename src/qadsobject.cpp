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
#include "qadsobject.h"
#include <QVector>
#include <QMutexLocker>
#include <QTimer>
#if defined(USE_TWINCAT)
#include <windows.h>
#include <tcadsdef.h>
#include <tcadsapi.h>
#else
#include <AdsLib.h>
#endif
#include "qadssymbols.h"

void QADSObjectCallbackClassTemplate::setADSState(int state)
{
    Q_EMIT notifyAllADSStates(state);
}

void QADSObjectCallbackClassTemplate::setAMSEvent(int event)
{
    Q_EMIT notifyAllAMSEvents(event);
}

void QADSObjectCallbackClassTemplate::setADSSymbolTableChanged()
{
    Q_EMIT notifyAllADSSymbolTableChanged();
}

class QADSObject::QADSObjectCallbackClass : public QADSObjectCallbackClassTemplate
{
public:
#ifdef USE_TWINCAT
    static void __stdcall AMSRouterCallback(long);
    static void __stdcall ADSStateCallback(AmsAddr*, AdsNotificationHeader*, unsigned long);
    static void __stdcall ADSSymbolTableChangedCallback(AmsAddr*, AdsNotificationHeader*, unsigned long);
#else
    static void ADSStateCallback(const AmsAddr*, const AdsNotificationHeader*, uint32_t);
    static void ADSSymbolTableChangedCallback(const AmsAddr*, const AdsNotificationHeader*, uint32_t);
#endif
    static QMutex m_pointerVectorMutex;
    static QVector<QADSObject::QADSObjectCallbackClass*> m_pointerVector;
};

QMutex QADSObject::QADSObjectCallbackClass::m_pointerVectorMutex;
QVector<QADSObject::QADSObjectCallbackClass*> QADSObject::QADSObjectCallbackClass::m_pointerVector;

// AMS Router event callback function
#ifdef USE_TWINCAT
void __stdcall QADSObject::QADSObjectCallbackClass::AMSRouterCallback (long nReason)
{
    if( QADSObject::m_adsObjectAMSEventCallback != Q_NULLPTR )
    {
        switch (nReason)
        {
        case AMSEVENT_ROUTERSTOP:
            QADSObject::m_adsObjectAMSEventCallback->setAMSEvent(QADSObject::ROUTERSTOP);
            break;
        case AMSEVENT_ROUTERSTART:
            QADSObject::m_adsObjectAMSEventCallback->setAMSEvent(QADSObject::ROUTERSTART);
            break;
        case AMSEVENT_ROUTERREMOVED:
            QADSObject::m_adsObjectAMSEventCallback->setAMSEvent(QADSObject::ROUTERREMOVED);
            break;
        default:
            break;
        }
    }
}
#endif

// ADS state callback function
#ifdef USE_TWINCAT
void __stdcall QADSObject::QADSObjectCallbackClass::ADSStateCallback(AmsAddr* pAddr, AdsNotificationHeader* pNotification, unsigned long hUser)
{
    Q_UNUSED(pAddr);
    INT       nIndex;
    QMutexLocker locker(&m_pointerVectorMutex);
    Q_UNUSED(locker);
    QADSObject::QADSObjectCallbackClass *sender = m_pointerVector[hUser];

    nIndex = *(short *)pNotification->data;
    //std::cout << "ADSStateCallback: " << nIndex << std::endl;

    if( sender != Q_NULLPTR )
    {
        switch (nIndex)
        {
        case ADSSTATE_INVALID:
            sender->setADSState(QADSObject::STATE_INVALID);
            break;
        case ADSSTATE_IDLE:
            sender->setADSState(QADSObject::STATE_IDLE);
            break;
        case ADSSTATE_RESET:
            sender->setADSState(QADSObject::STATE_RESET);
            break;
        case ADSSTATE_INIT:
            sender->setADSState(QADSObject::STATE_INIT);
            break;
        case ADSSTATE_START:
            sender->setADSState(QADSObject::STATE_START);
            break;
        case ADSSTATE_RUN:
            sender->setADSState(QADSObject::STATE_RUN);
            break;
        case ADSSTATE_STOP:
            sender->setADSState(QADSObject::STATE_STOP);
            break;
        case ADSSTATE_SAVECFG:
            sender->setADSState(QADSObject::STATE_SAVECFG);
            break;
        case ADSSTATE_LOADCFG:
            sender->setADSState(QADSObject::STATE_LOADCFG);
            break;
        case ADSSTATE_POWERFAILURE:
            sender->setADSState(QADSObject::STATE_POWERFAILURE);
            break;
        case ADSSTATE_POWERGOOD:
            sender->setADSState(QADSObject::STATE_POWERGOOD);
            break;
        case ADSSTATE_ERROR:
            sender->setADSState(QADSObject::STATE_ERROR);
            break;
        case ADSSTATE_SHUTDOWN:
            sender->setADSState(QADSObject::STATE_SHUTDOWN);
            break;
        case ADSSTATE_SUSPEND:
            sender->setADSState(QADSObject::STATE_SUSPEND);
            break;
        case ADSSTATE_RESUME:
            sender->setADSState(QADSObject::STATE_RESUME);
            break;
        case ADSSTATE_CONFIG:
            sender->setADSState(QADSObject::STATE_CONFIG);
            break;
        case ADSSTATE_RECONFIG:
            sender->setADSState(QADSObject::STATE_RECONFIG);
            break;
        case ADSSTATE_MAXSTATES:
            sender->setADSState(QADSObject::STATE_MAXSTATES);
            break;
        default:
            break;
        }
    }
}
#else
void QADSObject::QADSObjectCallbackClass::ADSStateCallback(const AmsAddr* pAddr, const AdsNotificationHeader* pNotification, uint32_t hUser)
{
    Q_UNUSED(pAddr);
    short       nIndex;
    QMutexLocker locker(&m_pointerVectorMutex);
    Q_UNUSED(locker);
    QADSObject::QADSObjectCallbackClass *sender = m_pointerVector[hUser];

    const uint8_t* data = reinterpret_cast<const uint8_t*>(pNotification + 1);
    nIndex = *(short *)data;
    //std::cout << "ADSStateCallback: " << nIndex << std::endl;

    if( sender != Q_NULLPTR )
    {
        switch (nIndex)
        {
        case ADSSTATE_INVALID:
            sender->setADSState(QADSObject::STATE_INVALID);
            break;
        case ADSSTATE_IDLE:
            sender->setADSState(QADSObject::STATE_IDLE);
            break;
        case ADSSTATE_RESET:
            sender->setADSState(QADSObject::STATE_RESET);
            break;
        case ADSSTATE_INIT:
            sender->setADSState(QADSObject::STATE_INIT);
            break;
        case ADSSTATE_START:
            sender->setADSState(QADSObject::STATE_START);
            break;
        case ADSSTATE_RUN:
            sender->setADSState(QADSObject::STATE_RUN);
            break;
        case ADSSTATE_STOP:
            sender->setADSState(QADSObject::STATE_STOP);
            break;
        case ADSSTATE_SAVECFG:
            sender->setADSState(QADSObject::STATE_SAVECFG);
            break;
        case ADSSTATE_LOADCFG:
            sender->setADSState(QADSObject::STATE_LOADCFG);
            break;
        case ADSSTATE_POWERFAILURE:
            sender->setADSState(QADSObject::STATE_POWERFAILURE);
            break;
        case ADSSTATE_POWERGOOD:
            sender->setADSState(QADSObject::STATE_POWERGOOD);
            break;
        case ADSSTATE_ERROR:
            sender->setADSState(QADSObject::STATE_ERROR);
            break;
        case ADSSTATE_SHUTDOWN:
            sender->setADSState(QADSObject::STATE_SHUTDOWN);
            break;
        case ADSSTATE_SUSPEND:
            sender->setADSState(QADSObject::STATE_SUSPEND);
            break;
        case ADSSTATE_RESUME:
            sender->setADSState(QADSObject::STATE_RESUME);
            break;
        case ADSSTATE_CONFIG:
            sender->setADSState(QADSObject::STATE_CONFIG);
            break;
        case ADSSTATE_RECONFIG:
            sender->setADSState(QADSObject::STATE_RECONFIG);
            break;
        case ADSSTATE_MAXSTATES:
            sender->setADSState(QADSObject::STATE_MAXSTATES);
            break;
        default:
            break;
        }
    }
}
#endif

// ADS state callback function
#ifdef USE_TWINCAT
void __stdcall QADSObject::QADSObjectCallbackClass::ADSSymbolTableChangedCallback(AmsAddr* pAddr, AdsNotificationHeader* pNotification, unsigned long hUser)
{
    Q_UNUSED(pAddr);
    Q_UNUSED(pNotification);
    QADSObject::QADSObjectCallbackClass *sender = m_pointerVector[hUser];

    if( sender != Q_NULLPTR )
    {
        sender->notifyAllADSSymbolTableChanged();
    }
}
#else
void QADSObject::QADSObjectCallbackClass::ADSSymbolTableChangedCallback(const AmsAddr* pAddr, const AdsNotificationHeader* pNotification, uint32_t hUser)
{
    Q_UNUSED(pAddr);
    Q_UNUSED(pNotification);
    QMutexLocker locker(&m_pointerVectorMutex);
    Q_UNUSED(locker);
    QADSObject::QADSObjectCallbackClass *sender = m_pointerVector[hUser];

    if( sender != Q_NULLPTR )
    {
        sender->notifyAllADSSymbolTableChanged();
    }
}
#endif

//Instantiate static callback class as Q_NULLPTR-Pointer
QADSObject::QADSObjectCallbackClass* QADSObject::m_adsObjectAMSEventCallback=Q_NULLPTR;

// Set instance count to 0
int QADSObject::m_instanceCount = 0;

QMutex QADSObject::m_amsEventMutex;

int QADSObject::m_adsStateMetaTypeId = -1;

int QADSObject::m_amsRouterEventMetaTypeId = -1;

QADSObject::QADSObject(QObject *parent, quint16 amsport, const QString &amsnetid, const QString &amshost) :
    QObject(parent),
    m_adsError(false),
    m_adsErrorString(""),
    m_adsPort(0),
    m_amsPort(amsport),
    m_amsNetId(amsnetid),
    m_amsHost(amshost),
    m_adsDllVersion(0),
    m_adsDllRevision(0),
    m_adsDllBuild(0),
    m_adsDeviceName(tr("Unknown")),
    m_adsDeviceVersion(0),
    m_adsDeviceRevision(0),
    m_adsDeviceBuild(0),
    m_adsState(STATE_INVALID),
    m_amsEvent(ROUTERUNKNOWN),
    m_adsObjectCallback(Q_NULLPTR),
    m_adsStateNotificationHandle(0),
    m_adsSymbolTableChangedNotificationHandle(0)
{
    // Register new meta type if not already registered.
    if( !QMetaType::isRegistered(m_adsStateMetaTypeId) )
    {
        m_adsStateMetaTypeId = qRegisterMetaType<ADSState>("QADSObject::ADSState");
    }
    if( !QMetaType::isRegistered(m_amsRouterEventMetaTypeId) )
    {
        m_amsRouterEventMetaTypeId = qRegisterMetaType<AMSRouterEvent>("QADSObject::AMSRouterEvent");
    }
    // Increment instance count.
    m_amsEventMutex.lock();
    ++m_instanceCount;
    m_amsEventMutex.unlock();

    // Setup AMS event callback.
    setupAMSEventCallback();

    setupADS();
}

QADSObject::~QADSObject()
{
    resetADS();
    m_amsEventMutex.lock();
    --m_instanceCount;
    if (m_instanceCount == 0)
    {
        resetAMS();
    }
    m_amsEventMutex.unlock();
}

void QADSObject::setupADS()
{
    // Various local variables.
    long  nErr;

    // Setup DLL Members
    setupDLLMemberVariables();

    // Setup the ADS port.
    m_adsPort = AdsPortOpenEx();

    // Check port for errors
    if( m_adsPort == 0 )
    {
        m_adsError = true;
        m_adsErrorString = tr("QADSObject, Line ");
        m_adsErrorString += QString("%1").arg(__LINE__);
        m_adsErrorString += ": ";
        m_adsErrorString += tr(" Could not open ADS port");
        Q_EMIT adsErrorChanged();
        Q_EMIT adsErrorStringChanged();
    }

    // Setup the AMS net ID.
    if ( !m_adsError )
    {
        if( m_amsNetId == "local" )
        {
            AmsAddr Addr;
            AmsAddr* pAddr = &Addr;
            nErr = AdsGetLocalAddressEx(m_adsPort, pAddr);
            if( nErr )
            {
                m_adsError = true;
                m_adsErrorString = tr("QADSObject, Line ");
                m_adsErrorString += QString("%1").arg(__LINE__);
                m_adsErrorString += ": ";
                m_adsErrorString += generateADSErrorString(nErr);
                Q_EMIT adsErrorChanged();
                Q_EMIT adsErrorStringChanged();
            }
            else
            {
                m_amsNetId = addressToString(Addr.netId.b);
                Q_EMIT amsNetIdChanged();
            }
        }
    }

    if( !m_adsError )
    {
       // Setup the device
       setupDeviceMemberVariables();

       // Setup the ADS state.
       setupADSState();

       // Get list of symbol names.
       getListOfADSSymbolNames();

       // Finally setup other callbacks if we get this far.
       setupADSSymbolChangedCallback();
    }
}

void QADSObject::resetADS()
{
    if( m_adsObjectCallback != Q_NULLPTR )
    {
        AmsAddr Addr;
        AmsAddr *pAddr = &Addr;
        Addr.port = m_amsPort;
        stringToAddress(m_amsNetId, Addr.netId.b);
        AdsSyncDelDeviceNotificationReqEx(m_adsPort, pAddr, m_adsStateNotificationHandle);
        AdsSyncDelDeviceNotificationReqEx(m_adsPort, pAddr, m_adsSymbolTableChangedNotificationHandle);
        QMutexLocker locker(&QADSObject::QADSObjectCallbackClass::m_pointerVectorMutex);
        Q_UNUSED(locker);
        int index = QADSObject::QADSObjectCallbackClass::m_pointerVector.indexOf(m_adsObjectCallback);
        if (index >= 0)
        {
            QADSObject::QADSObjectCallbackClass::m_pointerVector[index] = Q_NULLPTR;
        }
        delete m_adsObjectCallback;
        m_adsObjectCallback = Q_NULLPTR;
    }
    if (m_adsPort != 0)
    {
        AdsPortCloseEx(m_adsPort);
        m_adsPort = 0;
    }
    if (m_adsError)
    {
        m_adsError = false;
        Q_EMIT adsErrorChanged();
    }
    if (m_adsErrorString != "")
    {
        m_adsErrorString = "";
        Q_EMIT adsErrorStringChanged();
    }
}

void QADSObject::resetAMS()
{
#ifdef USE_TWINCAT
    if ( m_adsObjectAMSEventCallback != Q_NULLPTR )
    {
        AdsAmsUnRegisterRouterNotification();
        AdsPortClose();
        delete m_adsObjectAMSEventCallback;
        m_adsObjectAMSEventCallback = Q_NULLPTR;
    }
#else
    if ( m_adsObjectAMSEventCallback != Q_NULLPTR )
    {
        AdsDelRoute(AmsNetId(m_amsNetId.toStdString()));
        delete m_adsObjectAMSEventCallback;
        m_adsObjectAMSEventCallback = Q_NULLPTR;
    }
#endif
}

void QADSObject::syncWriteRequest(unsigned short nPort, unsigned long nIndexGroup, unsigned long nIndexOffset, unsigned long nLength, const void *pData)
{
    long nErr;
    AmsAddr Addr;
    AmsAddr *pAddr = &Addr;

    Addr.port = nPort;
    if( !stringToAddress(m_amsNetId, Addr.netId.b) )
    {
        m_adsError = true;
        m_adsErrorString = tr("QADSObject, Line ");
        m_adsErrorString += QString("%1").arg(__LINE__);
        m_adsErrorString += ": ";
        m_adsErrorString += tr("Error: %1 is not a valid AMS Net ID. AMS Net ID's are of the form XXX.XXX.XXX.XXX.1.1, where XXX is a number between 1 and 255.").arg(amsNetId());
        Q_EMIT adsErrorChanged();
        Q_EMIT adsErrorStringChanged();
    }
    if( !m_adsError )
    {
#ifdef USE_TWINCAT
        nErr = AdsSyncWriteReqEx(m_adsPort, pAddr, nIndexGroup, nIndexOffset,
                                 nLength, const_cast<void*>(pData));
#else
        nErr = AdsSyncWriteReqEx(m_adsPort, pAddr, nIndexGroup, nIndexOffset,
                                 nLength, pData);
#endif
        if (nErr)
        {
            m_adsError = true;
            m_adsErrorString = tr("QADSObject, Line ");
            m_adsErrorString += QString("%1").arg(__LINE__);
            m_adsErrorString += QString(", nIndexGroup %1, nIndexOffset %2: ").arg(nIndexGroup, 1, 16).arg(nIndexOffset, 1, 16);
            m_adsErrorString += generateADSErrorString(nErr);
            Q_EMIT adsErrorChanged();
            Q_EMIT adsErrorStringChanged();
        }
    }
}

void QADSObject::syncReadRequest(unsigned short nPort, unsigned long nIndexGroup, unsigned long nIndexOffset, unsigned long nLength, void *pData,
                     unsigned long* pcbReturn)
{
    long nErr;
    AmsAddr Addr;
    AmsAddr *pAddr = &Addr;

    Addr.port = nPort;
    if( !stringToAddress(m_amsNetId, Addr.netId.b) )
    {
        m_adsError = true;
        m_adsErrorString = tr("QADSObject, Line ");
        m_adsErrorString += QString("%1").arg(__LINE__);
        m_adsErrorString += ": ";
        m_adsErrorString += tr("Error: %1 is not a valid AMS Net ID. AMS Net ID's are of the form XXX.XXX.XXX.XXX.1.1, where XXX is a number between 1 and 255.").arg(amsNetId());
        Q_EMIT adsErrorChanged();
        Q_EMIT adsErrorStringChanged();
    }
    if( !m_adsError )
    {
#ifdef USE_TWINCAT
        nErr = AdsSyncReadReqEx2(m_adsPort, pAddr, nIndexGroup, nIndexOffset,
                                 nLength, pData, pcbReturn );
#else
        uint32_t returnValue = 0;
        nErr = AdsSyncReadReqEx2(m_adsPort, pAddr, nIndexGroup, nIndexOffset,
                                 nLength, pData, &returnValue );
        if (pcbReturn != Q_NULLPTR)
        {
            *pcbReturn = returnValue;
        }
#endif
        if (nErr)
        {
            m_adsError = true;
            m_adsErrorString = tr("QADSObject, Line ");
            m_adsErrorString += QString("%1").arg(__LINE__);
            m_adsErrorString += QString(", nIndexGroup %1, nIndexOffset %2: ").arg(nIndexGroup, 1, 16).arg(nIndexOffset, 1, 16);
            m_adsErrorString += generateADSErrorString(nErr);
            Q_EMIT adsErrorChanged();
            Q_EMIT adsErrorStringChanged();
        }
    }
}

void QADSObject::syncReadWriteRequest(unsigned short nPort, unsigned long nIndexGroup, unsigned long nIndexOffset, unsigned long nReadLength, void* pReadData,
                          unsigned long nWriteLength, const void *pWriteData, unsigned long* pcbReturn)
{
    long nErr;
    AmsAddr Addr;
    AmsAddr *pAddr = &Addr;

    Addr.port = nPort;
    if( !stringToAddress(m_amsNetId, Addr.netId.b) )
    {
        m_adsError = true;
        m_adsErrorString = tr("QADSObject, Line ");
        m_adsErrorString += QString("%1").arg(__LINE__);
        m_adsErrorString += ": ";
        m_adsErrorString += tr("Error: %1 is not a valid AMS Net ID. AMS Net ID's are of the form XXX.XXX.XXX.XXX.1.1, where XXX is a number between 1 and 255.").arg(amsNetId());
        Q_EMIT adsErrorChanged();
        Q_EMIT adsErrorStringChanged();
    }
    if( !m_adsError )
    {
#ifdef USE_TWINCAT
        nErr = AdsSyncReadWriteReqEx2(m_adsPort, pAddr, nIndexGroup, nIndexOffset, nReadLength,
                                      pReadData, nWriteLength, const_cast<void*>(pWriteData), pcbReturn);
#else
        uint32_t returnValue = 0;
        nErr = AdsSyncReadWriteReqEx2(m_adsPort, pAddr, nIndexGroup, nIndexOffset, nReadLength,
                                      pReadData, nWriteLength, pWriteData, &returnValue);
        if (pcbReturn != Q_NULLPTR)
        {
            *pcbReturn = returnValue;
        }
#endif
        if (nErr)
        {
            m_adsError = true;
            m_adsErrorString = tr("QADSObject, Line ");
            m_adsErrorString += QString("%1").arg(__LINE__);
            m_adsErrorString += QString(", nIndexGroup %1, nIndexOffset %2: ").arg(nIndexGroup, 1, 16).arg(nIndexOffset, 1, 16);
            m_adsErrorString += generateADSErrorString(nErr);
            Q_EMIT adsErrorChanged();
            Q_EMIT adsErrorStringChanged();
        }
    }
}

QString QADSObject::adsStateAsString()
{
    QString state = "";
    switch(m_adsState)
    {
    case STATE_INVALID:
        state = "STATE_INVALID";
        break;
    case STATE_IDLE:
        state = "STATE_IDLE";
        break;
    case STATE_RESET:
        state = "STATE_RESET";
        break;
    case STATE_INIT:
        state = "STATE_INIT";
        break;
    case STATE_START:
        state = "STATE_START";
        break;
    case STATE_RUN:
        state = "STATE_RUN";
        break;
    case STATE_STOP:
        state = "STATE_STOP";
        break;
    case STATE_SAVECFG:
        state = "STATE_SAVECFG";
        break;
    case STATE_LOADCFG:
        state = "STATE_LOADCFG";
        break;
    case STATE_POWERFAILURE:
        state = "STATE_POWERFAILURE";
        break;
    case STATE_POWERGOOD:
        state = "STATE_POWERGOOD";
        break;
    case STATE_ERROR:
        state = "STATE_ERROR";
        break;
    case STATE_SHUTDOWN:
        state = "STATE_SHUTDOWN";
        break;
    case STATE_SUSPEND:
        state = "STATE_SUSPEND";
        break;
    case STATE_RESUME:
        state = "STATE_RESUME";
        break;
    case STATE_CONFIG:
        state = "STATE_CONFIG";
        break;
    case STATE_RECONFIG:
        state = "STATE_RECONFIG";
        break;
    case STATE_MAXSTATES:
        state = "STATE_MAXSTATES";
        break;
    default:
        break;
    }
    return state;
}

QString QADSObject::amsEventAsString()
{
    QString event = "";
    switch(m_amsEvent)
    {
    case ROUTERSTOP:
        event = "ROUTERSTOP";
        break;
    case ROUTERSTART:
        event = "ROUTERSTART";
        break;
    case ROUTERREMOVED:
        event = "ROUTERREMOVED";
        break;
    case ROUTERUNKNOWN:
        event = "ROUTERUNKNOWN";
        break;
    default:
        break;
    }
    return event;
}

void QADSObject::setupDLLMemberVariables()
{
#ifdef USE_TWINCAT
    // Get the DLL version, etc.
    long nTemp;
    AdsVersion* pVersion;
    nTemp = AdsGetDllVersion();
    pVersion = (AdsVersion *)&nTemp;
    m_adsDllVersion = pVersion->version;
    m_adsDllRevision = pVersion->revision;
    m_adsDllBuild = pVersion->build;
#endif
}

void QADSObject::setupDeviceMemberVariables()
{
    long nErr;
    AmsAddr Addr;
    AmsAddr *pAddr = &Addr;
    AdsVersion  version;
    AdsVersion* pVersion;
    char pDevName[50];

    Addr.port = m_amsPort;
    if( !stringToAddress(m_amsNetId, Addr.netId.b) )
    {
        m_adsError = true;
        m_adsErrorString = tr("QADSObject, Line ");
        m_adsErrorString += QString("%1").arg(__LINE__);
        m_adsErrorString += ": ";
        m_adsErrorString += tr("Error: %1 is not a valid AMS Net ID. AMS Net ID's are of the form XXX.XXX.XXX.XXX.1.1, where XXX is a number between 1 and 255.").arg(m_amsNetId);
        Q_EMIT adsErrorChanged();
        Q_EMIT adsErrorStringChanged();
    }
    if ( !m_adsError )
    {
        // Get the information for the ADS device to which we are communicating.
        pVersion = &version;
        nErr = AdsSyncReadDeviceInfoReqEx(m_adsPort, pAddr, pDevName, pVersion);
        if( nErr )
        {
            m_adsError = true;
            m_adsErrorString = tr("QADSObject, Line ");
            m_adsErrorString += QString("%1").arg(__LINE__);
            m_adsErrorString += ": ";
            m_adsErrorString += generateADSErrorString(nErr);
            Q_EMIT adsErrorChanged();
            Q_EMIT adsErrorStringChanged();
        }
        else
        {
            m_adsDeviceName = pDevName;
            m_adsDeviceVersion = version.version;
            m_adsDeviceRevision = version.revision;
            m_adsDeviceBuild = version.build;
        }
    }
}

void QADSObject::setupADSState()
{
    long nErr;
    AmsAddr Addr;
    AmsAddr *pAddr = &Addr;
    unsigned short  nAdsState;
    unsigned short  nDeviceState;

    Addr.port = m_amsPort;
    if( !stringToAddress(m_amsNetId, Addr.netId.b) )
    {
        m_adsError = true;
        m_adsErrorString = tr("QADSObject, Line ");
        m_adsErrorString += QString("%1").arg(__LINE__);
        m_adsErrorString += ": ";
        m_adsErrorString += tr("Error: %1 is not a valid AMS Net ID. AMS Net ID's are of the form XXX.XXX.XXX.XXX.1.1, where XXX is a number between 1 and 255.").arg(m_amsNetId);
        Q_EMIT adsErrorChanged();
        Q_EMIT adsErrorStringChanged();
    }
    if ( !m_adsError )
    {
        // Get the status of the PLC.
        nErr = AdsSyncReadStateReqEx(m_adsPort, pAddr, &nAdsState, &nDeviceState);
        if( nErr )
        {
            m_adsError = true;
            m_adsErrorString = tr("QADSObject, Line ");
            m_adsErrorString += QString("%1").arg(__LINE__);
            m_adsErrorString += ": ";
            m_adsErrorString += generateADSErrorString(nErr);
            Q_EMIT adsErrorChanged();
            Q_EMIT adsErrorStringChanged();
        }
        else
        {
            switch (nAdsState)
            {
            case ADSSTATE_INVALID:
                m_adsState = STATE_INVALID;
                break;
            case ADSSTATE_IDLE:
                m_adsState = STATE_IDLE;
                break;
            case ADSSTATE_RESET:
                m_adsState = STATE_RESET;
                break;
            case ADSSTATE_INIT:
                m_adsState = STATE_INIT;
                break;
            case ADSSTATE_START:
                m_adsState = STATE_START;
                break;
            case ADSSTATE_RUN:
                m_adsState = STATE_RUN;
                break;
            case ADSSTATE_STOP:
                m_adsState = STATE_STOP;
                break;
            case ADSSTATE_SAVECFG:
                m_adsState = STATE_SAVECFG;
                break;
            case ADSSTATE_LOADCFG:
                m_adsState = STATE_LOADCFG;
                break;
            case ADSSTATE_POWERFAILURE:
                m_adsState = STATE_POWERFAILURE;
                break;
            case ADSSTATE_POWERGOOD:
                m_adsState = STATE_POWERGOOD;
                break;
            case ADSSTATE_ERROR:
                m_adsState = STATE_ERROR;
                break;
            case ADSSTATE_SHUTDOWN:
                m_adsState = STATE_SHUTDOWN;
                break;
            case ADSSTATE_SUSPEND:
                m_adsState = STATE_SUSPEND;
                break;
            case ADSSTATE_RESUME:
                m_adsState = STATE_RESUME;
                break;
            case ADSSTATE_CONFIG:
                m_adsState = STATE_CONFIG;
                break;
            case ADSSTATE_RECONFIG:
                m_adsState = STATE_RECONFIG;
                break;
            case ADSSTATE_MAXSTATES:
                m_adsState = STATE_MAXSTATES;
                break;
            default:
                break;
            }
        }
    }
}

void QADSObject::setupAMSEventCallback()
{
    long nErr;
    QMutexLocker locker(&m_amsEventMutex);
    Q_UNUSED(locker);
#ifdef USE_TWINCAT
    if ( m_adsObjectAMSEventCallback == Q_NULLPTR )
    {
        // Need to open a non-multi-threaded port for the router notification.
        AdsPortOpen();

        // Register with the AMS router.
        nErr = AdsAmsRegisterRouterNotification(&QADSObjectCallbackClass::AMSRouterCallback);
        if( nErr )
        {
            m_adsError = true;
            m_adsErrorString = tr("QADSObject, Line ");
            m_adsErrorString += QString("%1").arg(__LINE__);
            m_adsErrorString += ": ";
            m_adsErrorString += generateADSErrorString(nErr);
            Q_EMIT adsErrorChanged();
            Q_EMIT adsErrorStringChanged();
        }
    }

    //Create the callback class if not already created.
    if ( (m_adsObjectAMSEventCallback == Q_NULLPTR) && !m_adsError )
    {
        m_adsObjectAMSEventCallback = new QADSObjectCallbackClass;
    }
    if( m_adsObjectAMSEventCallback != Q_NULLPTR )
    {
        //Connect the static hidden signal with my private slots.
        connect(m_adsObjectAMSEventCallback, SIGNAL(notifyAllAMSEvents(int)),
                this, SLOT(privSetAMSEvent(int)), static_cast<Qt::ConnectionType>(Qt::QueuedConnection | Qt::UniqueConnection));
    }
#else
    // Need to create a route to host using the ADSLib.
    // Can't create a route if using "local" AMS net ID.
    if ( (m_adsObjectAMSEventCallback == Q_NULLPTR) &&
         (m_amsNetId != "local") )
    {
        // Add local route to your EtherCAT Master.
        nErr = AdsAddRoute(AmsNetId(m_amsNetId.toStdString()), m_amsHost.toStdString().c_str());
        if( nErr )
        {
            m_adsError = true;
            m_adsErrorString = tr("QADSObject, Line ");
            m_adsErrorString += QString("%1").arg(__LINE__);
            m_adsErrorString += ": Adding ADS route failed, did you specify a valid IP address or hostname?: ";
            m_adsErrorString += generateADSErrorString(nErr);
            Q_EMIT adsErrorChanged();
            Q_EMIT adsErrorStringChanged();
        }
    }

    //Create the callback class if not already created.
    if ( (m_adsObjectAMSEventCallback == Q_NULLPTR) &&
         (m_amsNetId != "local") && !m_adsError )
    {
        m_adsObjectAMSEventCallback = new QADSObjectCallbackClass();
    }
#endif
}

void QADSObject::setupADSStateCallback()
{
    if ( !m_adsError )
    {
        long nErr;
        unsigned long hUser;
        AmsAddr Addr;
        AmsAddr *pAddr = &Addr;
        AdsNotificationAttrib adsNotificationAttrib;

        Addr.port = m_amsPort;
        if( !stringToAddress(m_amsNetId, Addr.netId.b) )
        {
            m_adsError = true;
            m_adsErrorString = tr("QADSObject, Line ");
            m_adsErrorString += QString("%1").arg(__LINE__);
            m_adsErrorString += ": ";
            m_adsErrorString += tr("Error: %1 is not a valid AMS Net ID. AMS Net ID's are of the form XXX.XXX.XXX.XXX.1.1, where XXX is a number between 1 and 255.").arg(m_amsNetId);
            Q_EMIT adsErrorChanged();
            Q_EMIT adsErrorStringChanged();
        }

        if( !m_adsError )
        {
            // Create the callback class pointer if not already
            // created.
            if ( m_adsObjectCallback == Q_NULLPTR )
            {
                m_adsObjectCallback = new QADSObjectCallbackClass();
                QADSObject::QADSObjectCallbackClass::m_pointerVector.append(m_adsObjectCallback);
            }

            // Get pointer index.
            hUser = QADSObject::QADSObjectCallbackClass::m_pointerVector.size()-1;

            // Invoke notification
            adsNotificationAttrib.cbLength       = sizeof(short);
            adsNotificationAttrib.nTransMode     = ADSTRANS_SERVERONCHA;
            adsNotificationAttrib.nMaxDelay      = 0; // jede Aenderung sofort melden
            adsNotificationAttrib.dwChangeFilter = 0; //
#ifdef USE_TWINCAT
            nErr = AdsSyncAddDeviceNotificationReqEx(m_adsPort, pAddr, ADSIGRP_DEVICE_DATA, ADSIOFFS_DEVDATA_ADSSTATE,
                                                     &adsNotificationAttrib, &QADSObjectCallbackClass::ADSStateCallback, hUser,
                                                     &m_adsStateNotificationHandle);
#else
            uint32_t returnValue = 0;
            nErr = AdsSyncAddDeviceNotificationReqEx(m_adsPort, pAddr, ADSIGRP_DEVICE_DATA, ADSIOFFS_DEVDATA_ADSSTATE,
                                                     &adsNotificationAttrib, &QADSObjectCallbackClass::ADSStateCallback, hUser,
                                                     &returnValue);
            m_adsStateNotificationHandle = returnValue;
#endif
            if( nErr )
            {
                m_adsError = true;
                m_adsErrorString = tr("QADSObject, Line ");
                m_adsErrorString += QString("%1").arg(__LINE__);
                m_adsErrorString += ": ";
                m_adsErrorString += generateADSErrorString(nErr);
                Q_EMIT adsErrorChanged();
                Q_EMIT adsErrorStringChanged();
            }
            else
            {
                //Connect the static hidden signal with my private slots.
                connect(m_adsObjectCallback, SIGNAL(notifyAllADSStates(int)),
                        this, SLOT(privSetADSState(int)), static_cast<Qt::ConnectionType>(Qt::QueuedConnection | Qt::UniqueConnection));
            }
        }
    }
}

void QADSObject::setupADSSymbolChangedCallback()
{
    if ( !m_adsError )
    {
        long                    nErr;
        AmsAddr                 Addr;
        AmsAddr                 *pAddr = &Addr;
        unsigned long           hUser;
        AdsNotificationAttrib   adsNotificationAttrib;

        Addr.port = m_amsPort;
        if( !stringToAddress(m_amsNetId, Addr.netId.b) )
        {
            m_adsError = true;
            m_adsErrorString = tr("QADSObject, Line ");
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
            if ( m_adsObjectCallback == Q_NULLPTR )
            {
                m_adsObjectCallback = new QADSObjectCallbackClass();
                QADSObject::QADSObjectCallbackClass::m_pointerVector.append(m_adsObjectCallback);
            }

            // Get pointer index.
            hUser = QADSObject::QADSObjectCallbackClass::m_pointerVector.size()-1;

            // Specify attributes of the notification
            adsNotificationAttrib.cbLength = 1;
            adsNotificationAttrib.nTransMode = ADSTRANS_SERVERONCHA;
            adsNotificationAttrib.nMaxDelay = 0;
            adsNotificationAttrib.nCycleTime = 0;

            // Delete previous request if notification handle is not 0.
            if( m_adsSymbolTableChangedNotificationHandle != 0 )
            {
                nErr = AdsSyncDelDeviceNotificationReqEx(m_adsPort, pAddr,
                                                         m_adsSymbolTableChangedNotificationHandle);
                if (nErr)
                {
                    m_adsError = true;
                    m_adsErrorString = tr("QADSObject, Line ");
                    m_adsErrorString += QString("%1").arg(__LINE__);
                    m_adsErrorString += ": ";
                    m_adsErrorString += generateADSErrorString(nErr);
                    Q_EMIT adsErrorChanged();
                    Q_EMIT adsErrorStringChanged();
                }
            }
            if( !m_adsError )
            {
                // Start notification for changes to the symbol table
#ifdef USE_TWINCAT
                nErr = AdsSyncAddDeviceNotificationReqEx(m_adsPort, pAddr, ADSIGRP_SYM_VERSION, 0, &adsNotificationAttrib,
                                                         &QADSObjectCallbackClass::ADSSymbolTableChangedCallback, hUser,
                                                         &m_adsSymbolTableChangedNotificationHandle);
#else
                uint32_t handle = 0;
                nErr = AdsSyncAddDeviceNotificationReqEx(m_adsPort, pAddr, ADSIGRP_SYM_VERSION, 0, &adsNotificationAttrib,
                                                         &QADSObjectCallbackClass::ADSSymbolTableChangedCallback, hUser,
                                                         &handle);
                m_adsSymbolTableChangedNotificationHandle = handle;
#endif
                if (nErr)
                {
                    m_adsError = true;
                    m_adsErrorString = tr("QADSObject, Line ");
                    m_adsErrorString += QString("%1").arg(__LINE__);
                    m_adsErrorString += ": ";
                    m_adsErrorString += generateADSErrorString(nErr);
                    Q_EMIT adsErrorChanged();
                    Q_EMIT adsErrorStringChanged();
                }
                else
                {
                    // Connect to Qt since there were no errors
                    connect(m_adsObjectCallback, SIGNAL(notifyAllADSSymbolTableChanged()),
                          this, SLOT(privSetADSSymbolTableChanged()), static_cast<Qt::ConnectionType>(Qt::QueuedConnection | Qt::UniqueConnection));
                }
            }
        }
    }
}

void QADSObject::getListOfADSSymbolNames()
{
    m_listOfADSSymbolNames.clear();

    // Get the list of ADS symbol names if there are not errors.
    if( !m_adsError )
    {
        char                  *pchSymbols = Q_NULLPTR;
        quint32               uiIndex;
        AdsSymbolUploadInfo   tAdsSymbolUploadInfo;
        PAdsSymbolEntry       pAdsSymbolEntry;
        unsigned long         returnSize = 0;

        syncReadRequest(m_amsPort, ADSIGRP_SYM_UPLOADINFO, 0, sizeof(tAdsSymbolUploadInfo), &tAdsSymbolUploadInfo, Q_NULLPTR);
        if ( !m_adsError )
        {
            pchSymbols = new char[tAdsSymbolUploadInfo.nSymSize];
            Q_ASSERT(pchSymbols);

            if (pchSymbols)
            {
                // Read information about the PLC variables
                syncReadRequest(m_amsPort, ADSIGRP_SYM_UPLOAD, 0, tAdsSymbolUploadInfo.nSymSize, pchSymbols, &returnSize);

                if ( !m_adsError && (returnSize == tAdsSymbolUploadInfo.nSymSize))
                {
                    // Output information about the PLC variables
                    pAdsSymbolEntry = reinterpret_cast<PAdsSymbolEntry>(pchSymbols);
                    for (uiIndex = 0; uiIndex < tAdsSymbolUploadInfo.nSymbols; uiIndex++)
                    {
                        const char* name = PADSSYMBOLNAME(pAdsSymbolEntry);
                        m_listOfADSSymbolNames << QString(QByteArray(name, pAdsSymbolEntry->nameLength));
                        pAdsSymbolEntry = PADSNEXTSYMBOLENTRY(pAdsSymbolEntry);
                    }
                }
                // Release memory
                delete[] pchSymbols;
            }
        }
    }
}

QString QADSObject::generateADSErrorString(long nErr)
{
    QString error("");
    switch (nErr)
    {
    // Values obtained from:
    // http://infosys.beckhoff.com/content/1033/tcadsdll2/html/ads_returncodes.htm?id=10230
    // Global error codes
    case 0x1:
        error = tr("Global Error 0x1: Internal error");
        break;
    case 0x2:
        error = tr("Global Error 0x2: No Rtime");
        break;
    case 0x3:
        error = tr("Global Error 0x3: Allocation locked memory error");
        break;
    case 0x4:
        error = tr("Global Error 0x4: Insert mailbox error");
        break;
    case 0x5:
        error = tr("Global Error 0x5: Wrong receive HMSG");
        break;
    case 0x6:
        error = tr("Global Error 0x6: target port not found");
        break;
    case 0x7:
        error = tr("Global Error 0x7: target machine not found");
        break;
    case 0x8:
        error = tr("Global Error 0x8: Unknown command ID");
        break;
    case 0x9:
        error = tr("Global Error 0x9: Bad task ID");
        break;
    case 0xA:
        error = tr("Global Error 0xA: No IO");
        break;
    case 0xB:
        error = tr("Global Error 0xB: Unknown AMS command");
        break;
    case 0xC:
        error = tr("Global Error 0xC: Win 32 error");
        break;
    case 0xD:
        error = tr("Global Error 0xD: Port not connected");
        break;
    case 0xE:
        error = tr("Global Error 0xE: Invalid AMS length");
        break;
    case 0xF:
        error = tr("Global Error 0xF: Invalid AMS Net ID");
        break;
    case 0x10:
        error = tr("Global Error 0x10: Low Installation level");
        break;
    case 0x11:
        error = tr("Global Error 0x11: No debug available");
        break;
    case 0x12:
        error = tr("Global Error 0x12: Port disabled");
        break;
    case 0x13:
        error = tr("Global Error 0x13: Port already connected");
        break;
    case 0x14:
        error = tr("Global Error 0x14: AMS Sync Win32 error");
        break;
    case 0x15:
        error = tr("Global Error 0x15: AMS Sync Timeout");
        break;
    case 0x16:
        error = tr("Global Error 0x16: AMS Sync AMS error");
        break;
    case 0x17:
        error = tr("Global Error 0x17: AMS Sync no index map");
        break;
    case 0x18:
        error = tr("Global Error 0x18: Invalid AMS port");
        break;
    case 0x19:
        error = tr("Global Error 0x19: No memory");
        break;
    case 0x1A:
        error = tr("Global Error 0x1A: TCP send error");
        break;
    case 0x1B:
        error = tr("Global Error 0x1B: Host unreachable");
        break;
    //Router Error Codes
    case 0x500:
        error = tr("ROUTERERR_NOLOCKEDMEMORY 0x500: No locked memory can be allocated.");
        break;
    case 0x501:
        error = tr("ROUTERERR_RESIZEMEMORY 0x501: The size of the router memory could not be changed.");
        break;
    case 0x502:
        error = tr("ROUTERERR_MAILBOXFULL 0x502: The Mailbox of the AmsPort has reached the maximum number of possible entries. The current sent message was abolished.");
        break;
    case 0x503:
        error = tr("ROUTERERR_DEBUGBOXFULL 0x503: The Mailbox of the Ams debugger port has reached the maximum number of possible entries. The sent message will not be displayed in the debug monitor.");
        break;
    case 0x504:
        error = tr("ROUTERERR_UNKNOWNPORTTYPE 0x504");
        break;
    case 0x505:
        error = tr("ROUTERERR_NOTINITIALIZED 0x505: Router is not (yet) initialised");
        break;
    case 0x506:
        error = tr("ROUTERERR_PORTALREADYINUSE 0x506: The desired port number is already assigned.");
        break;
    case 0x507:
        error = tr("ROUTERERR_NOTREGISTERED 0x507: Ams Port not registered");
        break;
    case 0x508:
        error = tr("ROUTERERR_NOMOREQUEUES 0x508: The maximum number of Ams Ports reached.");
        break;
    case 0x509:
        error = tr("ROUTERERR_INVALIDPORT 0x509");
        break;
    case 0x50A:
        error = tr("ROUTERERR_NOTACTIVATED 0x50A: TwinCAT Router not (yet) active.");
        break;
    //General ADS Error Codes
    case 0x700:
        error = tr("General ADS Error 0x700: error class <device error>");
        break;
    case 0x701:
        error = tr("General ADS Error 0x701: Service is not supported by server");
        break;
    case 0x702:
        error = tr("General ADS Error 0x702: invalid index group");
        break;
    case 0x703:
        error = tr("General ADS Error 0x703: invalid index offset");
        break;
    case 0x704:
        error = tr("General ADS Error 0x704: reading/writing not permitted");
        break;
    case 0x705:
        error = tr("General ADS Error 0x705: parameter size not correct");
        break;
    case 0x706:
        error = tr("General ADS Error 0x706: invalid parameter value(s)");
        break;
    case 0x707:
        error = tr("General ADS Error 0x707: device is not in a ready state");
        break;
    case 0x708:
        error = tr("General ADS Error 0x708: device is busy");
        break;
    case 0x709:
        error = tr("General ADS Error 0x709: invalid context (must be in Windows)");
        break;
    case 0x70A:
        error = tr("General ADS Error 0x70A: out of memory");
        break;
    case 0x70B:
        error = tr("General ADS Error 0x70B: invalid parameter value(s)");
        break;
    case 0x70C:
        error = tr("General ADS Error 0x70C: not found (files, ...)");
        break;
    case 0x70D:
        error = tr("General ADS Error 0x70D: syntax error in command or file");
        break;
    case 0x70E:
        error = tr("General ADS Error 0x70E: objects do not match");
        break;
    case 0x70F:
        error = tr("General ADS Error 0x70F: object already exists");
        break;
    case 0x710:
        error = tr("General ADS Error 0x710: symbol not found");
        break;
    case 0x711:
        error = tr("General ADS Error 0x711: symbol version invalid");
        break;
    case 0x712:
        error = tr("General ADS Error 0x712: server is in invalid state");
        break;
    case 0x713:
        error = tr("General ADS Error 0x713: AdsTransMode not supported");
        break;
    case 0x714:
        error = tr("General ADS Error 0x714: Notification handle is invalid");
        break;
    case 0x715:
        error = tr("General ADS Error 0x715: Notification client not registered");
        break;
    case 0x716:
        error = tr("General ADS Error 0x716: no more notification handles");
        break;
    case 0x717:
        error = tr("General ADS Error 0x717: size for watch too big");
        break;
    case 0x718:
        error = tr("General ADS Error 0x718: device not initialized");
        break;
    case 0x719:
        error = tr("General ADS Error 0x719: device has a timeout");
        break;
    case 0x71A:
        error = tr("General ADS Error 0x71A: query interface failed");
        break;
    case 0x71B:
        error = tr("General ADS Error 0x71B: wrong interface required");
        break;
    case 0x71C:
        error = tr("General ADS Error 0x71C: class ID is invalid");
        break;
    case 0x71D:
        error = tr("General ADS Error 0x71D: object ID is invalid");
        break;
    case 0x71E:
        error = tr("General ADS Error 0x71E: request is pending");
        break;
    case 0x71F:
        error = tr("General ADS Error 0x71F: request is aborted");
        break;
    case 0x720:
        error = tr("General ADS Error 0x720: signal warning");
        break;
    case 0x721:
        error = tr("General ADS Error 0x721: invalid array index");
        break;
    case 0x722:
        error = tr("General ADS Error 0x722: symbol not active -> release handle and try again");
        break;
    case 0x723:
        error = tr("General ADS Error 0x700: access denied");
        break;
    case 0x740:
        error = tr("General ADS Error 0x740: Error class <client error>");
        break;
    case 0x741:
        error = tr("General ADS Error 0x741: invalid parameter at service");
        break;
    case 0x742:
        error = tr("General ADS Error 0x742: polling list is empty");
        break;
    case 0x743:
        error = tr("General ADS Error 0x743: var connection already in use");
        break;
    case 0x744:
        error = tr("General ADS Error 0x744: invoke ID in use");
        break;
    case 0x745:
        error = tr("General ADS Error 0x745: timeout elapsed");
        break;
    case 0x746:
        error = tr("General ADS Error 0x746: error in win32 subsystem");
        break;
    case 0x747:
        error = tr("General ADS Error 0x747: Invalid client timeout value");
        break;
    case 0x748:
        error = tr("General ADS Error 0x748: ads-port not opened");
        break;
    case 0x750:
        error = tr("General ADS Error 0x750: internal error in ads sync");
        break;
    case 0x751:
        error = tr("General ADS Error 0x751: hash table overflow");
        break;
    case 0x752:
        error = tr("General ADS Error 0x752: key not found in hash");
        break;
    case 0x753:
        error = tr("General ADS Error 0x753: no more symbols in cache");
        break;
    case 0x754:
        error = tr("General ADS Error 0x754: invalid response received");
        break;
    case 0x755:
        error = tr("General ADS Error 0x755: sync port is locked");
        break;
    // RTime Error Codes
    case 0x1000:
        error = tr("RTERR_INTERNAL 0x1000: Internal fatal error in the TwinCAT real-time system");
        break;
    case 0x1001:
        error = tr("RTERR_BADTIMERPERIODS 0x1001: Timer value not vaild");
        break;
    case 0x1002:
        error = tr("RTERR_INVALIDTASKPTR 0x1002: Task pointer has the invalid value ZERO");
        break;
    case 0x1003:
        error = tr("RTERR_INVALIDSTACKPTR 0x1003: Task stack pointer has the invalid value ZERO.");
        break;
    case 0x1004:
        error = tr("RTERR_PRIOEXISTS 0x1004: The demand task priority is already assigned.");
        break;
    case 0x1005:
        error = tr("RTERR_NOMORETCB 0x1005: No more free TCB (Task Control Block) available. Maximum number of TCBs is 64.");
        break;
    case 0x1006:
        error = tr("RTERR_NOMORESEMAS 0x1006: No more free semaphores available. Maximum number of semaphores is 64.");
        break;
    case 0x1007:
        error = tr("RTERR_NOMOREQUEUES 0x1007: No more free queue available. Maximum number of queue is 64.");
        break;
    case 0x1008:
        error = tr("0x1008: TwinCAT reserved.");
        break;
    case 0x1009:
        error = tr("0x1009: TwinCAT reserved.");
        break;
    case 0x100A:
        error = tr("0x100A: TwinCAT reserved.");
        break;
    case 0x100B:
        error = tr("0x100B: TwinCAT reserved.");
        break;
    case 0x100C:
        error = tr("0x100C: TwinCAT reserved.");
        break;
    case 0x100D:
        error = tr("RTERR_EXTIRQALREADYDEF 0x100D: An external synchronisation interrupt is already applied.");
        break;
    case 0x100E:
        error = tr("RTERR_EXTIRQNOTDEF 0x100E: No external synchronsiation interrupt applied.");
        break;
    case 0x100F:
        error = tr("RTERR_EXTIRQINSTALLFAILED 0x100F: The apply of the external synchronisation interrupt failed.");
        break;
    case 0x1010:
        error = tr("RTERR_IRQLNOTLESSOREQUAL 0x1010: Call of a service function in the wrong context.");
        break;
    // Winsock Error Codes
    case 0x274C:
        error = tr("Winsock Error 0x274C: A socket operation was attempted to an unreachable host");
        break;
    case 0x274D:
        error = tr("Winsock Error 0x274D: A connection attempt failed because the connected party did not properly respond after a period of time, or established connection failed because connected host has failed to respond.");
        break;
    case 0x2751:
        error = tr("Winsock Error 0x2751: No connection could be made because the target machine actively refused it");
        break;
    default:
        error = tr("Unknown Error %1").arg(nErr,0,16);
        break;
    }
    return error;
}

QString QADSObject::addressToString(const unsigned char addr[6])
{
    QString address = "";
    for(int i=0; i <= 4; i++)
    {
        address += QString::number((uint)addr[i]);
        address += '.';
    }
    address += QString::number((uint)addr[5]);
    return address;
}

bool QADSObject::stringToAddress(const QString &strAddr, unsigned char addr[6])
{
    QStringList address = strAddr.split(".");
    if( address.count() != 6 )
    {
        return false;
    }
    for(int i=0; i<6; i++)
    {
        unsigned int temp = address.at(i).toUInt();
        if( (temp > 255) || (temp == 0) )
        {
            return false;
        }
        addr[i] = (unsigned char)temp;
    }
    return true;
}

void QADSObject::initializeADSConnection()
{
    resetADS();
    setupADS();
}

void QADSObject::setADSState(ADSState state)
{
    // Tell TwinCAT to change it's state
    if( !m_adsError && (state != m_adsState) )
    {
        long nErr;
        AmsAddr Addr;
        AmsAddr *pAddr = &Addr;
        unsigned short  nAdsState = ADSSTATE_INVALID;
        unsigned short  nDeviceState = 0;
        void *pData = Q_NULLPTR;

        Addr.port = m_amsPort;
        if( !stringToAddress(m_amsNetId, Addr.netId.b) )
        {
            m_adsError = true;
            m_adsErrorString = tr("QADSObject, Line ");
            m_adsErrorString += QString("%1").arg(__LINE__);
            m_adsErrorString += ": ";
            m_adsErrorString += tr("Error: %1 is not a valid AMS Net ID. AMS Net ID's are of the form XXX.XXX.XXX.XXX.1.1, where XXX is a number between 1 and 255.").arg(m_amsNetId);
            Q_EMIT adsErrorChanged();
            Q_EMIT adsErrorStringChanged();
        }

        switch (state)
        {
        case STATE_IDLE:
            nAdsState = ADSSTATE_IDLE;
            break;
        case STATE_RESET:
            nAdsState = ADSSTATE_RESET;
            break;
        case STATE_INIT:
            nAdsState = ADSSTATE_INIT;
            break;
        case STATE_START:
            nAdsState = ADSSTATE_START;
            break;
        case STATE_RUN:
            nAdsState = ADSSTATE_RUN;
            break;
        case STATE_STOP:
            nAdsState = ADSSTATE_STOP;
            break;
        case STATE_SAVECFG:
            nAdsState = ADSSTATE_SAVECFG;
            break;
        case STATE_LOADCFG:
            nAdsState = ADSSTATE_LOADCFG;
            break;
        case STATE_SHUTDOWN:
            nAdsState = ADSSTATE_SHUTDOWN;
            break;
        case STATE_SUSPEND:
            nAdsState = ADSSTATE_SUSPEND;
            break;
        case STATE_RESUME:
            nAdsState = ADSSTATE_RESUME;
            break;
        case STATE_CONFIG:
            nAdsState = ADSSTATE_CONFIG;
            break;
        case STATE_RECONFIG:
            nAdsState = ADSSTATE_RECONFIG;
            break;
        default:
            break;
        }
        nErr = AdsSyncWriteControlReqEx(m_adsPort, pAddr, nAdsState, nDeviceState, 0, pData);
        if( nErr )
        {
            m_adsError = true;
            m_adsErrorString = tr("QADSObject, Line ");
            m_adsErrorString += QString("%1").arg(__LINE__);
            m_adsErrorString += ": ";
            m_adsErrorString += generateADSErrorString(nErr);
            Q_EMIT adsErrorChanged();
            Q_EMIT adsErrorStringChanged();
        }
    }
}

void QADSObject::setAMSPort(quint16 nPort)
{
    if (nPort != m_amsPort)
    {
        m_amsPort = nPort;
        Q_EMIT amsPortChanged();

    }
}

void QADSObject::setAMSNetId(const QString &amsnetid)
{
    if (amsnetid != m_amsNetId)
    {
        m_amsNetId = amsnetid;
        Q_EMIT amsNetIdChanged();
    }
}

void QADSObject::setAMSHost(const QString &amshost)
{
    if (amshost != m_amsHost)
    {
        m_amsHost = amshost;
        Q_EMIT amsHostChanged();
    }
}

void QADSObject::privSetADSState(int state)
{
    //std::cout << "privSetADSState: " << state << std::endl;
    // Set my own state.
    if( state != m_adsState )
    {
        //std::cout << "privSetADSState: Got here" << std::endl;
        m_adsState = static_cast<ADSState>(state);
        Q_EMIT adsStateChanged();
    }
}

void QADSObject::privSetAMSEvent(int event)
{
    // Set my own event.
    if( event != m_amsEvent )
    {
        switch(event)
        {
        case ROUTERSTOP:
        case ROUTERREMOVED:
            m_amsEventMutex.lock();
            resetAMS();
            m_amsEventMutex.unlock();
            resetADS();
            break;
        case ROUTERSTART:
            setupADS();
            break;
        default:
            break;
        }
        m_amsEvent = static_cast<AMSRouterEvent>(event);
        Q_EMIT amsEventChanged();
    }
}

void QADSObject::privSetADSSymbolTableChanged()
{
    getListOfADSSymbolNames();
    Q_EMIT listOfADSSymbolNamesChanged();
}
