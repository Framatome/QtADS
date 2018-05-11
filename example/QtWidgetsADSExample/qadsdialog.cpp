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
#include "qadsdialog.h"
#include "ui_qadsdialog.h"
#include <QADSENUM>
#include <QADSTIME>
#include <QADSUINT>
#include <QADSUDINT>
#include <QADSSTRING>
#include <QADSREAL>
#include <QADSLREAL>
#include <QADSINT>
#include <QADSDINT>
#include <QADSBOOL>
#include <QADSBYTE>
#include <QADSDWORD>
#include <QADSWORD>
#include <QADSSINT>
#include <QADSUSINT>
#include <QADSARRAYOFUINT>
#include <QADSARRAYOFUDINT>
#include <QADSARRAYOFSTRING>
#include <QADSARRAYOFREAL>
#include <QADSARRAYOFLREAL>
#include <QADSARRAYOFINT>
#include <QADSARRAYOFDINT>
#include <QADSARRAYOFBOOL>

#define MOVE_TO_END_OF_DIAGNOSTICS_TAB_DOCUMENT  {\
    QTextCursor cursor = ui->textEdit->textCursor(); \
    cursor.movePosition(QTextCursor::End, QTextCursor::MoveAnchor, 1); \
    }

#define ENTER_DIAGNOSTIC_ERROR(input) {\
    ui->textEdit->setTextColor(Qt::red); \
    MOVE_TO_END_OF_DIAGNOSTICS_TAB_DOCUMENT; \
    ui->textEdit->insertPlainText(QString("%1. %2\n").arg(m_textEditMessageCount++) \
                                                .arg(input->adsErrorString())); \
    ui->textEdit->setTextColor(Qt::black); \
    }

