#include "main.h"

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFontDatabase>

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  NSDEML_FILENAME = "NSDMailSend.cfg";
  NSDEML_KEY = "NSD::eml";
  NSDEML_VER = "1.1";
  NSDEML_SEP = "\n";
  NSDEML_CFGLineCount = 5;
  NSDEML_FILES_SEP = ";;";
  NSDPAY_SEP = "#";

  _currentVersion = NSDEML_VER.toFloat();

  ui->setupUi(this);
  ui->te_Output->setVisible(false);
  ui->lbl_Wait->setVisible(false);
  ui->chk_Payslip->setChecked(true);
  connect(ui->btn_Browse, SIGNAL(clicked()), this, SLOT(do_SetAttachments()));

  connect(ui->act_About, SIGNAL(triggered()), this, SLOT(do_About()));

  connect(ui->act_New, SIGNAL(triggered()), this, SLOT(do_New()));
  connect(ui->act_Save, SIGNAL(triggered()), this, SLOT(do_Save()));
  connect(ui->act_Open, SIGNAL(triggered()), this, SLOT(do_Load()));

  connect(ui->act_Load_Mail_Sender, SIGNAL(triggered()), this, SLOT(do_LoadConfig()));
  connect(ui->act_Save_Mail_Sender, SIGNAL(triggered()), this, SLOT(do_SaveConfig()));

  do_LoadServerFile(qApp->applicationDirPath() + "\\" + NSDEML_FILENAME , false);
}

void MainWindow::do_SetAttachments()
{
  _Files.clear();
  ui->cbb_Attachments->clear();

  QFileDialog dialog(this);
  dialog.setDirectory(QDir::homePath());
  dialog.setFileMode(QFileDialog::ExistingFiles);
  if(ui->chk_Payslip->isChecked()) {
    dialog.setNameFilters(QStringList() << "PDF (*.pdf)");
  }

  if (dialog.exec())
    _Files = dialog.selectedFiles();

  ui->cbb_Attachments->addItems(_Files);
}

void MainWindow::do_New()
{
  ui->le_Recipient->setText("");
  if(ui->chk_Payslip->isChecked()) {
    ui->le_Subject->setText("Payslip");
  } else {
    ui->le_Subject->clear();
  }
  ui->cbb_Attachments->clear();
  ui->txt_Message->setText("");
  ui->le_Recipient->setFocus();
}

void MainWindow::do_Save()
{
  QString mFileName = QFileDialog::getSaveFileName(this, tr("Save File"),
                                                   QDir::homePath() + "\\" + QLocale::c().toString(QDate().currentDate(),"yyyyMMdd") + "-" + ui->le_Subject->text() + ".eml" ,
                                                   tr("NSDMailSend Mail file (*.eml)"));
  if(mFileName == "") return;
  QFile mFile(mFileName);
  QTextStream out;

  if (!mFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
    QMessageBox::warning( 0, tr( "NSD Mail Send" ), tr( "Couldn't open file for writing!\n" ) );
    goto l_Out;
  }
  out.setDevice(&mFile);
  out << NSDEML_KEY << "::" << NSDEML_VER << NSDEML_SEP
      << ui->le_Recipient->text() << NSDEML_SEP
      << ui->le_Subject->text() << NSDEML_SEP
      << _Files.join(NSDEML_FILES_SEP)  << NSDEML_SEP
      << (ui->chk_Payslip->isChecked() ? "1" : "2") << NSDEML_SEP
      << ui->txt_Message->toPlainText() << NSDEML_SEP;
  out.flush();

l_Out:
  mFile.close();
}

