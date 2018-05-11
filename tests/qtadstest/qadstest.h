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
#ifndef QADSTEST_H
#define QADSTEST_H

#include <QObject>
#include <QADSTIME>
#include <QADSARRAYOFSTRING>
#include <QADSARRAYOFUINT>

// This test requires that the QtADSExamplePLC TwinCAT 3 program is running.

class QADSTest : public QObject
{
    Q_OBJECT
public:
    explicit QADSTest(QObject *parent = Q_NULLPTR);

Q_SIGNALS:
    void finished();

public Q_SLOTS:
    void run();
    void tPassageOfTimeChanged();
    void quit();

private:
    QADSTIME *m_tPassageOfTime;
    QADSARRAYOFSTRING* m_aExampleSTRING;
    QADSARRAYOFUINT* m_aExampleUINT;
};

#endif // QADSTEST_H