QAdsDialog::QAdsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QAdsDialog),
    m_textEditMessageCount(0),
    m_eExampleEnum(new QADSENUM(this,851,"192.168.1.12.1.1","192.168.1.12","MAIN.eExampleEnum",QADSPLCVariable::ON_DEMAND,0,"EXAMPLE_ENUM")),
    m_eGlobalExampleEnum(new QADSENUM(this,851,"192.168.1.12.1.1","192.168.1.12","GVL.eGlobalExampleEnum",QADSPLCVariable::ON_DEMAND,0,"EXAMPLE_ENUM")),
    m_tPassageOfTime(new QADSTIME(this,851,"192.168.1.12.1.1","192.168.1.12","GVL.tPassageOfTime",QADSPLCVariable::CYCLICAL,2000000)),
    m_uiExample(new QADSUINT(this,851,"192.168.1.12.1.1","192.168.1.12","MAIN.uiExample",QADSPLCVariable::SYNCHRONIZED,0)),
    m_udiExample(new QADSUDINT(this,851,"192.168.1.12.1.1","192.168.1.12","MAIN.udiExample",QADSPLCVariable::SYNCHRONIZED,0)),
    m_sExample(new QADSSTRING(this,851,"192.168.1.12.1.1","192.168.1.12","MAIN.sExample",QADSPLCVariable::SYNCHRONIZED,0)),
    m_rExample(new QADSREAL(this,851,"192.168.1.12.1.1","192.168.1.12","MAIN.rExample",QADSPLCVariable::SYNCHRONIZED,0)),
    m_lrExample(new QADSLREAL(this,851,"192.168.1.12.1.1","192.168.1.12","MAIN.lrExample",QADSPLCVariable::SYNCHRONIZED,0)),
    m_iExample(new QADSINT(this,851,"192.168.1.12.1.1","192.168.1.12","MAIN.iExample",QADSPLCVariable::SYNCHRONIZED,0)),
    m_diExample(new QADSDINT(this,851,"192.168.1.12.1.1","192.168.1.12","MAIN.diExample",QADSPLCVariable::SYNCHRONIZED,0)),
    m_boolExample(new QADSBOOL(this,851,"192.168.1.12.1.1","192.168.1.12","MAIN.boolExample",QADSPLCVariable::SYNCHRONIZED,0)),
    m_bExample(new QADSBYTE(this,851,"192.168.1.12.1.1","192.168.1.12","MAIN.bExample",QADSPLCVariable::SYNCHRONIZED,0)),
    m_dwExample(new QADSDWORD(this,851,"192.168.1.12.1.1","192.168.1.12","MAIN.dwExample",QADSPLCVariable::SYNCHRONIZED,0)),
    m_wExample(new QADSWORD(this,851,"192.168.1.12.1.1","192.168.1.12","MAIN.wExample",QADSPLCVariable::SYNCHRONIZED,0)),
    m_siExample(new QADSSINT(this,851,"192.168.1.12.1.1","192.168.1.12","MAIN.siExample",QADSPLCVariable::SYNCHRONIZED,0)),
    m_usiExample(new QADSUSINT(this,851,"192.168.1.12.1.1","192.168.1.12","MAIN.usiExample",QADSPLCVariable::SYNCHRONIZED,0)),
    m_aExampleUINT(new QADSARRAYOFUINT(this,851,"192.168.1.12.1.1","192.168.1.12","MAIN.aExampleUINT",QADSPLCVariable::ON_DEMAND,0)),
    m_aExampleUDINT(new QADSARRAYOFUDINT(this,851,"192.168.1.12.1.1","192.168.1.12","MAIN.aExampleUDINT",QADSPLCVariable::ON_DEMAND,0)),
    m_aExampleSTRING(new QADSARRAYOFSTRING(this,851,"192.168.1.12.1.1","192.168.1.12","MAIN.aExampleSTRING",QADSPLCVariable::ON_DEMAND,0)),
    m_aExampleREAL(new QADSARRAYOFREAL(this,851,"192.168.1.12.1.1","192.168.1.12","MAIN.aExampleREAL",QADSPLCVariable::ON_DEMAND,0)),
    m_aExampleLREAL(new QADSARRAYOFLREAL(this,851,"192.168.1.12.1.1","192.168.1.12","MAIN.aExampleLREAL",QADSPLCVariable::ON_DEMAND,0)),
    m_aExampleINT(new QADSARRAYOFINT(this,851,"192.168.1.12.1.1","192.168.1.12","MAIN.aExampleINT",QADSPLCVariable::ON_DEMAND,0)),
    m_aExampleDINT(new QADSARRAYOFDINT(this,851,"192.168.1.12.1.1","192.168.1.12","MAIN.aExampleDINT",QADSPLCVariable::ON_DEMAND,0)),
    m_aExampleBOOL(new QADSARRAYOFBOOL(this,851,"192.168.1.12.1.1","192.168.1.12","MAIN.aExampleBOOL",QADSPLCVariable::ON_DEMAND,0))
{
    ui->setupUi(this);

    MOVE_TO_END_OF_DIAGNOSTICS_TAB_DOCUMENT;
    ui->textEdit->insertPlainText(QString("%1. ADS Dll Version: %2\n").arg(m_textEditMessageCount++)
                                  .arg(static_cast<int>(m_eExampleEnum->adsDllVersion())));
    MOVE_TO_END_OF_DIAGNOSTICS_TAB_DOCUMENT;
    ui->textEdit->insertPlainText(QString("%1. ADS Dll Revision: %2\n").arg(m_textEditMessageCount++)
                                  .arg(static_cast<int>(m_eExampleEnum->adsDllRevision())));
    MOVE_TO_END_OF_DIAGNOSTICS_TAB_DOCUMENT;
    ui->textEdit->insertPlainText(QString("%1. ADS Dll Build: %2\n").arg(m_textEditMessageCount++)
                                  .arg(m_eExampleEnum->adsDllBuild()));
    MOVE_TO_END_OF_DIAGNOSTICS_TAB_DOCUMENT;
    ui->textEdit->insertPlainText(QString("%1. AMS Net ID: %2\n").arg(m_textEditMessageCount++)
                                  .arg(m_eExampleEnum->amsNetId()));
    MOVE_TO_END_OF_DIAGNOSTICS_TAB_DOCUMENT;
    ui->textEdit->insertPlainText(QString("%1. ADS Port: %2\n").arg(m_textEditMessageCount++)
                                  .arg(m_eExampleEnum->adsPort()));
    MOVE_TO_END_OF_DIAGNOSTICS_TAB_DOCUMENT;
    ui->textEdit->insertPlainText(QString("%1. ADS Device Name: %2\n").arg(m_textEditMessageCount++)
                                  .arg(m_eExampleEnum->adsDeviceName()));
    MOVE_TO_END_OF_DIAGNOSTICS_TAB_DOCUMENT;
    ui->textEdit->insertPlainText(QString("%1. ADS Device Version: %2\n").arg(m_textEditMessageCount++)
                                  .arg(static_cast<int>(m_eExampleEnum->adsDeviceVersion())));
    MOVE_TO_END_OF_DIAGNOSTICS_TAB_DOCUMENT;
    ui->textEdit->insertPlainText(QString("%1. ADS Device Revision: %2\n").arg(m_textEditMessageCount++)
                                  .arg(static_cast<int>(m_eExampleEnum->adsDeviceRevision())));
    MOVE_TO_END_OF_DIAGNOSTICS_TAB_DOCUMENT;
    ui->textEdit->insertPlainText(QString("%1. ADS Device Build: %2\n").arg(m_textEditMessageCount++)
                                  .arg(static_cast<int>(m_eExampleEnum->adsDeviceBuild())));
    MOVE_TO_END_OF_DIAGNOSTICS_TAB_DOCUMENT;
    ui->textEdit->insertPlainText(QString("%1. ADS PLC State: %2\n").arg(m_textEditMessageCount++)
                                  .arg(m_eExampleEnum->adsStateAsString()));
    if( m_eExampleEnum->adsError() )
    {
        ENTER_DIAGNOSTIC_ERROR(m_eExampleEnum);
    }
    if( m_tPassageOfTime->adsError() )
    {
        ENTER_DIAGNOSTIC_ERROR(m_tPassageOfTime);
    }
    if( m_uiExample->adsError() )
    {
        ENTER_DIAGNOSTIC_ERROR(m_uiExample);
    }
    if( m_udiExample->adsError() )
    {
        ENTER_DIAGNOSTIC_ERROR(m_udiExample);
    }
    if( m_sExample->adsError() )
    {
        ENTER_DIAGNOSTIC_ERROR(m_sExample);
    }
    if( m_rExample->adsError() )
    {
        ENTER_DIAGNOSTIC_ERROR(m_rExample);
    }
    if( m_lrExample->adsError() )
    {
        ENTER_DIAGNOSTIC_ERROR(m_lrExample);
    }
    if( m_iExample->adsError() )
    {
        ENTER_DIAGNOSTIC_ERROR(m_iExample);
    }
    if( m_diExample->adsError() )
    {
        ENTER_DIAGNOSTIC_ERROR(m_diExample);
    }
    if( m_boolExample->adsError() )
    {
        ENTER_DIAGNOSTIC_ERROR(m_boolExample);
    }
    if( m_bExample->adsError() )
    {
        ENTER_DIAGNOSTIC_ERROR(m_bExample);
    }
    if( m_dwExample->adsError() )
    {
        ENTER_DIAGNOSTIC_ERROR(m_dwExample);
    }
    if( m_wExample->adsError() )
    {
        ENTER_DIAGNOSTIC_ERROR(m_wExample);
    }
    if( m_siExample->adsError() )
    {
        ENTER_DIAGNOSTIC_ERROR(m_siExample);
    }
    if( m_usiExample->adsError() )
    {
        ENTER_DIAGNOSTIC_ERROR(m_usiExample);
    }
    if( m_aExampleUINT->adsError() )
    {
        ENTER_DIAGNOSTIC_ERROR(m_aExampleUINT);
    }
    if( m_aExampleUDINT->adsError() )
    {
        ENTER_DIAGNOSTIC_ERROR(m_aExampleUDINT);
    }
    if( m_aExampleSTRING->adsError() )
    {
        ENTER_DIAGNOSTIC_ERROR(m_aExampleSTRING);
    }
    if( m_aExampleREAL->adsError() )
    {
        ENTER_DIAGNOSTIC_ERROR(m_aExampleREAL);
    }
    if( m_aExampleLREAL->adsError() )
    {
        ENTER_DIAGNOSTIC_ERROR(m_aExampleLREAL);
    }
    if( m_aExampleINT->adsError() )
    {
        ENTER_DIAGNOSTIC_ERROR(m_aExampleINT);
    }
    if( m_aExampleDINT->adsError() )
    {
        ENTER_DIAGNOSTIC_ERROR(m_aExampleDINT);
    }
    if( m_aExampleBOOL->adsError() )
    {
        ENTER_DIAGNOSTIC_ERROR(m_aExampleBOOL);
    }

    switch(m_eExampleEnum->value())
    {
    case 0:
        ui->radioButtonExample0->setChecked(true);
        break;
    case 1:
        ui->radioButtonExample1->setChecked(true);
        break;
    case 2:
        ui->radioButtonExample2->setChecked(true);
        break;
    default:
        break;
    }

    ui->spinBoxUIExample->setMinimum(QADSUINT::minValue());
    ui->spinBoxUIExample->setMaximum(QADSUINT::maxValue());
    ui->spinBoxUIExample->setValue(m_uiExample->value());
    ui->doubleSpinBoxUDIExample->setMinimum(QADSUDINT::minValue());
    ui->doubleSpinBoxUDIExample->setMaximum(QADSUDINT::maxValue());
    ui->doubleSpinBoxUDIExample->setValue(m_udiExample->value());
    ui->lineEditSExample->setText(m_sExample->value());
    ui->doubleSpinBoxRExample->setMinimum(QADSREAL::minValue());
    ui->doubleSpinBoxRExample->setMaximum(QADSREAL::maxValue());
    ui->doubleSpinBoxRExample->setValue(m_rExample->value());
    ui->doubleSpinBoxLRExample->setMinimum(QADSLREAL::minValue());
    ui->doubleSpinBoxLRExample->setMaximum(QADSLREAL::maxValue());
    ui->doubleSpinBoxLRExample->setValue(m_lrExample->value());
    ui->spinBoxIExample->setMinimum(QADSINT::minValue());
    ui->spinBoxIExample->setMaximum(QADSINT::maxValue());
    ui->spinBoxIExample->setValue(m_iExample->value());
    ui->spinBoxDIExample->setMinimum(QADSDINT::minValue());
    ui->spinBoxDIExample->setMaximum(QADSDINT::maxValue());
    ui->spinBoxDIExample->setValue(m_diExample->value());
    ui->checkBoxBoolExample->setChecked(m_boolExample->value());
    ui->spinBoxBExample->setMinimum(QADSBYTE::minValue());
    ui->spinBoxBExample->setMaximum(QADSBYTE::maxValue());
    ui->spinBoxBExample->setValue(m_bExample->value());
    ui->doubleSpinBoxDWExample->setMinimum(QADSDWORD::minValue());
    ui->doubleSpinBoxDWExample->setMaximum(QADSDWORD::maxValue());
    ui->doubleSpinBoxDWExample->setValue(m_dwExample->value());
    ui->spinBoxWExample->setMinimum(QADSWORD::minValue());
    ui->spinBoxWExample->setMaximum(QADSWORD::maxValue());
    ui->spinBoxWExample->setValue(m_wExample->value());
    ui->spinBoxSIExample->setMinimum(QADSSINT::minValue());
    ui->spinBoxSIExample->setMaximum(QADSSINT::maxValue());
    ui->spinBoxSIExample->setValue(m_siExample->value());
    ui->spinBoxUSIExample->setMinimum(QADSUSINT::minValue());
    ui->spinBoxUSIExample->setMaximum(QADSUSINT::maxValue());
    ui->spinBoxUSIExample->setValue(m_usiExample->value());

    // Monitor state changes (only need to connect this signal once).
    connect(m_eExampleEnum, SIGNAL(adsStateChanged()),
            this, SLOT(catchADSStateChanged()));
    connect(m_eExampleEnum, SIGNAL(amsEventChanged()),
            this, SLOT(catchAMSEventChanged()));

    // Connect the error signal to the slot on this class.
    connect(m_eExampleEnum, SIGNAL(adsErrorChanged()),
            this, SLOT(catchADSErrorChanged()));
    connect(m_tPassageOfTime, SIGNAL(adsErrorChanged()),
            this, SLOT(catchADSErrorChanged()));
    connect(m_uiExample, SIGNAL(adsErrorChanged()),
            this, SLOT(catchADSErrorChanged()));
    connect(m_udiExample, SIGNAL(adsErrorChanged()),
            this, SLOT(catchADSErrorChanged()));
    connect(m_sExample, SIGNAL(adsErrorChanged()),
            this, SLOT(catchADSErrorChanged()));
    connect(m_rExample, SIGNAL(adsErrorChanged()),
            this, SLOT(catchADSErrorChanged()));
    connect(m_lrExample, SIGNAL(adsErrorChanged()),
            this, SLOT(catchADSErrorChanged()));
    connect(m_iExample, SIGNAL(adsErrorChanged()),
            this, SLOT(catchADSErrorChanged()));
    connect(m_diExample, SIGNAL(adsErrorChanged()),
            this, SLOT(catchADSErrorChanged()));
    connect(m_boolExample, SIGNAL(adsErrorChanged()),
            this, SLOT(catchADSErrorChanged()));
    connect(m_bExample, SIGNAL(adsErrorChanged()),
            this, SLOT(catchADSErrorChanged()));
    connect(m_dwExample, SIGNAL(adsErrorChanged()),
            this, SLOT(catchADSErrorChanged()));
    connect(m_wExample, SIGNAL(adsErrorChanged()),
            this, SLOT(catchADSErrorChanged()));
    connect(m_siExample, SIGNAL(adsErrorChanged()),
            this, SLOT(catchADSErrorChanged()));
    connect(m_usiExample, SIGNAL(adsErrorChanged()),
            this, SLOT(catchADSErrorChanged()));
    connect(m_aExampleUINT, SIGNAL(adsErrorChanged()),
            this, SLOT(catchADSErrorChanged()));
    connect(m_aExampleUDINT, SIGNAL(adsErrorChanged()),
            this, SLOT(catchADSErrorChanged()));
    connect(m_aExampleSTRING, SIGNAL(adsErrorChanged()),
            this, SLOT(catchADSErrorChanged()));
    connect(m_aExampleREAL, SIGNAL(adsErrorChanged()),
            this, SLOT(catchADSErrorChanged()));
    connect(m_aExampleLREAL, SIGNAL(adsErrorChanged()),
            this, SLOT(catchADSErrorChanged()));
    connect(m_aExampleINT, SIGNAL(adsErrorChanged()),
            this, SLOT(catchADSErrorChanged()));
    connect(m_aExampleDINT, SIGNAL(adsErrorChanged()),
            this, SLOT(catchADSErrorChanged()));
    connect(m_aExampleBOOL, SIGNAL(adsErrorChanged()),
            this, SLOT(catchADSErrorChanged()));

    // Variable connections.
    connect(m_tPassageOfTime, SIGNAL(valueChanged()),
            this, SLOT(tPassageOfTimeChanged()));
    connect(m_uiExample, SIGNAL(valueChanged()),
            this, SLOT(uiExampleChanged()));
    connect(m_udiExample, SIGNAL(valueChanged()),
            this, SLOT(udiExampleChanged()));
    connect(m_sExample, SIGNAL(valueChanged()),
            this, SLOT(sExampleChanged()));
    connect(m_rExample, SIGNAL(valueChanged()),
            this, SLOT(rExampleChanged()));
    connect(m_lrExample, SIGNAL(valueChanged()),
            this, SLOT(lrExampleChanged()));
    connect(m_iExample, SIGNAL(valueChanged()),
            this, SLOT(iExampleChanged()));
    connect(m_diExample, SIGNAL(valueChanged()),
            this, SLOT(diExampleChanged()));
    connect(m_boolExample, SIGNAL(valueChanged()),
            this, SLOT(boolExampleChanged()));
    connect(m_bExample, SIGNAL(valueChanged()),
            this, SLOT(bExampleChanged()));
    connect(m_dwExample, SIGNAL(valueChanged()),
            this, SLOT(dwExampleChanged()));
    connect(m_wExample, SIGNAL(valueChanged()),
            this, SLOT(wExampleChanged()));
    connect(m_siExample, SIGNAL(valueChanged()),
            this, SLOT(siExampleChanged()));
    connect(m_usiExample, SIGNAL(valueChanged()),
            this, SLOT(usiExampleChanged()));

}

