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
#ifndef QADSOBJECT_H
#define QADSOBJECT_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QMutex>
#include <QMetaType>
#include "qadsglobal.h"

/*!
  \brief Base QtADS object class.
  \details Provides methods for communicating with TwinCAT (via ADS),
  reporting errors, and monitoring the state of TwinCAT.
*/

class QTADS_EXPORT QADSObject : public QObject
{
    Q_OBJECT
    Q_PROPERTY(long adsPort READ adsPort CONSTANT)
    Q_PROPERTY(quint16 amsPort READ amsPort WRITE setAMSPort NOTIFY amsPortChanged)
    Q_PROPERTY(QString amsNetId READ amsNetId WRITE setAMSNetId NOTIFY amsNetIdChanged)
    Q_PROPERTY(QString amsHost READ amsHost WRITE setAMSHost NOTIFY amsHostChanged)
    Q_PROPERTY(bool adsError READ adsError NOTIFY adsErrorChanged)
    Q_PROPERTY(QString adsErrorString READ adsErrorString NOTIFY adsErrorStringChanged)
    Q_PROPERTY(unsigned char adsDllVersion READ adsDllVersion)
    Q_PROPERTY(unsigned char adsDllRevision READ adsDllRevision)
    Q_PROPERTY(unsigned short adsDllBuild READ adsDllBuild)
    Q_PROPERTY(QString adsDeviceName READ adsDeviceName)
    Q_PROPERTY(unsigned char adsDeviceVersion READ adsDeviceVersion)
    Q_PROPERTY(unsigned char adsDeviceRevision READ adsDeviceRevision)
    Q_PROPERTY(unsigned short adsDeviceBuild READ adsDeviceBuild)
    Q_PROPERTY(ADSState adsState READ adsState WRITE setADSState NOTIFY adsStateChanged)
    Q_ENUMS(ADSState)
    Q_PROPERTY(AMSRouterEvent amsEvent READ amsEvent NOTIFY amsEventChanged)
    Q_ENUMS(AMSRouterEvent)
    Q_PROPERTY(QStringList listOfADSSymbolNames READ listOfADSSymbolNames NOTIFY listOfADSSymbolNamesChanged)
public:
    /*!
     \verbatim
     Default constructor.
     Parameters
      amsport
          The AMS port defaults to the TwinCAT3 port 851.
          If using TwinCAT2 set the port to 801 or one of the other PLC ports if used (811, 821, or 831).
      amsnetid
          AMS net ID of TwinCAT device. Can be "local" for the local-host device or the AMS net ID of the form xxx.xxx.xxx.xxx.1.1 for a remote device.
      amshost
          Hostname or IP address of the ADS target system. Required for ADSLib only in order to create a route associated with the AMS net ID.
      \endverbatim
    */
    explicit QADSObject(QObject *parent = Q_NULLPTR, quint16 amsport = 851, const QString &amsnetid=QString("local"), const QString &amshost=QString("localhost"));

    ~QADSObject();

    enum ADSState { STATE_INVALID, STATE_IDLE, STATE_RESET, STATE_INIT, STATE_START, STATE_RUN, STATE_STOP,
                           STATE_SAVECFG, STATE_LOADCFG, STATE_POWERFAILURE, STATE_POWERGOOD, STATE_ERROR,
                           STATE_SHUTDOWN, STATE_SUSPEND, STATE_RESUME, STATE_CONFIG, STATE_RECONFIG,
                           STATE_MAXSTATES
                         };


    enum AMSRouterEvent { ROUTERSTOP, ROUTERSTART, ROUTERREMOVED, ROUTERUNKNOWN };

    /*!
    \verbatim
    Writes data synchronously to an ADS device.
    Parameters
     nPort
          [in] ADS port number (TwinCAT2 PLC = 801, TwinCAT3 PLC = 851).
     nIndexGroup
          [in] Index Group.
     nIndexOffset
          [in] Index Offset.
     nLength
          [in] Length of the data, in bytes, written to the ADS server.
     pData
          [in] Pointer to the data written to the ADS server.
    \endverbatim
    */
    void syncWriteRequest(unsigned short nPort, unsigned long nIndexGroup, unsigned long nIndexOffset, unsigned long nLength, const void* pData);

