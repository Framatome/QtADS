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
#ifndef QADSARRAYINTEGERBASE_H
#define QADSARRAYINTEGERBASE_H

#include "qadsplcvariable.h"
#include "qadsarray.h"
#include <QMetaType>

/************************************************************************************************/
/******************************** QADSUnsignedInteger16Array ********************************/
/************************************************************************************************/
class QTADS_EXPORT QADSQUINT16ARRAY : public QADSARRAY<quint16>
{
public:
    QADSQUINT16ARRAY()
        :QADSARRAY<quint16>()
    { }

    QADSQUINT16ARRAY(unsigned int x)
        :QADSARRAY<quint16>(x)
    { }

    QADSQUINT16ARRAY(unsigned int y, unsigned int x)
        :QADSARRAY<quint16>(y,x)
    {  }

    QADSQUINT16ARRAY(unsigned int z, unsigned int y, unsigned int x)
        :QADSARRAY<quint16>(z,y,x)
    { }

    QADSQUINT16ARRAY(const QADSQUINT16ARRAY &init)
        :QADSARRAY<quint16>(init)
    { }

    QADSQUINT16ARRAY(const std::vector<quint16> &init)
        :QADSARRAY<quint16>(init)
    { }

    QADSQUINT16ARRAY(const QVector<quint16> &init)
        :QADSARRAY<quint16>(init)
    { }
};

// Need to declare new meta-type.
Q_DECLARE_METATYPE(QADSQUINT16ARRAY)

/*!
  \brief Base class for 16 bit unsigned integer PLC variable arrays.
*/

class QTADS_EXPORT QADSUnsignedInteger16Array : public QADSPLCVariable
{
    Q_OBJECT
    Q_PROPERTY(QADSQUINT16ARRAY value READ value WRITE setValue NOTIFY valueChanged)
public:
    // The read cycle time unit is 100ns as defined by TwinCAT (http://infosys.beckhoff.com/content/1033/tcadsdll2/html/TcAdsDll_StrucAdsNotificationAttrib.htm).
    explicit QADSUnsignedInteger16Array(QObject *parent = Q_NULLPTR, quint16 amsport = 851, const QString &amsnetid=QString("local"), const QString &amshost=QString("localhost"),
                                        const QString &var=QString(""), PLCVariableReadOperation readop=ON_DEMAND, unsigned long cycletime=0);

    ~QADSUnsignedInteger16Array();

    virtual const QADSQUINT16ARRAY &value();

    Q_INVOKABLE virtual quint16 value(int x);

    Q_INVOKABLE virtual quint16 value(int y, int x);

    Q_INVOKABLE virtual quint16 value(int z, int y, int x);

    virtual void initializeADSConnection() Q_DECL_OVERRIDE;

public Q_SLOTS:
    virtual void setValue(const QADSQUINT16ARRAY &val);

    virtual void setValue(quint16 val, int x);

    virtual void setValue(quint16 val, int y, int x);

    virtual void setValue(quint16 val, int z, int y, int x);

private Q_SLOTS:
    void privSetValue(const QADSQUINT16ARRAY &val);

Q_SIGNALS:
    void valueChanged();

protected:
    void setupVariableCallback();

    void resetVariableCallback();

    virtual void parseArrayParameters();

    void readValue();

private:
    void baseParseArrayParameters();

    static int m_metaTypeId;
    QADSQUINT16ARRAY m_value;
    class QADSUnsignedInteger16ArrayCallbackClass;
    QADSUnsignedInteger16ArrayCallbackClass *m_adsUnsignedInteger16ArrayCallback;
    unsigned long m_variableCallBackNotificationHandle;
};

class QTADS_EXPORT QADSUnsignedInteger16ArrayCallbackClassTemplate : public QObject
{
    Q_OBJECT
public:
    QADSUnsignedInteger16ArrayCallbackClassTemplate(unsigned int z,
                                              unsigned int y,
                                              unsigned int x)
        :m_x(x), m_y(y), m_z(z)
    {}

    unsigned int x() const
    { return m_x; }

    unsigned int y() const
    { return m_y; }

    unsigned int z() const
    { return m_z; }

    unsigned int count()
    { return (m_x*m_y*m_z); }