QAdsDialog::~QAdsDialog()
{
    delete ui;
}

void QAdsDialog::catchADSErrorChanged()
{
    QADSObject *temp
#ifndef Q_WS_WINCE
            = dynamic_cast<QADSObject *>(sender());
#else
            = static_cast<QADSObject *>(sender());
#endif
    ENTER_DIAGNOSTIC_ERROR(temp);
}

void QAdsDialog::catchADSStateChanged()
{
    QADSObject *temp
#ifndef Q_WS_WINCE
            = dynamic_cast<QADSObject *>(sender());
#else
            = static_cast<QADSObject *>(sender());
#endif
    MOVE_TO_END_OF_DIAGNOSTICS_TAB_DOCUMENT;
    ui->textEdit->insertPlainText(QString("%1. ADS PLC State Changed to: %2\n").arg(m_textEditMessageCount++)
                                  .arg(temp->adsStateAsString()));
}

void QAdsDialog::catchAMSEventChanged()
{
    QADSObject *temp
#ifndef Q_WS_WINCE
            = dynamic_cast<QADSObject *>(sender());
#else
            = static_cast<QADSObject *>(sender());
#endif
    MOVE_TO_END_OF_DIAGNOSTICS_TAB_DOCUMENT;
    ui->textEdit->insertPlainText(QString("%1. AMS Router State Changed to: %2\n").arg(m_textEditMessageCount++)
                                  .arg(temp->amsEventAsString()));
}

