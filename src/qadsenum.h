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
#ifndef QADSENUM_H
#define QADSENUM_H

#include "qadsintegerbase.h"

/*!
  \brief Class for supporting ENUM PLC variables.

  \see http://infosys.beckhoff.com/content/1033/tcplccontrol/html/tcplcctrl_plc_data_types_overview.htm?id=13807
*/

class QTADS_EXPORT QADSENUM : public QADSInteger16
{
    Q_OBJECT
    Q_PROPERTY(QString enumerationName READ enumerationName WRITE setEnumerationName NOTIFY enumerationNameChanged)
public:
    // The read cycle time unit is 100ns as defined by TwinCAT (http://infosys.beckhoff.com/content/1033/tcadsdll2/html/TcAdsDll_StrucAdsNotificationAttrib.htm).
    explicit QADSENUM(QObject *parent = Q_NULLPTR, quint16 amsport = 851, const QString &amsnetid=QString("local"), const QString &amshost=QString("localhost"),
                      const QString &var=QString(""), PLCVariableReadOperation readop=ON_DEMAND,unsigned long cycletime=0,
                      const QString &enumName=QString(""));

    const QString &enumerationName() const
    { return m_enumerationName; }

    virtual qint16 value() Q_DECL_OVERRIDE;

    virtual void setValue(qint16 val) Q_DECL_OVERRIDE;

public Q_SLOTS:
    void setEnumerationName(const QString &name);

Q_SIGNALS:
    void enumerationNameChanged();

protected:
    virtual void parseVariableType() Q_DECL_OVERRIDE;

private:
    QString m_enumerationName;

};

#endif // QADSENUM_H
