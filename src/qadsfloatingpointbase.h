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
#ifndef QADSFLOATINGPOINTBASE_H
#define QADSFLOATINGPOINTBASE_H

#include "qadsplcvariable.h"

/*!
  \brief Base class for 64 bit floating point PLC variables.
*/

/************************************************************************************************/
/******************************** QADSFloatingPoint64 ***************************************/
/************************************************************************************************/
class QTADS_EXPORT QADSFloatingPoint64 : public QADSPLCVariable
{
    Q_OBJECT
    Q_PROPERTY(double value READ value WRITE setValue NOTIFY valueChanged)
public:
    // The read cycle time unit is 100ns as defined by TwinCAT (http://infosys.beckhoff.com/content/1033/tcadsdll2/html/TcAdsDll_StrucAdsNotificationAttrib.htm).
    explicit QADSFloatingPoint64(QObject *parent = Q_NULLPTR, quint16 amsport = 851, const QString &amsnetid=QString("local"), const QString &amshost=QString("localhost"),
                                 const QString &var=QString(""), PLCVariableReadOperation readop=ON_DEMAND, unsigned long cycletime=0);

    ~QADSFloatingPoint64();

    virtual double value();

    static double maxValue()
    { return 1.7976931348623157e308; }

    static double minValue()
    { return -1.7976931348623157e308; }

    virtual void initializeADSConnection() Q_DECL_OVERRIDE;

public Q_SLOTS:
    virtual void setValue(double value);

private Q_SLOTS:
    void privSetValue(double value);

Q_SIGNALS:
    void valueChanged();

protected:
    void setupVariableCallback();

    void resetVariableCallback();

    virtual void parseVariableType() = 0;

    void readValue();

private:
    double m_value;
    class QADSFloatingPoint64CallbackClass;
    QADSFloatingPoint64CallbackClass *m_adsFloatingPoint64Callback;
    unsigned long m_variableCallBackNotificationHandle;
};

class QTADS_EXPORT QADSFloatingPoint64CallbackClassTemplate : public QObject
{
    Q_OBJECT
public:
    void setValue(double value);

Q_SIGNALS:
    void valueChanged(double);
};

/*!
  \brief Base class for 32 bit floating point PLC variables.
*/
/************************************************************************************************/
/******************************** QADSFloatingPoint32 ***************************************/
/************************************************************************************************/
class QTADS_EXPORT QADSFloatingPoint32 : public QADSPLCVariable
{
    Q_OBJECT
    Q_PROPERTY(float value READ value WRITE setValue NOTIFY valueChanged)
public:
    // The read cycle time unit is 100ns as defined by TwinCAT (http://infosys.beckhoff.com/content/1033/tcadsdll2/html/TcAdsDll_StrucAdsNotificationAttrib.htm).
    explicit QADSFloatingPoint32(QObject *parent = Q_NULLPTR, quint16 amsport = 851, const QString &amsnetid=QString("local"), const QString &amshost=QString("localhost"),
                                 const QString &var=QString(""), PLCVariableReadOperation readop=ON_DEMAND, unsigned long cycletime=0);

    ~QADSFloatingPoint32();

    virtual float value();

    static float maxValue()
    { return 3.4028234e38f; }

    static float minValue()
    { return -3.4028234e38f; }

    virtual void initializeADSConnection() Q_DECL_OVERRIDE;

public Q_SLOTS:
    virtual void setValue(float value);

private Q_SLOTS:
    void privSetValue(float value);

Q_SIGNALS:
    void valueChanged();

protected:
    void setupVariableCallback();

    void resetVariableCallback();

    virtual void parseVariableType() = 0;

    void readValue();

private:
    float m_value;
    class QADSFloatingPoint32CallbackClass;
    QADSFloatingPoint32CallbackClass *m_adsFloatingPoint32Callback;
    unsigned long m_variableCallBackNotificationHandle;
};

class QTADS_EXPORT QADSFloatingPoint32CallbackClassTemplate : public QObject
{
    Q_OBJECT
public:
    void setValue(float value);

Q_SIGNALS:
    void valueChanged(float);
};

#endif // QADSFLOATINGPOINTBASE_H