void QAdsDialog::on_radioButtonExample0_toggled(bool checked)
{
    if (checked)
    {
        if (m_eGlobalExampleEnum->value() != 0)
        {
            // Play a little swapping game.
            const quint16 aUINT = m_aExampleUINT->value(1);
            m_aExampleUINT->setValue(m_aExampleUINT->value(2), 1);
            m_aExampleUINT->setValue(aUINT, 2);
            const quint32 aUDINT = m_aExampleUDINT->value(1);
            m_aExampleUDINT->setValue(m_aExampleUDINT->value(2), 1);
            m_aExampleUDINT->setValue(aUDINT, 2);
            const QString aSTRING = m_aExampleSTRING->value(1);
            m_aExampleSTRING->setValue(m_aExampleSTRING->value(2), 1);
            m_aExampleSTRING->setValue(aSTRING, 2);
            const float aREAL = m_aExampleREAL->value(1);
            m_aExampleREAL->setValue(m_aExampleREAL->value(2), 1);
            m_aExampleREAL->setValue(aREAL, 2);
            const double aLREAL = m_aExampleLREAL->value(1);
            m_aExampleLREAL->setValue(m_aExampleLREAL->value(2), 1);
            m_aExampleLREAL->setValue(aLREAL, 2);
            const qint16 aINT = m_aExampleINT->value(1);
            m_aExampleINT->setValue(m_aExampleINT->value(2), 1);
            m_aExampleINT->setValue(aINT, 2);
            const qint32 aDINT = m_aExampleDINT->value(1);
            m_aExampleDINT->setValue(m_aExampleDINT->value(2), 1);
            m_aExampleDINT->setValue(aDINT, 2);
            const bool aBOOL = m_aExampleBOOL->value(1);
            m_aExampleBOOL->setValue(m_aExampleBOOL->value(2), 1);
            m_aExampleBOOL->setValue(aBOOL, 2);
            m_eExampleEnum->setValue(0);
        }
    }
}

