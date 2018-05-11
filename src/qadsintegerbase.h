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
#ifndef QADSINTEGERBASE_H
#define QADSINTEGERBASE_H

#include "qadsplcvariable.h"

/*!
  \brief Base class for 8 bit unsigned integer PLC variables.
*/

/************************************************************************************************/
/******************************** QADSUnsignedInteger8 **************************************/
/************************************************************************************************/
class QTADS_EXPORT QADSUnsignedInteger8 : public QADSPLCVariable
{
    Q_OBJECT
    Q_PROPERTY(quint8 value READ value WRITE setValue NOTIFY valueChanged)
public:
    // The read cycle time unit is 100ns as defined by TwinCAT (http://infosys.beckhoff.com/content/1033/tcadsdll2/html/TcAdsDll_StrucAdsNotificationAttrib.htm).
    explicit QADSUnsignedInteger8(QObject *parent = Q_NULLPTR, quint16 amsport = 851, const QString &amsnetid=QString("local"), const QString &amshost=QString("localhost"),
                                  const QString &var=QString(""), PLCVariableReadOperation readop=ON_DEMAND, unsigned long cycletime=0);

    ~QADSUnsignedInteger8();

    static quint8 maxValue()
    { return 255; }

    static quint8 minValue()
    { return 0; }

    virtual quint8 value();

    virtual void initializeADSConnection() Q_DECL_OVERRIDE;

public Q_SLOTS:
    virtual void setValue(quint8 value);

private Q_SLOTS:
    void privSetValue(quint8 value);

Q_SIGNALS:
    void valueChanged();

protected:
    void setupVariableCallback();

    void resetVariableCallback();

    virtual void parseVariableType() = 0;

    void readValue();

private:
    quint8 m_value;
    class QADSUnsignedInteger8CallbackClass;
    QADSUnsignedInteger8CallbackClass *m_adsUnsignedInteger8Callback;
    unsigned long m_variableCallBackNotificationHandle;
};

class QTADS_EXPORT QADSUnsignedInteger8CallbackClassTemplate : public QObject
{
    Q_OBJECT
public:
    void setValue(quint8 value);

Q_SIGNALS:
    void valueChanged(quint8);
};


/*!
  \brief Base class for 8 bit signed integer PLC variables.
*/

/************************************************************************************************/
/******************************** QADSInteger8 **********************************************/
/************************************************************************************************/
class QTADS_EXPORT QADSInteger8 : public QADSPLCVariable
{
    Q_OBJECT
    Q_PROPERTY(qint8 value READ value WRITE setValue NOTIFY valueChanged)
public:
    // The read cycle time unit is 100ns as defined by TwinCAT (http://infosys.beckhoff.com/content/1033/tcadsdll2/html/TcAdsDll_StrucAdsNotificationAttrib.htm).
    explicit QADSInteger8(QObject *parent = Q_NULLPTR, quint16 amsport = 851, const QString &amsnetid=QString("local"), const QString &amshost=QString("localhost"),
                          const QString &var=QString(""), PLCVariableReadOperation readop=ON_DEMAND, unsigned long cycletime=0);

    ~QADSInteger8();

    static qint8 maxValue()
    { return 127; }

    static qint8 minValue()
    { return -128; }

    virtual qint8 value();

    virtual void initializeADSConnection() Q_DECL_OVERRIDE;

public Q_SLOTS:
    virtual void setValue(qint8 value);

private Q_SLOTS:
    void privSetValue(qint8 value);

Q_SIGNALS:
    void valueChanged();

protected:
    void setupVariableCallback();

    void resetVariableCallback();

    virtual void parseVariableType() = 0;

    void readValue();

private:
    qint8 m_value;
    class QADSInteger8CallbackClass;
    QADSInteger8CallbackClass *m_adsInteger8Callback;
    unsigned long m_variableCallBackNotificationHandle;
};

class QTADS_EXPORT QADSInteger8CallbackClassTemplate : public QObject
{
    Q_OBJECT
public:
    void setValue(qint8 value);

Q_SIGNALS:
    void valueChanged(qint8);
};


