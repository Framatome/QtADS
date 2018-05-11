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
#ifndef QADSPLCVARIABLE_H
#define QADSPLCVARIABLE_H

#include "qadsobject.h"

/*!
  \brief Base class for all PLC variables.
*/

class QTADS_EXPORT QADSPLCVariable : public QADSObject
{
    Q_OBJECT
    Q_PROPERTY(QString plcVariableName READ plcVariableName WRITE setPLCVariableName NOTIFY plcVariableNameChanged)
    Q_PROPERTY(PLCVariableReadOperation plcVariableReadOperation READ plcVariableReadOperation WRITE setPLCVariableReadOperation NOTIFY plcVariableReadOperationChanged)
    Q_PROPERTY(unsigned long plcVariableCycleTime READ plcVariableCycleTime WRITE setPLCVariableCycleTime NOTIFY plcVariableCycleTimeChanged)
    Q_PROPERTY(QString adsSymbolName READ adsSymbolName NOTIFY adsSymbolNameChanged)
    Q_PROPERTY(unsigned long adsSymbolGroup READ adsSymbolGroup NOTIFY adsSymbolGroupChanged)
    Q_PROPERTY(unsigned long adsSymbolOffset READ adsSymbolOffset NOTIFY adsSymbolOffsetChanged)
    Q_PROPERTY(unsigned long adsSymbolSize READ adsSymbolSize NOTIFY adsSymbolSizeChanged)
    Q_PROPERTY(QString adsSymbolType READ adsSymbolType NOTIFY adsSymbolTypeChanged)
    Q_PROPERTY(QString adsSymbolComment READ adsSymbolComment NOTIFY adsSymbolCommentChanged)
    Q_PROPERTY(unsigned long adsSymbolHandle READ adsSymbolHandle NOTIFY adsSymbolHandleChanged)
    Q_ENUMS(PLCVariableReadOperation)
public:
    enum PLCVariableReadOperation {ON_DEMAND, CYCLICAL, SYNCHRONIZED};

    /*!
     \verbatim
     Default constructor.
     Parameters
      amsport
          The AMS port defaults to the TwinCAT3 port 851.
          If using TwinCAT2 set the port to 801 or one of the other PLC ports if used (811, 821, or 831).
      amsnetid
          AMS net ID of TwinCAT device. Can be "local" for the local-host device or the AMS net ID of the form xxx.xxx.xxx.xxx.1.1 for a remote device.
      var
          Name of PLC variable. For globals, prefix the name with '.' when using TwinCAT2. When using TwinCAT3 use the global variable list name as the prefix.
      readop
          Type of read operation:
          ON_DEMAND = read only when value() method is called (inherited classes).
          CYCLICAL = valueChanged() signal is emitted every "cycletime" (inherited classes).
          SYNCHRONIZED = valueChanged() signal is emitted everytime the value of the PLC variable changes (inherited classes).
      cycletime
          Cycle time for CYCLICAL operation.
          The read cycle time unit is 100ns as defined by TwinCAT (http://infosys.beckhoff.com/content/1033/tcadsdll2/html/TcAdsDll_StrucAdsNotificationAttrib.htm).
      \endverbatim
    */
    explicit QADSPLCVariable(QObject *parent = Q_NULLPTR, quint16 amsport = 851, const QString &amsnetid=QString("local"), const QString &amshost=QString("localhost"),
                             const QString &var=QString(""), PLCVariableReadOperation readop=ON_DEMAND, unsigned long cycletime=0);

    ~QADSPLCVariable();

    /*!
     Return the name of the PLC variable set via the constructor.
    */
    const QString & plcVariableName() const
    { return m_plcVariableName; }

    /*!
     Return the type of read operation.
    */
    PLCVariableReadOperation plcVariableReadOperation() const
    { return m_plcVariableReadOperation; }

    /*!
     Return the cycle time for the CYCLICAL operation.
    */
    unsigned long plcVariableCycleTime() const
    { return m_plcVariableCycleTime; }

    /*!
     Return the ADS symbol name for this PLC variable.
    */
    const QString & adsSymbolName() const
    { return m_adsSymbolName; }

    /*!
     Return the ADS symbol group for this PLC variable.
    */
    unsigned long adsSymbolGroup() const
    { return m_adsSymbolGroup; }

    /*!
     Return the ADS symbol offset for this PLC variable.
    */
    unsigned long adsSymbolOffset() const
    { return m_adsSymbolOffset; }

    /*!
     Return the ADS symbol size in bytes for this PLC variable.
    */
    unsigned long adsSymbolSize() const
    { return m_adsSymbolSize; }

    /*!
     Return the ADS symbol type for this PLC variable.
    */
    const QString & adsSymbolType() const
    { return m_adsSymbolType; }

    /*!
     Return the ADS symbol comment for this PLC variable.
    */
    const QString & adsSymbolComment() const
    { return m_adsSymbolComment; }

    /*!
     Return the ADS symbol handle to the PLC variable.
    */
    unsigned long adsSymbolHandle() const
    { return m_adsSymbolHandle; }

    virtual void initializeADSConnection() Q_DECL_OVERRIDE;

public Q_SLOTS:
    /*!
     Set the read operation. Must call initializeADSConnection after calling this function.
    */
    virtual void setPLCVariableReadOperation(PLCVariableReadOperation readop);

    /*!
     Set the cycle time. Must call initializeADSConnection after calling this function.
    */
    virtual void setPLCVariableCycleTime(unsigned long cycletime);

    /*!
     Set the name of the PLC variable. Must call initializeADSConnection after calling this function.
    */
    virtual void setPLCVariableName(const QString &var);

Q_SIGNALS:
    /*!
     Signal emitted when the PLC variable name changes.
    */
    void plcVariableNameChanged();

    /*!
     Signal emitted when the PLC read operation is changed.
    */
    void plcVariableReadOperationChanged();

    /*!
     Signal emitted when the PLC read cycle time is changed.
    */
    void plcVariableCycleTimeChanged();

    /*!
     Signal emitted when the ADS symbol name has changed.
    */
    void adsSymbolNameChanged();

    /*!
     Signal emitted when the ADS symbol group has changed.
    */
    void adsSymbolGroupChanged();

    /*!
     Signal emitted when the ADS symbol offset has changed.
    */
    void adsSymbolOffsetChanged();

    /*!
     Signal emitted when the ADS symbol size has changed.
    */
    void adsSymbolSizeChanged();

    /*!
     Signal emitted when the ADS symbol type has changed.
    */
    void adsSymbolTypeChanged();

    /*!
     Signal emitted when the ADS symbol comment has changed.
    */
    void adsSymbolCommentChanged();

    /*!
     Signal emitted when the ADS symbol handle has changed.
    */
    void adsSymbolHandleChanged();

protected Q_SLOTS:
    virtual void protCatchADSStateChange();

    virtual void protCatchADSSymbolTableChanged();

private:
    void getPLCVariableInformation();

    void releaseHandle();

    QString m_plcVariableName;
    PLCVariableReadOperation m_plcVariableReadOperation;
    unsigned long m_plcVariableCycleTime;
    QString m_adsSymbolName;
    unsigned long m_adsSymbolGroup;
    unsigned long m_adsSymbolOffset;
    unsigned long m_adsSymbolSize;
    QString m_adsSymbolType;
    QString m_adsSymbolComment;
    unsigned long m_adsSymbolHandle;
    static int m_plcVariableReadOperationMetaTypeId;
};

// Need to declare new meta-types.
Q_DECLARE_METATYPE(QADSPLCVariable::PLCVariableReadOperation)

#endif // QADSPLCVARIABLE_H