void QAdsDialog::on_radioButtonExample1_toggled(bool checked)
{
    if (checked)
    {
        if (m_eGlobalExampleEnum->value() != 1)
        {
            // Play a little swapping game.
            const quint16 aUINT = m_aExampleUINT->value(0);
            m_aExampleUINT->setValue(m_aExampleUINT->value(2), 0);
            m_aExampleUINT->setValue(aUINT, 2);
            const quint32 aUDINT = m_aExampleUDINT->value(0);
            m_aExampleUDINT->setValue(m_aExampleUDINT->value(2), 0);
            m_aExampleUDINT->setValue(aUDINT, 2);
            const QString aSTRING = m_aExampleSTRING->value(0);
            m_aExampleSTRING->setValue(m_aExampleSTRING->value(2), 0);
            m_aExampleSTRING->setValue(aSTRING, 2);
            const float aREAL = m_aExampleREAL->value(0);
            m_aExampleREAL->setValue(m_aExampleREAL->value(2), 0);
            m_aExampleREAL->setValue(aREAL, 2);
            const double aLREAL = m_aExampleLREAL->value(0);
            m_aExampleLREAL->setValue(m_aExampleLREAL->value(2), 0);
            m_aExampleLREAL->setValue(aLREAL, 2);
            const qint16 aINT = m_aExampleINT->value(0);
            m_aExampleINT->setValue(m_aExampleINT->value(2), 0);
            m_aExampleINT->setValue(aINT, 2);
            const qint32 aDINT = m_aExampleDINT->value(0);
            m_aExampleDINT->setValue(m_aExampleDINT->value(2), 0);
            m_aExampleDINT->setValue(aDINT, 2);
            const bool aBOOL = m_aExampleBOOL->value(0);
            m_aExampleBOOL->setValue(m_aExampleBOOL->value(2), 0);
            m_aExampleBOOL->setValue(aBOOL, 2);
            m_eExampleEnum->setValue(1);
        }
    }
}

