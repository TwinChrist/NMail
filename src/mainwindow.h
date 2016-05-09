#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "smtp.h"
#include <QtWidgets/QMessageBox>
#include <QFileDialog>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

    enum EMailCfg {
        eCFG_Key ,
        eCFG_Server  ,
        eCFG_Port  ,
        eCFG_Encryption ,
        eCFG_User  ,
        eCFG_Pass ,

        eCFG_Count
    };

    enum EMailEml {
        eEML_Key            = 0,
        eEML_Recipient      = 1,
        eEML_Subject        = 2,
        eEML_Attachments    = 3,
        eEML_Payslip        = 4,
        eEML_Message , // Should be the last one

        eEML_Count ,
        eEML_Count_10 = eEML_Attachments + 1
    };

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void do_Load(QString vFileName);

private slots:
    void do_SendMail();
    void do_SentMessage(QString);
    void do_SetAttachments();

    void do_New();
    void do_Save();
    void do_Load();

    void do_SaveConfig();
    void do_LoadConfig();

    void do_SaveServerFile();
    void do_LoadServerFile(QString vFileName = "",bool vShowNotExist = true);

    void do_About();

    void on_chk_Payslip_toggled(bool vChecked);

    void on_cbb_Attachments_currentIndexChanged(int vIndex = 0);

    void on_btn_Send_clicked();

    void do_DisableDuringSend(bool vDisable);


private:
    Ui::MainWindow  *ui;
    QString         _cmdargFile;
    QStringList     _Files;

    QString         NSDEML_FILENAME;
    QString         NSDEML_KEY;
    QString         NSDEML_VER;
    QString         NSDEML_SEP;
    QString         NSDEML_FILES_SEP;
    int             NSDEML_CFGLineCount;
    QString         NSDPAY_SEP;

    float           _currentVersion;

};

#endif // MAINWINDOW_H