    /*!
     \verbatim
     Reads data synchronously from an ADS server.
     Parameters
      nPort
          [in] ADS port number (TwinCAT2 PLC = 801, 811, 821, 831, TwinCAT3 PLC = 851).
      nIndexGroup
          [in] Index Group.
      nIndexOffset
          [in] Index Offset.
      nLength
          [in] Length of the data in bytes.
      pData
          [out] Pointer to a data buffer that will receive the data.
      pcbReturn
          [out] pointer to a variable. If successful, this variable will return the number of actually read data bytes.
      \endverbatim
    */
    void syncReadRequest(unsigned short nPort, unsigned long nIndexGroup, unsigned long nIndexOffset, unsigned long nLength, void* pData,
                         unsigned long* pcbReturn);

    /*!
     \verbatim
     Writes data synchronously into an ADS server and receives data back from the ADS device.
     Parameters
      nPort
          [in] ADS port number (TwinCAT2 PLC = 801, 811, 821, 831, TwinCAT3 PLC = 851).
      nIndexGroup
          [in] Index Group.
      nIndexOffset
          [in] Index Offset.
      nReadLength
          [in] Length of the data, in bytes, returned by the ADS device.
      pReadData
          [out] Buffer with data returned by the ADS device.
      nWriteLength
          [in] Length of the data, in bytes, written to the ADS device.
      pWriteData
          [out] Buffer with data written to the ADS device.
      pcbReturn
          [out] pointer to a variable. If successful, this variable will return the number of actually read data bytes.
      \endverbatim
    */
    void syncReadWriteRequest(unsigned short nPort, unsigned long nIndexGroup, unsigned long nIndexOffset, unsigned long nReadLength, void* pReadData,
                              unsigned long nWriteLength, const void* pWriteData, unsigned long* pcbReturn);

    /*!
     Returns the ADS port number for all ADS client/server transactions.
    */
    long adsPort() const
    { return m_adsPort; }

    /*!
     Returns the AMS port number.
    */
    quint16 amsPort() const
    { return m_amsPort; }

    /*!
     Returns the AMS Net ID of the ADS server.
    */
    const QString & amsNetId() const
    { return m_amsNetId; }

    /*!
     Returns the AMS hostname or IP address.
    */
    const QString & amsHost() const
    { return m_amsHost; }

    /*!
     Returns true when an ADS error has occurred or false otherwise.
    */
    bool adsError() const
    { return m_adsError; }

    /*!
     Returns the ADS error message when an error has occurred.
    */
    const QString & adsErrorString() const
    { return m_adsErrorString; }

    /*!
     Returns the version number of the TwinCAT ADS Dll.
     When using AdsLib this value will always be 0.
    */
    unsigned char adsDllVersion() const
    { return m_adsDllVersion; }

    /*!
     Returns the revision number of the TwinCAT ADS Dll.
     When using AdsLib this value will always be 0.
    */
    unsigned char adsDllRevision() const
    { return m_adsDllRevision; }

    /*!
     Returns the build number of the TwinCAT ADS Dll.
     When using AdsLib this value will always be 0.
    */
    unsigned short adsDllBuild() const
    { return m_adsDllBuild; }

    /*!
     Returns the device name of the ADS server.
    */
    const QString & adsDeviceName() const
    { return m_adsDeviceName; }

    /*!
     Returns the version number of the ADS server.
    */
    unsigned char adsDeviceVersion() const
    { return m_adsDeviceVersion; }

    /*!
     Returns the device revision of the ADS server.
    */
    unsigned char adsDeviceRevision() const
    { return m_adsDeviceRevision; }

    /*!
     Returns the build number of the ADS server.
    */
    unsigned short adsDeviceBuild() const
    { return m_adsDeviceBuild; }

    /*!
     Returns the state of the PLC for the ADS server.
     \see ADSState
    */
    ADSState adsState() const
    { return m_adsState; }

    /*!
     Returns the state of the PLC for the ADS server as a string.
     \see TwinCATADSState
    */
    QString adsStateAsString();

    /*!
     Returns the AMS router event.
     \see AMSRouterEvent
    */
    AMSRouterEvent amsEvent() const
    { return m_amsEvent; }

    /*!
     Returns the AMS router event as a string.
     \see AMSRouterEvent
    */
    Q_INVOKABLE QString amsEventAsString();

    /*!
     Returns QStringList of ADS symbol names.
    */
    QStringList listOfADSSymbolNames() const
    { return m_listOfADSSymbolNames; }

Q_SIGNALS:
    /*!
     Signal emitted when the PLC state of the ADS server changes.
    */
    void adsStateChanged();

