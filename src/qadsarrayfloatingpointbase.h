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
#ifndef QADSARRAYFLOATINGPOINTBASE_H
#define QADSARRAYFLOATINGPOINTBASE_H

#include "qadsplcvariable.h"
#include "qadsarray.h"
#include <QMetaType>

/************************************************************************************************/
/******************************** QADSFloatingPoint64Array **********************************/
/************************************************************************************************/
class QTADS_EXPORT QADSDOUBLEARRAY : public QADSARRAY<double>
{
public:
    QADSDOUBLEARRAY()
        :QADSARRAY<double>()
    { }

    QADSDOUBLEARRAY(unsigned int x)
        :QADSARRAY<double>(x)
    { }

    QADSDOUBLEARRAY(unsigned int y, unsigned int x)
        :QADSARRAY<double>(y,x)
    {  }

    QADSDOUBLEARRAY(unsigned int z, unsigned int y, unsigned int x)
        :QADSARRAY<double>(z,y,x)
    { }

    QADSDOUBLEARRAY(const QADSDOUBLEARRAY &init)
        :QADSARRAY<double>(init)
    { }

    QADSDOUBLEARRAY(const std::vector<double> &init)
        :QADSARRAY<double>(init)
    { }

    QADSDOUBLEARRAY(const QVector<double> &init)
        :QADSARRAY<double>(init)
    { }
};

// Need to declare new meta-type.
Q_DECLARE_METATYPE(QADSDOUBLEARRAY)

/*!
  \brief Base class for 64 bit floating point PLC variable arrays.
*/

class QTADS_EXPORT QADSFloatingPoint64Array : public QADSPLCVariable
{
    Q_OBJECT
    Q_PROPERTY(QADSDOUBLEARRAY value READ value WRITE setValue NOTIFY valueChanged)
public:
    // The read cycle time unit is 100ns as defined by TwinCAT (http://infosys.beckhoff.com/content/1033/tcadsdll2/html/TcAdsDll_StrucAdsNotificationAttrib.htm).
    explicit QADSFloatingPoint64Array(QObject *parent = Q_NULLPTR, quint16 amsport = 851, const QString &amsnetid=QString("local"), const QString &amshost=QString("localhost"),
                                      const QString &var=QString(""), PLCVariableReadOperation readop=ON_DEMAND, unsigned long cycletime=0);

    ~QADSFloatingPoint64Array();

    virtual const QADSDOUBLEARRAY &value();

    Q_INVOKABLE virtual double value(int x);

    Q_INVOKABLE virtual double value(int y, int x);

    Q_INVOKABLE virtual double value(int z, int y, int x);

    virtual void initializeADSConnection() Q_DECL_OVERRIDE;

public Q_SLOTS:
    virtual void setValue(const QADSDOUBLEARRAY &val);

    virtual void setValue(double val, int x);

    virtual void setValue(double val, int y, int x);

    virtual void setValue(double val, int z, int y, int x);

private Q_SLOTS:
    void privSetValue(const QADSDOUBLEARRAY &val);

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
    QADSDOUBLEARRAY m_value;
    class QADSFloatingPoint64ArrayCallbackClass;
    QADSFloatingPoint64ArrayCallbackClass *m_adsFloatingPoint64ArrayCallback;
    unsigned long m_variableCallBackNotificationHandle;
};

class QTADS_EXPORT QADSFloatingPoint64ArrayCallbackClassTemplate : public QObject
{
    Q_OBJECT
public:
    QADSFloatingPoint64ArrayCallbackClassTemplate( unsigned int z,
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

    void setValue(const QADSDOUBLEARRAY &val);

Q_SIGNALS:
    void valueChanged(const QADSDOUBLEARRAY &);

private:
    unsigned int m_x;
    unsigned int m_y;
    unsigned int m_z;
};

/************************************************************************************************/
/******************************** QADSFloatingPoint32Array **********************************/
/************************************************************************************************/
class QTADS_EXPORT QADSFLOATARRAY : public QADSARRAY<float>
{
public:
    QADSFLOATARRAY()
        :QADSARRAY<float>()
    { }

    QADSFLOATARRAY(unsigned int x)
        :QADSARRAY<float>(x)
    { }

    QADSFLOATARRAY(unsigned int y, unsigned int x)
        :QADSARRAY<float>(y,x)
    {  }

    QADSFLOATARRAY(unsigned int z, unsigned int y, unsigned int x)
        :QADSARRAY<float>(z,y,x)
    { }

    QADSFLOATARRAY(const QADSFLOATARRAY &init)
        :QADSARRAY<float>(init)
    { }

    QADSFLOATARRAY(const std::vector<float> &init)
        :QADSARRAY<float>(init)
    { }

    QADSFLOATARRAY(const QVector<float> &init)
        :QADSARRAY<float>(init)
    { }
};

// Need to declare new meta-type.
Q_DECLARE_METATYPE(QADSFLOATARRAY)

/*!
  \brief Base class for 32 bit floating point PLC variable arrays.
*/

class QTADS_EXPORT QADSFloatingPoint32Array : public QADSPLCVariable
{
    Q_OBJECT
    Q_PROPERTY(QADSFLOATARRAY value READ value WRITE setValue NOTIFY valueChanged)
public:
    // The read cycle time unit is 100ns as defined by TwinCAT (http://infosys.beckhoff.com/content/1033/tcadsdll2/html/TcAdsDll_StrucAdsNotificationAttrib.htm).
    explicit QADSFloatingPoint32Array(QObject *parent = Q_NULLPTR, quint16 amsport = 851, const QString &amsnetid=QString("local"), const QString &amshost=QString("localhost"),
                                      const QString &var=QString(""), PLCVariableReadOperation readop=ON_DEMAND, unsigned long cycletime=0);

    ~QADSFloatingPoint32Array();

    virtual const QADSFLOATARRAY &value();

    Q_INVOKABLE virtual float value(int x);

    Q_INVOKABLE virtual float value(int y, int x);

    Q_INVOKABLE virtual float value(int z, int y, int x);

    virtual void initializeADSConnection() Q_DECL_OVERRIDE;

public Q_SLOTS:
    virtual void setValue(const QADSFLOATARRAY &val);

    virtual void setValue(float val, int x);

    virtual void setValue(float val, int y, int x);

    virtual void setValue(float val, int z, int y, int x);

private Q_SLOTS:
    void privSetValue(const QADSFLOATARRAY &val);

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
    QADSFLOATARRAY m_value;
    class QADSFloatingPoint32ArrayCallbackClass;
    QADSFloatingPoint32ArrayCallbackClass *m_adsFloatingPoint32ArrayCallback;
    unsigned long m_variableCallBackNotificationHandle;
};

class QTADS_EXPORT QADSFloatingPoint32ArrayCallbackClassTemplate : public QObject
{
    Q_OBJECT
public:
    QADSFloatingPoint32ArrayCallbackClassTemplate( unsigned int z,
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

    void setValue(const QADSFLOATARRAY &val);

Q_SIGNALS:
    void valueChanged(const QADSFLOATARRAY &);

private:
    unsigned int m_x;
    unsigned int m_y;
    unsigned int m_z;
};

#endif // QADSARRAYFLOATINGPOINTBASE_H