void QAdsDialog::on_radioButtonExample2_toggled(bool checked)
{
    if (checked)
    {
        if (m_eGlobalExampleEnum->value() != 2)
        {
            // Play a little swapping game.
            const quint16 aUINT = m_aExampleUINT->value(0);
            m_aExampleUINT->setValue(m_aExampleUINT->value(1), 0);
            m_aExampleUINT->setValue(aUINT, 1);
            const quint32 aUDINT = m_aExampleUDINT->value(0);
            m_aExampleUDINT->setValue(m_aExampleUDINT->value(1), 0);
            m_aExampleUDINT->setValue(aUDINT, 1);
            const QString aSTRING = m_aExampleSTRING->value(0);
            m_aExampleSTRING->setValue(m_aExampleSTRING->value(1), 0);
            m_aExampleSTRING->setValue(aSTRING, 1);
            const float aREAL = m_aExampleREAL->value(0);
            m_aExampleREAL->setValue(m_aExampleREAL->value(1), 0);
            m_aExampleREAL->setValue(aREAL, 1);
            const double aLREAL = m_aExampleLREAL->value(0);
            m_aExampleLREAL->setValue(m_aExampleLREAL->value(1), 0);
            m_aExampleLREAL->setValue(aLREAL, 1);
            const qint16 aINT = m_aExampleINT->value(0);
            m_aExampleINT->setValue(m_aExampleINT->value(1), 0);
            m_aExampleINT->setValue(aINT, 1);
            const qint32 aDINT = m_aExampleDINT->value(0);
            m_aExampleDINT->setValue(m_aExampleDINT->value(1), 0);
            m_aExampleDINT->setValue(aDINT, 1);
            const bool aBOOL = m_aExampleBOOL->value(0);
            m_aExampleBOOL->setValue(m_aExampleBOOL->value(1), 0);
            m_aExampleBOOL->setValue(aBOOL, 1);
            m_eExampleEnum->setValue(2);
        }
    }
}