/*!
  \brief Base class for 16 bit unsigned integer PLC variables.
*/

/************************************************************************************************/
/******************************** QADSUnsignedInteger16 *************************************/
/************************************************************************************************/
class QTADS_EXPORT QADSUnsignedInteger16 : public QADSPLCVariable
{
    Q_OBJECT
    Q_PROPERTY(quint16 value READ value WRITE setValue NOTIFY valueChanged)
public:
    // The read cycle time unit is 100ns as defined by TwinCAT (http://infosys.beckhoff.com/content/1033/tcadsdll2/html/TcAdsDll_StrucAdsNotificationAttrib.htm).
    explicit QADSUnsignedInteger16(QObject *parent = Q_NULLPTR, quint16 amsport = 851, const QString &amsnetid=QString("local"), const QString &amshost=QString("localhost"),
                                   const QString &var=QString(""), PLCVariableReadOperation readop=ON_DEMAND, unsigned long cycletime=0);

    ~QADSUnsignedInteger16();

    static quint16 maxValue()
    { return 65535; }

    static quint16 minValue()
    { return 0; }

    virtual quint16 value();

    virtual void initializeADSConnection() Q_DECL_OVERRIDE;

public Q_SLOTS:
    virtual void setValue(quint16 value);

private Q_SLOTS:
    void privSetValue(quint16 value);

Q_SIGNALS:
    void valueChanged();

protected:
    void setupVariableCallback();

    void resetVariableCallback();

    virtual void parseVariableType() = 0;

    void readValue();

private:
    quint16 m_value;
    class QADSUnsignedInteger16CallbackClass;
    QADSUnsignedInteger16CallbackClass *m_adsUnsignedInteger16Callback;
    unsigned long m_variableCallBackNotificationHandle;
};

class QTADS_EXPORT QADSUnsignedInteger16CallbackClassTemplate : public QObject
{
    Q_OBJECT
public:
    void setValue(quint16 value);

Q_SIGNALS:
    void valueChanged(quint16);
};


/*!
  \brief Base class for 16 bit signed integer PLC variables.
*/

/************************************************************************************************/
/******************************** QADSInteger16 *********************************************/
/************************************************************************************************/
class QTADS_EXPORT QADSInteger16 : public QADSPLCVariable
{
    Q_OBJECT
    Q_PROPERTY(qint16 value READ value WRITE setValue NOTIFY valueChanged)
public:
    // The read cycle time unit is 100ns as defined by TwinCAT (http://infosys.beckhoff.com/content/1033/tcadsdll2/html/TcAdsDll_StrucAdsNotificationAttrib.htm).
    explicit QADSInteger16(QObject *parent = Q_NULLPTR, quint16 amsport = 851, const QString &amsnetid=QString("local"), const QString &amshost=QString("localhost"),
                           const QString &var=QString(""), PLCVariableReadOperation readop=ON_DEMAND, unsigned long cycletime=0);
    ~QADSInteger16();

    static qint16 maxValue()
    { return 32767; }

    static qint16 minValue()
    { return -32768; }

    virtual qint16 value();

    virtual void initializeADSConnection() Q_DECL_OVERRIDE;

public Q_SLOTS:
    virtual void setValue(qint16 value);

private Q_SLOTS:
    void privSetValue(qint16 value);

Q_SIGNALS:
    void valueChanged();

protected:
    void setupVariableCallback();

    void resetVariableCallback();

    virtual void parseVariableType() = 0;

    void readValue();

private:
    qint16 m_value;
    class QADSInteger16CallbackClass;
    QADSInteger16CallbackClass *m_adsInteger16Callback;
    unsigned long m_variableCallBackNotificationHandle;
};

class QTADS_EXPORT QADSInteger16CallbackClassTemplate : public QObject
{
    Q_OBJECT
public:
    void setValue(qint16 value);

Q_SIGNALS:
    void valueChanged(qint16);
};