void MainWindow::do_Load()
{
  QString mContent;
  QStringList mLines;
  QStringList mMessage;
  QString mFileName;

  if(ui->chk_Payslip->isChecked()) {
    do_SetAttachments();
    return;
  }

  if(_cmdargFile.trimmed().isEmpty()) {
    mFileName = QFileDialog::getOpenFileName(this, tr("Open File"),
                                             QDir::homePath() ,
                                             tr("NSDMailSend Mail file (*.eml)"));
  } else {
    mFileName = _cmdargFile;
  }
  _cmdargFile = "";

  if(mFileName == "") return;

  QFile mFile(mFileName);
  if(!mFile.exists()){
    QMessageBox::warning( 0, tr( "NSD Mail Send" ), tr( "File doesn't exists!\n" ) );
    goto l_Out;
  }

  if (!mFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
    QMessageBox::warning( 0, tr( "NSD Mail Send" ), tr( "Couldn't open file for reading!\n" ) );
    goto l_Out;
  }

  mContent = mFile.readAll();
  mLines = mContent.split(NSDEML_SEP);

  if(mLines[eCFG_Key].left(NSDEML_KEY.length()) != NSDEML_KEY) {
    QMessageBox::warning( 0, tr( "NSD Mail Send" ), tr( "This file is not a config file\n" ) );
    goto l_Out;
  } else {
    if (mLines[eCFG_Key].right(NSDEML_VER.length()) == NSDEML_VER) { // Current Version
      if(mLines.count() < eEML_Count) {
        QMessageBox::warning( 0, tr( "NSD Mail Send" ), tr( "Config file structure error\n" ) );
        goto l_Out;
      }
    } else {
      if (mLines[eCFG_Key] == "NSD::eml::1.0") {
        if(mLines.count() < eEML_Count_10) {
          QMessageBox::warning( 0, tr( "NSD Mail Send" ), tr( "Config file structure error\n" ) );
          goto l_Out;
        } else {
          _currentVersion = 1.0;
        }
      } else {
        // Check another version
      }
    }
  }
  if(_currentVersion >= 1.1) { // After this Version added option to save payslip value.
    ui->chk_Payslip->setChecked(mLines[eEML_Payslip] == "1");
  }
  _Files = mLines[eEML_Attachments].split(NSDEML_FILES_SEP);
  ui->le_Recipient->setText(mLines[eEML_Recipient]);
  ui->le_Subject->setText(mLines[eEML_Subject]);
  ui->cbb_Attachments->clear();
  ui->cbb_Attachments->addItems(_Files);
  ui->txt_Message->clear();
  for(int i = eEML_Message;i < mLines.count();i++) {
    mMessage.append(mLines[i]);
  }
  ui->txt_Message->setPlainText(mMessage.join(NSDEML_SEP));

l_Out:
  mFile.close();
}

void MainWindow::do_SaveConfig()
{
  do_SaveServerFile();
}

void MainWindow::do_LoadConfig()
{
  do_LoadServerFile();
}

void MainWindow::do_SaveServerFile()
{
  QString mFileName = QFileDialog::getSaveFileName(this, tr("Save File"),
                                                   qApp->applicationDirPath() + "\\" + NSDEML_FILENAME ,
                                                   tr("NSDMailSend Mail Server Config (*.cfg)"));
  if(mFileName == "") return;
  QFile mFile(mFileName);
  QTextStream out;

  if (!mFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
    QMessageBox::warning( 0, tr( "NSD Mail Send" ), tr( "Couldn't open file for writing!\n" ) );
    goto l_Out;
  }
  out.setDevice(&mFile);
  out << NSDEML_KEY << "::" << NSDEML_VER << NSDEML_SEP
      << ui->le_Server->text() << NSDEML_SEP
      << ui->le_Port->text() << NSDEML_SEP
      << QString("%1").arg(ui->chk_Encrypted->isChecked()?1:2) << NSDEML_SEP
      << ui->le_User->text() << NSDEML_SEP
      << ui->le_Pass->text() << NSDEML_SEP;
  out.flush();
  ui->btn_Sender->setText(ui->le_User->text());

l_Out:
  mFile.close();
}