void QAdsDialog::tPassageOfTimeChanged()
{
    ui->timeEdit->setTime(m_tPassageOfTime->value());
}

void QAdsDialog::uiExampleChanged()
{
    ui->spinBoxUIExample->setValue(m_uiExample->value());
}

void QAdsDialog::udiExampleChanged()
{
    ui->doubleSpinBoxUDIExample->setValue(m_udiExample->value());
}

void QAdsDialog::sExampleChanged()
{
    ui->lineEditSExample->setText(m_sExample->value());
}

void QAdsDialog::rExampleChanged()
{
    ui->doubleSpinBoxRExample->setValue(m_rExample->value());
}

void QAdsDialog::lrExampleChanged()
{
    ui->doubleSpinBoxLRExample->setValue(m_lrExample->value());
}

void QAdsDialog::iExampleChanged()
{
    ui->spinBoxIExample->setValue(m_iExample->value());
}

void QAdsDialog::diExampleChanged()
{
    ui->spinBoxDIExample->setValue(m_diExample->value());
}

void QAdsDialog::boolExampleChanged()
{
    ui->checkBoxBoolExample->setChecked(m_boolExample->value());
}

void QAdsDialog::bExampleChanged()
{
    ui->spinBoxBExample->setValue(m_bExample->value());
}

void QAdsDialog::dwExampleChanged()
{
    ui->doubleSpinBoxDWExample->setValue(m_dwExample->value());
}

void QAdsDialog::wExampleChanged()
{
    ui->spinBoxWExample->setValue(m_wExample->value());
}

void QAdsDialog::siExampleChanged()
{
    ui->spinBoxSIExample->setValue(m_siExample->value());
}

void QAdsDialog::usiExampleChanged()
{
    ui->spinBoxUSIExample->setValue(m_uiExample->value());
}
