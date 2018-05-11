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
#include "qadstest.h"
#include <QTimer>
#include <QTextStream>

QADSTest::QADSTest(QObject *parent) :
    QObject(parent),
    m_tPassageOfTime(Q_NULLPTR),
    m_aExampleSTRING(Q_NULLPTR),
    m_aExampleUINT(Q_NULLPTR)
{
}

void QADSTest::run()
{
    // Cyclical and Synchronized methods don't work with QCoreApplication.  Implement a polling method instead using QTimer.
    m_tPassageOfTime = new QADSTIME(this, 851, "local", "localhost", "GVL.tPassageOfTime", QADSPLCVariable::ON_DEMAND, 0);
    m_aExampleSTRING = new QADSARRAYOFSTRING(this, 851, "local", "localhost", "MAIN.aExampleSTRING", QADSPLCVariable::ON_DEMAND, 0);
    m_aExampleUINT = new QADSARRAYOFUINT(this, 851, "local", "localhost", "MAIN.aExampleUINT", QADSPLCVariable::ON_DEMAND, 0);
    if ( !m_tPassageOfTime->adsError() )
    {
        QTextStream(stdout) << "ADS Dll Version: " << static_cast<int>(m_tPassageOfTime->adsDllVersion()) << "\n";
        QTextStream(stdout) << "ADS Dll Revision: " << static_cast<int>(m_tPassageOfTime->adsDllRevision()) << "\n";
        QTextStream(stdout) << "ADS Dll Build: " << m_tPassageOfTime->adsDllBuild() << "\n";
        QTextStream(stdout) << "AMS Net ID: " << m_tPassageOfTime->amsNetId() << "\n";
        QTextStream(stdout) << "ADS Port: " << m_tPassageOfTime->adsPort() << "\n";
        QTextStream(stdout) << "ADS Device Name: " << m_tPassageOfTime->adsDeviceName() << "\n";
        QTextStream(stdout) << "ADS Device Version: " << static_cast<int>(m_tPassageOfTime->adsDeviceVersion()) << "\n";
        QTextStream(stdout) << "ADS Device Revision: " << static_cast<int>(m_tPassageOfTime->adsDeviceRevision()) << "\n";
        QTextStream(stdout) << "ADS Device Build: " << m_tPassageOfTime->adsDeviceBuild() << "\n";
        QTextStream(stdout) << "ADS PLC State: " << m_tPassageOfTime->adsStateAsString() << "\n";
        QTextStream(stdout) << "ADS PLC Symbol Name: " << m_tPassageOfTime->adsSymbolName() << "\n";
        QTextStream(stdout) << "ADS PLC Symbol Group: " << m_tPassageOfTime->adsSymbolGroup() << "\n";
        QTextStream(stdout) << "ADS PLC Symbol Offset: " << m_tPassageOfTime->adsSymbolOffset() << "\n";
        QTextStream(stdout) << "ADS PLC Symbol Size: " << m_tPassageOfTime->adsSymbolSize() << "\n";
        QTextStream(stdout) << "ADS PLC Symbol Comment: " << m_tPassageOfTime->adsSymbolComment() << "\n";
        QTextStream(stdout) << "ADS PLC Symbol Handle: " << m_tPassageOfTime->adsSymbolHandle() << "\n";

        QTimer *timer = new QTimer(this);
        connect(timer, SIGNAL(timeout()), this, SLOT(tPassageOfTimeChanged()));
        timer->start(200);
    }
    else
    {
        QTextStream(stdout) << "Error Initializing QADSTIME: " << m_tPassageOfTime->adsErrorString() << "\n";
    }
    QTimer::singleShot(30000, this, SLOT(quit()));
}

void QADSTest::tPassageOfTimeChanged()
{
    QTextStream(stdout) << "tPassageOfTime: " << m_tPassageOfTime->value().toString("hh:mm:ss.zzz") << "\n";
    if ( !m_aExampleSTRING->adsError() )
    {
        QADSSTRINGARRAY aExampleSTRING = m_aExampleSTRING->value();
        if (aExampleSTRING.count() >= 3)
        {
            QString value = aExampleSTRING[2];
            aExampleSTRING[2] = aExampleSTRING[1];
            aExampleSTRING[1] = aExampleSTRING[0];
            aExampleSTRING[0] = value;
            m_aExampleSTRING->setValue(aExampleSTRING);
        }
        const quint16 aUINT = m_aExampleUINT->value(0);
        m_aExampleUINT->setValue(m_aExampleUINT->value(2), 0);
        m_aExampleUINT->setValue(aUINT, 2);
    }
}

void QADSTest::quit()
{
    Q_EMIT finished();
}