void MainWindow::do_LoadServerFile(QString vFileName,bool vShowNotExist)
{
  QString mContent;
  QStringList mLines;
  QString mFileName = vFileName;
  if(mFileName == "") {
    mFileName = QFileDialog::getOpenFileName(this, tr("Open File"),
                                             qApp->applicationDirPath() + "\\" + NSDEML_FILENAME ,
                                             tr("NSDMailSend Mail Server Config (*.cfg)"));
  }
  if(mFileName == "") return;
  QFile mFile(mFileName);
  if(!mFile.exists()){
    if(vShowNotExist){
      QMessageBox::warning( 0, tr( "NSD Mail Send" ), tr( "File doesn't exists!\n" ) );
    }
    goto l_Out;
  }

  if (!mFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
    QMessageBox::warning( 0, tr( "NSD Mail Send" ), tr( "Couldn't open file for reading!\n" ) );
    goto l_Out;
  }

  mContent = mFile.readAll();
  mLines = mContent.split(NSDEML_SEP);
  if(mLines.count() < eCFG_Count) {
    QMessageBox::warning( 0, tr( "NSD Mail Send" ), tr( "Config file structure error\n" ) );
    goto l_Out;
  } else {
    if(mLines[eCFG_Key].left(NSDEML_KEY.length()) != NSDEML_KEY) {
      QMessageBox::warning( 0, tr( "NSD Mail Send" ), tr( "This file is not a config file\n" ) );
      goto l_Out;
    }
  }

  ui->le_Server->setText(mLines[eCFG_Server]);
  ui->le_Port->setText(mLines[eCFG_Port]);
  ui->chk_Encrypted->setChecked(mLines[eCFG_Encryption].toInt() == 1?true:false);
  ui->le_User->setText(mLines[eCFG_User]);
  ui->le_Pass->setText(mLines[eCFG_Pass]);

  ui->btn_Sender->setChecked(false);
  ui->grp_Sender->setVisible(false);
  ui->btn_Sender->setText(mLines[eCFG_User]);
l_Out:
  mFile.close();

}

void MainWindow::do_About()
{
  QMessageBox dlg;
  dlg.setWindowTitle("NSD Mail Send");
  QFont mFont("B Badr" , 13);
  //  mFont.setBold(true);
  dlg.setFont(mFont);
  dlg.setTextFormat(Qt::RichText);
  dlg.setText(
        "<p align=\"right\">توسعه نرم افزارهای هوشمند نوآور</p>"
        "<a href=\"http:\\www.nsd-corp.com\" target=\"_top\">http://www.nsd-corp.com</a><br>"
        "Version : " NSDUNIQUEVER_STR
        );
  dlg.exec();
}


void MainWindow::do_SendMail()
{

  if(ui->le_Recipient->text().isEmpty()) {
    if(ui->te_Output->isVisible()) {
      ui->te_Output->setTextColor(Qt::red);
      ui->te_Output->append(ui->cbb_Attachments->currentText());
      ui->te_Output->append(tr( "Recepient Is Empty!" ));
      ui->te_Output->setTextColor(Qt::black);
      return;
    } else {
      QMessageBox::warning(this,"Send Mail","Recepient Is Empty!");
      return;
    }
  }

  ui->te_Output->append(QString("=").repeated(45)); // ==== BEGIN
  ui->te_Output->setVisible(true);
  ui->lbl_Wait->setVisible(true);

  Smtp* smtp = new Smtp(ui->le_User->text(), ui->le_Pass->text(), ui->le_Server->text() , ui->chk_Encrypted->isChecked(), ui->le_Port->text().toInt() );
  connect(smtp, SIGNAL(status(QString)), this, SLOT(do_SentMessage(QString)));
  // smtp->sendMail will check the existance of files.
  if(ui->chk_Payslip->isChecked()) {
    smtp->sendMail(ui->le_User->text(), ui->le_Recipient->text() , ui->le_Subject->text(),ui->txt_Message->toPlainText(),
                   QStringList(ui->cbb_Attachments->currentText()),
                   ui->te_Output);
  } else {
    ui->te_Output->setText("");
    if( !_Files.isEmpty() ) {
      smtp->sendMail(ui->le_User->text(), ui->le_Recipient->text() , ui->le_Subject->text(),ui->txt_Message->toPlainText(),
                     _Files,
                     ui->te_Output );
    } else {
      smtp->sendMail(ui->le_User->text(), ui->le_Recipient->text() , ui->le_Subject->text(),ui->txt_Message->toPlainText(),
                     QStringList(),
                     ui->te_Output);
    }
  }
  ui->te_Output->append(QString("=").repeated(45)); // ==== END

}