/*!
  \brief Base class for 32 bit unsigned integer PLC variables.
*/

/************************************************************************************************/
/******************************** QADSUnsignedInteger32 *************************************/
/************************************************************************************************/
class QTADS_EXPORT QADSUnsignedInteger32 : public QADSPLCVariable
{
    Q_OBJECT
    Q_PROPERTY(quint32 value READ value WRITE setValue NOTIFY valueChanged)
public:
    // The read cycle time unit is 100ns as defined by TwinCAT (http://infosys.beckhoff.com/content/1033/tcadsdll2/html/TcAdsDll_StrucAdsNotificationAttrib.htm).
    explicit QADSUnsignedInteger32(QObject *parent = Q_NULLPTR, quint16 amsport = 851, const QString &amsnetid=QString("local"), const QString &amshost=QString("localhost"),
                                   const QString &var=QString(""), PLCVariableReadOperation readop=ON_DEMAND, unsigned long cycletime=0);

    ~QADSUnsignedInteger32();

    static quint32 maxValue()
    { return 4294967295; }

    static quint32 minValue()
    { return 0; }

    virtual quint32 value();

    virtual void initializeADSConnection() Q_DECL_OVERRIDE;

public Q_SLOTS:
    virtual void setValue(quint32 value);

private Q_SLOTS:
    void privSetValue(quint32 value);

Q_SIGNALS:
    void valueChanged();

protected:
    void setupVariableCallback();

    void resetVariableCallback();

    virtual void parseVariableType() = 0;

    void readValue();

private:
    quint32 m_value;
    class QADSUnsignedInteger32CallbackClass;
    QADSUnsignedInteger32CallbackClass *m_adsUnsignedInteger32Callback;
    unsigned long m_variableCallBackNotificationHandle;
};

class QTADS_EXPORT QADSUnsignedInteger32CallbackClassTemplate : public QObject
{
    Q_OBJECT
public:
    void setValue(quint32 value);

Q_SIGNALS:
    void valueChanged(quint32);
};


/*!
  \brief Base class for 32 bit signed integer PLC variables.
*/

/************************************************************************************************/
/******************************** QADSInteger32 *********************************************/
/************************************************************************************************/
class QTADS_EXPORT QADSInteger32 : public QADSPLCVariable
{
    Q_OBJECT
    Q_PROPERTY(qint32 value READ value WRITE setValue NOTIFY valueChanged)
public:
    // The read cycle time unit is 100ns as defined by TwinCAT (http://infosys.beckhoff.com/content/1033/tcadsdll2/html/TcAdsDll_StrucAdsNotificationAttrib.htm).
    explicit QADSInteger32(QObject *parent = Q_NULLPTR, quint16 amsport = 851, const QString &amsnetid=QString("local"), const QString &amshost=QString("localhost"),
                           const QString &var=QString(""), PLCVariableReadOperation readop=ON_DEMAND, unsigned long cycletime=0);

    ~QADSInteger32();

    static qint32 maxValue()
    { return qint32(2147483647); }

    static qint32 minValue()
    { return -qint32(2147483648); }

    virtual qint32 value();

    virtual void initializeADSConnection() Q_DECL_OVERRIDE;

public Q_SLOTS:
    virtual void setValue(qint32 value);

private Q_SLOTS:
    void privSetValue(qint32 value);

Q_SIGNALS:
    void valueChanged();

protected:
    void setupVariableCallback();

    void resetVariableCallback();

    virtual void parseVariableType() = 0;

    void readValue();

private:
    qint32 m_value;
    class QADSInteger32CallbackClass;
    QADSInteger32CallbackClass *m_adsInteger32Callback;
    unsigned long m_variableCallBackNotificationHandle;
};

class QTADS_EXPORT QADSInteger32CallbackClassTemplate : public QObject
{
    Q_OBJECT
public:
    void setValue(qint32 value);

Q_SIGNALS:
    void valueChanged(qint32);
};

#endif // QADSINTEGERBASE_H