    /*!
     Signal emitted when an AMS router event occurs.
    */
    void amsEventChanged();

    /*!
     Signal emitted when the AMS port number changes.
    */
    void amsPortChanged();

    /*!
     Signal emitted when the AMS Net ID changes.
    */
    void amsNetIdChanged();

    /*!
     Signal emitted when the AMS hostname or IP address changes.
    */
    void amsHostChanged();

    /*!
     Signal emitted when an ADS error occurs.
    */
    void adsErrorChanged();

    /*!
     Signal emitted when the ADS error string changes.
    */
    void adsErrorStringChanged();

    /*!
     Signal emitted when the ADS symbol table changes.
    */
    void listOfADSSymbolNamesChanged();

public Q_SLOTS:
    /*!
     Public slot to initialize the ADS connection. This function
     may be used to manually initialize the ADS connection after
     changing parameters.
    */
    virtual void initializeADSConnection();

    /*!
     Set/change the PLC state of the ADS server.
    */
    virtual void setADSState(ADSState state);

    /*!
     \verbatim
     Sets the AMS port number. Must call initializeADSConnection after calling this function.
     Parameters
      nPort
          [in] AMS port number (TwinCAT2 PLC = 801, 811, 821, 831, TwinCAT3 PLC = 851).
      \endverbatim
    */
    virtual void setAMSPort(quint16 nPort);

    /*!
     \verbatim
     Sets the AMS Net ID. Must call initializeADSConnection after calling this function.
     Parameters
      amsnetid
          [in] AMS net ID of TwinCAT device. Can be "local" for the local-host device or the AMS net ID of the form xxx.xxx.xxx.xxx.1.1 for a remote device.
      \endverbatim
    */
    virtual void setAMSNetId(const QString &amsnetid);

    /*!
     \verbatim
     Sets the AMS hostname or IP address. Must call initializeADSConnection after calling this function.
     Parameters
      amshost
          [in] AMS hostname or IP address of target TwinCAT device. Required for ADSLib only in order to create a route associated with the AMS net ID.
      \endverbatim
    */
    virtual void setAMSHost(const QString &amshost);

private Q_SLOTS:
    void privSetADSState(int state);

    void privSetAMSEvent(int event);

    void privSetADSSymbolTableChanged();

protected:
    QString generateADSErrorString(long nErr);

    QString addressToString(const unsigned char addr[6]);

    bool stringToAddress(const QString &strAddr, unsigned char addr[6]);

    bool m_adsError;
    QString m_adsErrorString;

private:
    void setupADS();
    void resetADS();
    void resetAMS();
    void setupDLLMemberVariables();
    void setupDeviceMemberVariables();
    void setupADSState();
    void setupAMSEventCallback();
    void setupADSStateCallback();
    void setupADSSymbolChangedCallback();
    void getListOfADSSymbolNames();
    long m_adsPort;
    quint16 m_amsPort;
    QString m_amsNetId;
    QString m_amsHost;
    unsigned char m_adsDllVersion;
    unsigned char m_adsDllRevision;
    unsigned short m_adsDllBuild;
    QString m_adsDeviceName;
    unsigned char m_adsDeviceVersion;
    unsigned char m_adsDeviceRevision;
    unsigned short m_adsDeviceBuild;
    ADSState m_adsState;
    AMSRouterEvent m_amsEvent;
    QStringList m_listOfADSSymbolNames;
    class QADSObjectCallbackClass;
    QADSObjectCallbackClass *m_adsObjectCallback;
    static QADSObjectCallbackClass *m_adsObjectAMSEventCallback;
    unsigned long m_adsStateNotificationHandle;
    unsigned long m_adsSymbolTableChangedNotificationHandle;
    static int m_instanceCount;
    static QMutex m_amsEventMutex;
    static int m_adsStateMetaTypeId;
    static int m_amsRouterEventMetaTypeId;
};

// Need to declare new meta-types.
Q_DECLARE_METATYPE(QADSObject::ADSState)
Q_DECLARE_METATYPE(QADSObject::AMSRouterEvent)

class QADSObjectCallbackClassTemplate : public QObject
{
    Q_OBJECT
public:
    void setADSState(int state);
    void setAMSEvent(int event);
    void setADSSymbolTableChanged();

Q_SIGNALS:
    void notifyAllADSStates(int);
    void notifyAllAMSEvents(int);
    void notifyAllADSSymbolTableChanged();
};

#endif // QADSOBJECT_H