void MainWindow::do_SentMessage(QString status)
{
  if(status == "Message sent") {
    ui->te_Output->append(tr("Message sent!\n"));
  }
}

MainWindow::~MainWindow()
{
  delete ui;
}

void MainWindow::do_Load(QString vFileName)
{
  _cmdargFile = vFileName;
  do_Load();
}

void MainWindow::on_chk_Payslip_toggled(bool vChecked)
{
  ui->le_Recipient->setEnabled(!vChecked);
//  ui->le_Subject->setEnabled(!vChecked);
  ui->txt_Message->setEnabled(!vChecked);
  if(vChecked) {
    ui->le_Subject->setText("Payslip");
    emit on_cbb_Attachments_currentIndexChanged();
  } else {
    ui->le_Subject->clear();
  }
}

void MainWindow::on_cbb_Attachments_currentIndexChanged(int vIndex)
{
  Q_UNUSED(vIndex);
  if(ui->chk_Payslip->isChecked()) {
    QString mFilename = QFileInfo(ui->cbb_Attachments->currentText()).fileName().replace(".pdf","",Qt::CaseInsensitive);
    QStringList mSections = mFilename.split(NSDPAY_SEP);
    if(mSections.count()!= 3) return;
    ui->le_Recipient->setText(mSections[1].trimmed());
    ui->txt_Message->setText("1 File Attached : " + mFilename);
    ui->txt_Message->append("Personnel Code : " + mSections[0]);
    ui->txt_Message->append("Personnel Name : " + mSections[2]);
  }
}

void MainWindow::on_btn_Send_clicked()
{
  do_DisableDuringSend(true);
  ui->te_Output->append("<<<<<<<<<<<<<<<<<<< Begin >>>>>>>>>>>>>>>>>>>"); // ==== BEGIN
  if(ui->chk_Payslip->isChecked()) {
    for(int i = 0;i<ui->cbb_Attachments->count();i++) {
      ui->cbb_Attachments->setCurrentIndex(i);
      do_SendMail();
    }
  } else {
    do_SendMail();
  }
  ui->te_Output->append("<<<<<<<<<<<<<<<<<<<  End  >>>>>>>>>>>>>>>>>>>"); // ==== END
  do_DisableDuringSend(false);

}

void MainWindow::do_DisableDuringSend(bool vDisable)
{
  ui->lbl_Wait->setVisible(vDisable);

  ui->btn_Load_Mail_Sender->setEnabled(!vDisable);
  ui->btn_Save_Mail_Sender->setEnabled(!vDisable);

  ui->btn_New->setEnabled(!vDisable);
  ui->btn_Open->setEnabled(!vDisable);
  ui->btn_Save->setEnabled(!vDisable);
  ui->btn_Send->setEnabled(!vDisable);
  ui->btn_Exit->setEnabled(!vDisable);

  ui->cbb_Attachments->setEnabled(!vDisable);
  ui->btn_Browse->setEnabled(!vDisable);
  ui->btn_Sender->setEnabled(!vDisable);

  ui->mnu_File->setEnabled(!vDisable);
  QApplication::processEvents();
}

