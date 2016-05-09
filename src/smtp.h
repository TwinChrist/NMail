/*
Copyright (c) 2013 Raivis Strogonovs

http://morf.lv

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.*/

#ifndef SMTP_H
#define SMTP_H


#include <QtNetwork/QAbstractSocket>
#include <QtNetwork/QSslSocket>
#include <QString>
#include <QTextStream>
#include <QDebug>
#include <QtWidgets/QMessageBox>
#include <QByteArray>
#include <QFile>
#include <QFileInfo>
#include <QTextEdit>


class Smtp : public QObject
{
    Q_OBJECT

public:
    Smtp( const QString &user, const QString &pass,
          const QString &host, bool vEncrypted, int port = 465);
    ~Smtp();

    uint sendMail(const QString &vFrom, const QString &vTo,
                   const QString &vSubject, const QString &vBody,
                   QStringList vFiles = QStringList(), QTextEdit *vTextOut = 0);

    struct ResonseStruct {
        uint    code;
        QString desc;
    };

    enum  ReplyCodesEnum {
      eRC_Init = 220,

      eRc_Ok = 250,

      eRc_AuthLogin_GetUserPass = 334,
      eRc_AuthLogin_Accepted = 235,
      eRc_AuthLogin_Failed = 535 ,

      eRC_Data_Start = 354 ,

      eRC_Quit_Ok = 221

    };

    ResonseStruct read(int vTimeout = 30000);
    void write(QString vString, bool toBase64 = false, bool vLogCommands = true, int vTimeout = 30000);

    bool logStatus;
    bool logCommands;
    bool logReplies;
    bool logErrors;
signals:
    void status( const QString &);

private slots:
    void stateChanged(QAbstractSocket::SocketState socketState);
    void errorReceived(QAbstractSocket::SocketError socketError);
    void disconnected();
    void connected();
    void readyRead();

private:
    QTextStream *stream;
    QSslSocket *socket;
    QString from;
    QString rcpt;
    QString user;
    QString pass;
    QString host;
    int port;
    enum states{Tls, HandShake ,Auth,User,Pass,Rcpt,Mail,Data,Init,Body,Quit,Close};
//    int state;

    bool isEncrypted;
    bool isConnected;
    QTextEdit *_Textout;
    void logscreen(int vNumber, QString vString);
    void prompt(QString vString, const QColor vColor = Qt::black);
};
#endif