    void setValue(const QADSQUINT16ARRAY &val);

Q_SIGNALS:
    void valueChanged(const QADSQUINT16ARRAY &);

private:
    unsigned int m_x;
    unsigned int m_y;
    unsigned int m_z;
};


/************************************************************************************************/
/******************************** QADSInteger16Array ****************************************/
/************************************************************************************************/
class QTADS_EXPORT QADSQINT16ARRAY : public QADSARRAY<qint16>
{
public:
    QADSQINT16ARRAY()
        :QADSARRAY<qint16>()
    { }

    QADSQINT16ARRAY(unsigned int x)
        :QADSARRAY<qint16>(x)
    { }

    QADSQINT16ARRAY(unsigned int y, unsigned int x)
        :QADSARRAY<qint16>(y,x)
    {  }

    QADSQINT16ARRAY(unsigned int z, unsigned int y, unsigned int x)
        :QADSARRAY<qint16>(z,y,x)
    { }

    QADSQINT16ARRAY(const QADSQINT16ARRAY &init)
        :QADSARRAY<qint16>(init)
    { }

    QADSQINT16ARRAY(const std::vector<qint16> &init)
        :QADSARRAY<qint16>(init)
    { }

    QADSQINT16ARRAY(const QVector<qint16> &init)
        :QADSARRAY<qint16>(init)
    { }
};

// Need to declare new meta-type.
Q_DECLARE_METATYPE(QADSQINT16ARRAY)

/*!
  \brief Base class for 16 bit signed integer PLC variable arrays.
*/

class QTADS_EXPORT QADSInteger16Array : public QADSPLCVariable
{
    Q_OBJECT
    Q_PROPERTY(QADSQINT16ARRAY value READ value WRITE setValue NOTIFY valueChanged)
public:
    // The read cycle time unit is 100ns as defined by TwinCAT (http://infosys.beckhoff.com/content/1033/tcadsdll2/html/TcAdsDll_StrucAdsNotificationAttrib.htm).
    explicit QADSInteger16Array(QObject *parent = Q_NULLPTR, quint16 amsport = 851, const QString &amsnetid=QString("local"), const QString &amshost=QString("localhost"),
                                const QString &var=QString(""), PLCVariableReadOperation readop=ON_DEMAND, unsigned long cycletime=0);

    ~QADSInteger16Array();

    virtual const QADSQINT16ARRAY &value();

    Q_INVOKABLE virtual qint16 value(int x);

    Q_INVOKABLE virtual qint16 value(int y, int x);

    Q_INVOKABLE virtual qint16 value(int z, int y, int x);

    virtual void initializeADSConnection() Q_DECL_OVERRIDE;

public Q_SLOTS:
    virtual void setValue(const QADSQINT16ARRAY &val);

    virtual void setValue(qint16 val, int x);

    virtual void setValue(qint16 val, int y, int x);

    virtual void setValue(qint16 val, int z, int y, int x);

private Q_SLOTS:
    void privSetValue(const QADSQINT16ARRAY &val);

Q_SIGNALS:
    void valueChanged();

protected:
    void setupVariableCallback();

    void resetVariableCallback();

    virtual void parseArrayParameters();

    void readValue();

private:
    void baseParseArrayParameters();

    static int m_metaTypeId;
    QADSQINT16ARRAY m_value;
    class QADSInteger16ArrayCallbackClass;
    QADSInteger16ArrayCallbackClass *m_adsInteger16ArrayCallback;
    unsigned long m_variableCallBackNotificationHandle;
};

class QTADS_EXPORT QADSInteger16ArrayCallbackClassTemplate : public QObject
{
    Q_OBJECT
public:
    QADSInteger16ArrayCallbackClassTemplate( unsigned int z,
                                              unsigned int y,
                                              unsigned int x)
        :m_x(x), m_y(y), m_z(z)
    {}

    unsigned int x() const
    { return m_x; }

    unsigned int y() const
    { return m_y; }

    unsigned int z() const
    { return m_z; }

    unsigned int count()
    { return (m_x*m_y*m_z); }

    void setValue(const QADSQINT16ARRAY &val);

Q_SIGNALS:
    void valueChanged(const QADSQINT16ARRAY &);

private:
    unsigned int m_x;
    unsigned int m_y;
    unsigned int m_z;
};

