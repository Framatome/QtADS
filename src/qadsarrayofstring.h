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
#ifndef QADSARRAYOFSTRING_H
#define QADSARRAYOFSTRING_H

#include "qadsplcvariable.h"
#include "qadsarray.h"
#include <QMetaType>

class QTADS_EXPORT QADSSTRINGARRAY : public QADSARRAY<QString>
{
public:
    QADSSTRINGARRAY()
        :QADSARRAY<QString>()
    { }

    QADSSTRINGARRAY(unsigned int x)
        :QADSARRAY<QString>(x)
    { }

    QADSSTRINGARRAY(unsigned int y, unsigned int x)
        :QADSARRAY<QString>(y,x)
    { }

    QADSSTRINGARRAY(unsigned int z, unsigned int y, unsigned int x)
        :QADSARRAY<QString>(z,y,x)
    { }

    QADSSTRINGARRAY(const QADSSTRINGARRAY &init)
        :QADSARRAY<QString>(init)
    { }

    QADSSTRINGARRAY(const std::vector<QString> &init)
        :QADSARRAY<QString>(init)
    { }

    QADSSTRINGARRAY(const QVector<QString> &init)
        :QADSARRAY<QString>(init)
    { }
};

// Need to declare new meta-type.
Q_DECLARE_METATYPE(QADSSTRINGARRAY)

/*!
  \brief Class for supporting ARRAY OF STRING PLC variables.

  \see http://infosys.beckhoff.com/content/1033/tcplccontrol/html/tcplcctrl_plc_data_types_overview.htm?id=13807
*/

class QTADS_EXPORT QADSARRAYOFSTRING : public QADSPLCVariable
{
    Q_OBJECT
    Q_PROPERTY(QADSSTRINGARRAY value READ value WRITE setValue NOTIFY valueChanged)
public:
    // The read cycle time unit is 100ns as defined by TwinCAT (http://infosys.beckhoff.com/content/1033/tcadsdll2/html/TcAdsDll_StrucAdsNotificationAttrib.htm).
    explicit QADSARRAYOFSTRING(QObject *parent = Q_NULLPTR, quint16 amsport = 851, const QString &amsnetid=QString("local"), const QString &amshost=QString("localhost"),
                               const QString &var=QString(""), PLCVariableReadOperation readop=ON_DEMAND, unsigned long cycletime=0);

    ~QADSARRAYOFSTRING();

    const QADSSTRINGARRAY &value();

    Q_INVOKABLE QString value(int x);

    Q_INVOKABLE QString value(int y, int x);

    Q_INVOKABLE QString value(int z, int y, int x);

    virtual void initializeADSConnection() Q_DECL_OVERRIDE;

public Q_SLOTS:
    void setValue(const QADSSTRINGARRAY &value);

    void setValue(QString val, int x);

    void setValue(QString val, int y, int x);

    void setValue(QString val, int z, int y, int x);

private Q_SLOTS:
    void privSetValue(const QADSSTRINGARRAY &val);

Q_SIGNALS:
    void valueChanged();

protected:
    virtual void protCatchADSStateChange() Q_DECL_OVERRIDE;

    virtual void protCatchADSSymbolTableChanged() Q_DECL_OVERRIDE;

    void setupVariableCallback();

    void resetVariableCallback();

    void parseArrayParameters();

    void readValue();

private:
    static int m_metaTypeId;
    QADSSTRINGARRAY m_value;
    class QADSARRAYOFSTRINGCallbackClass;
    QADSARRAYOFSTRINGCallbackClass *m_adsARRAYOFSTRINGCallback;
    unsigned long m_variableCallBackNotificationHandle;
    unsigned long m_parsedStringSize;
};

class QTADS_EXPORT QADSARRAYOFSTRINGCallbackClassTemplate : public QObject
{
    Q_OBJECT
public:
    QADSARRAYOFSTRINGCallbackClassTemplate( unsigned int z,
                                              unsigned int y,
                                              unsigned int x, 
                                              unsigned int size)
        :m_x(x), m_y(y), m_z(z), m_size(size)
    {}

    unsigned int x() const
    { return m_x; }

    unsigned int y() const
    { return m_y; }

    unsigned int z() const
    { return m_z; }

    unsigned int count()
    { return (m_x*m_y*m_z); }

    unsigned int size()
    { return m_size; }

    void setValue(const QADSSTRINGARRAY &val);

Q_SIGNALS:
    void valueChanged(const QADSSTRINGARRAY &);

private:
    unsigned int m_x;
    unsigned int m_y;
    unsigned int m_z;
    unsigned int m_size;
};

#endif // QADSARRAYOFSTRING_H
