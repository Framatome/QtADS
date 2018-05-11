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
#ifndef QADSDIALOG_H
#define QADSDIALOG_H

#include <QDialog>

namespace Ui {
class QAdsDialog;
}
class QADSENUM;
class QADSTIME;
class QADSUINT;
class QADSUDINT;
class QADSSTRING;
class QADSREAL;
class QADSLREAL;
class QADSINT;
class QADSDINT;
class QADSBOOL;
class QADSBYTE;
class QADSDWORD;
class QADSWORD;
class QADSSINT;
class QADSUSINT;
class QADSARRAYOFUINT;
class QADSARRAYOFUDINT;
class QADSARRAYOFSTRING;
class QADSARRAYOFREAL;
class QADSARRAYOFLREAL;
class QADSARRAYOFINT;
class QADSARRAYOFDINT;
class QADSARRAYOFBOOL;

class QAdsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit QAdsDialog(QWidget *parent = 0);
    ~QAdsDialog();

private slots:
    void catchADSErrorChanged();

    void catchADSStateChanged();

    void catchAMSEventChanged();

    void on_radioButtonExample0_toggled(bool checked);

    void on_radioButtonExample1_toggled(bool checked);

    void on_radioButtonExample2_toggled(bool checked);

    void tPassageOfTimeChanged();

    void uiExampleChanged();

    void udiExampleChanged();

    void sExampleChanged();

    void rExampleChanged();

    void lrExampleChanged();

    void iExampleChanged();

    void diExampleChanged();

    void boolExampleChanged();

    void bExampleChanged();

    void dwExampleChanged();

    void wExampleChanged();

    void siExampleChanged();

    void usiExampleChanged();

private:
    Ui::QAdsDialog *ui;
    unsigned int m_textEditMessageCount;
    QADSENUM* m_eExampleEnum;
    QADSENUM* m_eGlobalExampleEnum;
    QADSTIME* m_tPassageOfTime;
    QADSUINT* m_uiExample;
    QADSUDINT* m_udiExample;
    QADSSTRING* m_sExample;
    QADSREAL* m_rExample;
    QADSLREAL* m_lrExample;
    QADSINT* m_iExample;
    QADSDINT* m_diExample;
    QADSBOOL* m_boolExample;
    QADSBYTE* m_bExample;
    QADSDWORD* m_dwExample;
    QADSWORD* m_wExample;
    QADSSINT* m_siExample;
    QADSUSINT* m_usiExample;
    QADSARRAYOFUINT* m_aExampleUINT;
    QADSARRAYOFUDINT* m_aExampleUDINT;
    QADSARRAYOFSTRING* m_aExampleSTRING;
    QADSARRAYOFREAL* m_aExampleREAL;
    QADSARRAYOFLREAL* m_aExampleLREAL;
    QADSARRAYOFINT* m_aExampleINT;
    QADSARRAYOFDINT* m_aExampleDINT;
    QADSARRAYOFBOOL* m_aExampleBOOL;
};

#endif // QADSDIALOG_H