/************************************************************************************************/
/******************************** QADSUnsignedInteger32Array ********************************/
/************************************************************************************************/
class QTADS_EXPORT QADSQUINT32ARRAY : public QADSARRAY<quint32>
{
public:
    QADSQUINT32ARRAY()
        :QADSARRAY<quint32>()
    { }

    QADSQUINT32ARRAY(unsigned int x)
        :QADSARRAY<quint32>(x)
    { }

    QADSQUINT32ARRAY(unsigned int y, unsigned int x)
        :QADSARRAY<quint32>(y,x)
    {  }

    QADSQUINT32ARRAY(unsigned int z, unsigned int y, unsigned int x)
        :QADSARRAY<quint32>(z,y,x)
    { }

    QADSQUINT32ARRAY(const QADSQUINT32ARRAY &init)
        :QADSARRAY<quint32>(init)
    { }

    QADSQUINT32ARRAY(const std::vector<quint32> &init)
        :QADSARRAY<quint32>(init)
    { }

    QADSQUINT32ARRAY(const QVector<quint32> &init)
        :QADSARRAY<quint32>(init)
    { }
};

// Need to declare new meta-type.
Q_DECLARE_METATYPE(QADSQUINT32ARRAY)

/*!
  \brief Base class for 32 bit unsigned integer PLC variable arrays.
*/

class QTADS_EXPORT QADSUnsignedInteger32Array : public QADSPLCVariable
{
    Q_OBJECT
    Q_PROPERTY(QADSQUINT32ARRAY value READ value WRITE setValue NOTIFY valueChanged)
public:
    // The read cycle time unit is 100ns as defined by TwinCAT (http://infosys.beckhoff.com/content/1033/tcadsdll2/html/TcAdsDll_StrucAdsNotificationAttrib.htm).
    explicit QADSUnsignedInteger32Array(QObject *parent = Q_NULLPTR, quint16 amsport = 851, const QString &amsnetid=QString("local"), const QString &amshost=QString("localhost"),
                                        const QString &var=QString(""), PLCVariableReadOperation readop=ON_DEMAND, unsigned long cycletime=0);

    ~QADSUnsignedInteger32Array();

    virtual const QADSQUINT32ARRAY &value();

    Q_INVOKABLE virtual quint32 value(int x);

    Q_INVOKABLE virtual quint32 value(int y, int x);

    Q_INVOKABLE virtual quint32 value(int z, int y, int x);

    virtual void initializeADSConnection() Q_DECL_OVERRIDE;

public Q_SLOTS:
    virtual void setValue(const QADSQUINT32ARRAY &val);

    virtual void setValue(quint32 val, int x);

    virtual void setValue(quint32 val, int y, int x);

    virtual void setValue(quint32 val, int z, int y, int x);

private Q_SLOTS:
    void privSetValue(const QADSQUINT32ARRAY &val);

Q_SIGNALS:
    void valueChanged();

protected:
    void setupVariableCallback();

    void resetVariableCallback();

    virtual void parseArrayParameters();

    void readValue();

private:
    void baseParseArrayParameters();

    static int m_metaTypeId;
    QADSQUINT32ARRAY m_value;
    class QADSUnsignedInteger32ArrayCallbackClass;
    QADSUnsignedInteger32ArrayCallbackClass *m_adsUnsignedInteger32ArrayCallback;
    unsigned long m_variableCallBackNotificationHandle;
};

class QTADS_EXPORT QADSUnsignedInteger32ArrayCallbackClassTemplate : public QObject
{
    Q_OBJECT
public:
    QADSUnsignedInteger32ArrayCallbackClassTemplate( unsigned int z,
                                              unsigned int y,
                                              unsigned int x)
        :m_x(x), m_y(y), m_z(z)
    {}

    unsigned int x() const
    { return m_x; }

    unsigned int y() const
    { return m_y; }

    unsigned int z() const
    { return m_z; }

    unsigned int count()
    { return (m_x*m_y*m_z); }

    void setValue(const QADSQUINT32ARRAY &val);

Q_SIGNALS:
    void valueChanged(const QADSQUINT32ARRAY &);

private:
    unsigned int m_x;
    unsigned int m_y;
    unsigned int m_z;
};


/************************************************************************************************/
/******************************** QADSInteger32Array ****************************************/
/************************************************************************************************/
class QTADS_EXPORT QADSQINT32ARRAY : public QADSARRAY<qint32>
{
public:
    QADSQINT32ARRAY()
        :QADSARRAY<qint32>()
    { }

    QADSQINT32ARRAY(unsigned int x)
        :QADSARRAY<qint32>(x)
    { }

    QADSQINT32ARRAY(unsigned int y, unsigned int x)
        :QADSARRAY<qint32>(y,x)
    {  }

    QADSQINT32ARRAY(unsigned int z, unsigned int y, unsigned int x)
        :QADSARRAY<qint32>(z,y,x)
    { }

    QADSQINT32ARRAY(const QADSQINT32ARRAY &init)
        :QADSARRAY<qint32>(init)
    { }

    QADSQINT32ARRAY(const std::vector<qint32> &init)
        :QADSARRAY<qint32>(init)
    { }

    QADSQINT32ARRAY(const QVector<qint32> &init)
        :QADSARRAY<qint32>(init)
    { }
};

// Need to declare new meta-type.
Q_DECLARE_METATYPE(QADSQINT32ARRAY)

/*!
  \brief Base class for 32 bit signed integer PLC variable arrays.
*/

class QTADS_EXPORT QADSInteger32Array : public QADSPLCVariable
{
    Q_OBJECT
    Q_PROPERTY(QADSQINT32ARRAY value READ value WRITE setValue NOTIFY valueChanged)
public:
    // The read cycle time unit is 100ns as defined by TwinCAT (http://infosys.beckhoff.com/content/1033/tcadsdll2/html/TcAdsDll_StrucAdsNotificationAttrib.htm).
    explicit QADSInteger32Array(QObject *parent = Q_NULLPTR, quint16 amsport = 851, const QString &amsnetid=QString("local"), const QString &amshost=QString("localhost"),
                                const QString &var=QString(""), PLCVariableReadOperation readop=ON_DEMAND, unsigned long cycletime=0);

    ~QADSInteger32Array();

    virtual const QADSQINT32ARRAY &value();

    Q_INVOKABLE virtual qint32 value(int x);

    Q_INVOKABLE virtual qint32 value(int y, int x);

    Q_INVOKABLE virtual qint32 value(int z, int y, int x);

    virtual void initializeADSConnection() Q_DECL_OVERRIDE;

public Q_SLOTS:
    virtual void setValue(const QADSQINT32ARRAY &val);

    virtual void setValue(qint32 val, int x);

    virtual void setValue(qint32 val, int y, int x);

    virtual void setValue(qint32 val, int z, int y, int x);

private Q_SLOTS:
    void privSetValue(const QADSQINT32ARRAY &val);

Q_SIGNALS:
    void valueChanged();

protected:
    void setupVariableCallback();

    void resetVariableCallback();

    virtual void parseArrayParameters();

    void readValue();

private:
    void baseParseArrayParameters();

    static int m_metaTypeId;
    QADSQINT32ARRAY m_value;
    class QADSInteger32ArrayCallbackClass;
    QADSInteger32ArrayCallbackClass *m_adsInteger32ArrayCallback;
    unsigned long m_variableCallBackNotificationHandle;
};

class QTADS_EXPORT QADSInteger32ArrayCallbackClassTemplate : public QObject
{
    Q_OBJECT
public:
    QADSInteger32ArrayCallbackClassTemplate( unsigned int z,
                                              unsigned int y,
                                              unsigned int x)
        :m_x(x), m_y(y), m_z(z)
    {}

    unsigned int x() const
    { return m_x; }

    unsigned int y() const
    { return m_y; }

    unsigned int z() const
    { return m_z; }

    unsigned int count()
    { return (m_x*m_y*m_z); }

    void setValue(const QADSQINT32ARRAY &val);

Q_SIGNALS:
    void valueChanged(const QADSQINT32ARRAY &);

private:
    unsigned int m_x;
    unsigned int m_y;
    unsigned int m_z;
};

#endif // QADSARRAYINTEGERBASE_H
