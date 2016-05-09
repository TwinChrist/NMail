/*
Copyright (c) 2013 Raivis Strogonovs

http://morf.lv

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.*/

/* smtp replies description
 *   connect
  HELO
  250	Requested mail action okay, completed
  500	Syntax error, command unrecognised
  501	Syntax error in parameters or arguments
  504	Command parameter not implemented
  521	<domain> does not accept mail [rfc1846]
  421	<domain> Service not available, closing transmission channel
  EHLO
  250	Requested mail action okay, completed
  550	Not implemented
  500	Syntax error, command unrecognised
  501	Syntax error in parameters or arguments
  504	Command parameter not implemented
  421	<domain> Service not available, closing transmission channel
  MAIL
  250	Requested mail action okay, completed
  552	Requested mail action aborted: exceeded storage allocation
  451	Requested action aborted: local error in processing
  452	Requested action not taken: insufficient system storage
  500	Syntax error, command unrecognised
  501	Syntax error in parameters or arguments
  421	<domain> Service not available, closing transmission channel
  RCPT
  250	Requested mail action okay, completed
  251	User not local; will forward to <forward-path>
  550	Requested action not taken: mailbox unavailable
  551	User not local; please try <forward-path>
  552	Requested mail action aborted: exceeded storage allocation
  553	Requested action not taken: mailbox name not allowed
  450	Requested mail action not taken: mailbox unavailable
  451	Requested action aborted: local error in processing
  452	Requested action not taken: insufficient system storage
  500	Syntax error, command unrecognised
  501	Syntax error in parameters or arguments
  503	Bad sequence of commands
  521	<domain> does not accept mail [rfc1846]
  421	<domain> Service not available, closing transmission channel
  DATA
  354	Start mail input; end with <CRLF>.<CRLF>
  451	Requested action aborted: local error in processing
  554	Transaction failed
  500	Syntax error, command unrecognised
  501	Syntax error in parameters or arguments
  503	Bad sequence of commands
  421	<domain> Service not available, closing transmission channel
  received data
  250	Requested mail action okay, completed
  552	Requested mail action aborted: exceeded storage allocation
  554	Transaction failed
  451	Requested action aborted: local error in processing
  452	Requested action not taken: insufficient system storage
  RSET
  200	(nonstandard success response, see rfc876)
  250	Requested mail action okay, completed
  500	Syntax error, command unrecognised
  501	Syntax error in parameters or arguments
  504	Command parameter not implemented
  421	<domain> Service not available, closing transmission channel
  SEND
  250	Requested mail action okay, completed
  552	Requested mail action aborted: exceeded storage allocation
  451	Requested action aborted: local error in processing
  452	Requested action not taken: insufficient system storage
  500	Syntax error, command unrecognised
  501	Syntax error in parameters or arguments
  502	Command not implemented
  421	<domain> Service not available, closing transmission channel
  SOML
  250	Requested mail action okay, completed
  552	Requested mail action aborted: exceeded storage allocation
  451	Requested action aborted: local error in processing
  452	Requested action not taken: insufficient system storage
  500	Syntax error, command unrecognised
  501	Syntax error in parameters or arguments
  502	Command not implemented
  421	<domain> Service not available, closing transmission channel
  SAML
  250	Requested mail action okay, completed
  552	Requested mail action aborted: exceeded storage allocation
  451	Requested action aborted: local error in processing
  452	Requested action not taken: insufficient system storage
  500	Syntax error, command unrecognised
  501	Syntax error in parameters or arguments
  502	Command not implemented
  421	<domain> Service not available, closing transmission channel
  VRFY
  250	Requested mail action okay, completed
  251	User not local; will forward to <forward-path>
  252	Cannot VRFY user, but will accept message and attempt delivery
  550	Requested action not taken: mailbox unavailable
  551	User not local; please try <forward-path>
  553	Requested action not taken: mailbox name not allowed
  500	Syntax error, command unrecognised
  501	Syntax error in parameters or arguments
  502	Command not implemented
  504	Command parameter not implemented
  421	<domain> Service not available, closing transmission channel
  EXPN
  250	Requested mail action okay, completed
  550	Requested action not taken: mailbox unavailable
  500	Syntax error, command unrecognised
  501	Syntax error in parameters or arguments
  502	Command not implemented
  504	Command parameter not implemented
  421	<domain> Service not available, closing transmission channel
  HELP
  211	System status, or system help reply
  214	Help message
  500	Syntax error, command unrecognised
  501	Syntax error in parameters or arguments
  502	Command not implemented
  504	Command parameter not implemented
  421	<domain> Service not available, closing transmission channel
  NOOP
  200	(nonstandard success response, see rfc876)
  250	Requested mail action okay, completed
  500	Syntax error, command unrecognised
  421	<domain> Service not available, closing transmission channel
  QUIT
  221	<domain> Service closing transmission channel
  500	Syntax error, command unrecognised
  TURN
  250	Requested mail action okay, completed
  502	Command not implemented
  500	Syntax error, command unrecognised
  503	Bad sequence of commands
*/
#include "main.h"
#include "smtp.h"

#include <QDate>
#include <QTime>
#include <QApplication>
#include <QTextCodec>

Smtp::Smtp(const QString &user, const QString &pass, const QString &host, bool vEncrypted, int port)
{
  socket = new QSslSocket(this);
  isEncrypted = vEncrypted;
  isConnected = false;

  connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()));
  connect(socket, SIGNAL(connected()), this, SLOT(connected()));
  connect(socket, SIGNAL(error(QAbstractSocket::SocketError)), this,SLOT(errorReceived(QAbstractSocket::SocketError)));
  connect(socket, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(stateChanged(QAbstractSocket::SocketState)));
  connect(socket, SIGNAL(disconnected()), this,SLOT(disconnected()));

  this->user = user;
  this->pass = pass;

  this->host = host;
  this->port = port;

  logStatus = /*true; //*/false;
  logCommands = /*true; //*/false;
  logReplies = /*true; //*/false;
  logErrors = true;
}
Smtp::~Smtp()
{
  delete stream;
  delete socket;
}
void Smtp::stateChanged(QAbstractSocket::SocketState socketState)
{
  QString mStateDesc;
  switch (socketState) {
    case QAbstractSocket::UnconnectedState:
      mStateDesc = "The socket is disconnected.";
      break;
    case QAbstractSocket::HostLookupState:
      mStateDesc = "The socket is performing a host name lookup.";
      break;
    case QAbstractSocket::ConnectingState:
      mStateDesc = "The socket has started establishing a connection.";
      break;
    case QAbstractSocket::ConnectedState:
      mStateDesc = "A connection is established.";
      break;
    case QAbstractSocket::BoundState:
      mStateDesc = "The socket is bound to an address and port.";
      break;
    case QAbstractSocket::ClosingState:
      mStateDesc = "The socket is about to close (data may still be waiting to be written).";
      break;
    case QAbstractSocket::ListeningState:
      mStateDesc = "For internal use only.";
      break;
    default:
      break;
  }
  if(logStatus) {
    prompt(QString("%1 %2 : %3").arg("state - ").arg(socketState).arg(mStateDesc));
  }
}
void Smtp::errorReceived(QAbstractSocket::SocketError socketError)
{
  QString mErrorDesc;
  switch (socketError) {
    case QAbstractSocket::UnknownSocketError://	-1	:
      mErrorDesc = "An unidentified error occurred.";
      break;
    case QAbstractSocket::ConnectionRefusedError://	0	:
      mErrorDesc = "The connection was refused by the peer (or timed out).";
      break;
    case QAbstractSocket::RemoteHostClosedError://	1	:
      mErrorDesc = ""; //The remote host closed the connection. Note that the client socket (i.e., this socket) will be closed after the remote close notification has been sent.";
      break;
    case QAbstractSocket::HostNotFoundError://	2	:
      mErrorDesc = "The host address was not found.";
      break;
    case QAbstractSocket::SocketAccessError://	3	:
      mErrorDesc = "The socket operation failed because the application lacked the required privileges.";
      break;
    case QAbstractSocket::SocketResourceError://	4	:
      mErrorDesc = "The local system ran out of resources (e.g., too many sockets).";
      break;
    case QAbstractSocket::SocketTimeoutError://	5:
      mErrorDesc = "	The socket operation timed out.";
      break;
    case QAbstractSocket::DatagramTooLargeError://	6	:
      mErrorDesc = "The datagram was larger than the operating system's limit (which can be as low as 8192 bytes).";
      break;
    case QAbstractSocket::NetworkError://	7	:
      mErrorDesc = "An error occurred with the network (e.g., the network cable was accidentally plugged out).";
      break;
    case QAbstractSocket::AddressInUseError://	8	:
      mErrorDesc = "The address specified to QAbstractSocket::bind() is already in use and was set to be exclusive.";
      break;
    case QAbstractSocket::SocketAddressNotAvailableError://	9	:
      mErrorDesc = "The address specified to QAbstractSocket::bind() does not belong to the host.";
      break;
    case QAbstractSocket::UnsupportedSocketOperationError://	10	:
      mErrorDesc = "The requested socket operation is not supported by the local operating system (e.g., lack of IPv6 support).";
      break;
    case QAbstractSocket::UnfinishedSocketOperationError://	11	:
      mErrorDesc = "Used by QAbstractSocketEngine only, The last operation attempted has not finished yet (still in progress in the background).";
      break;
    case QAbstractSocket::ProxyAuthenticationRequiredError://	12	:
      mErrorDesc = "The socket is using a proxy, and the proxy requires authentication.";
      break;
    case QAbstractSocket::SslHandshakeFailedError://	13	:
      mErrorDesc = "The SSL/TLS handshake failed, so the connection was closed (only used in QSslSocket)";
      break;
    case QAbstractSocket::ProxyConnectionRefusedError://	14	:
      mErrorDesc = "Could not contact the proxy server because the connection to that server was denied";
      break;
    case QAbstractSocket::ProxyConnectionClosedError://	15	:
      mErrorDesc = "The connection to the proxy server was closed unexpectedly (before the connection to the final peer was established)";
      break;
    case QAbstractSocket::ProxyConnectionTimeoutError://	16	:
      mErrorDesc = "The connection to the proxy server timed out or the proxy server stopped responding in the authentication phase.";
      break;
    case QAbstractSocket::ProxyNotFoundError://	17	:
      mErrorDesc = "The proxy address set with setProxy() (or the application proxy) was not found.";
      break;
    case QAbstractSocket::ProxyProtocolError://	18	:
      mErrorDesc = "The connection negotiation with the proxy server failed, because the response from the proxy server could not be understood.";
      break;
    case QAbstractSocket::OperationError://	19	:
      mErrorDesc = "An operation was attempted while the socket was in a state that did not permit it.";
      break;
    case QAbstractSocket::SslInternalError://	20	:
      mErrorDesc = "The SSL library being used reported an internal error. This is probably the result of a bad installation or misconfiguration of the library.";
      break;
    case QAbstractSocket::SslInvalidUserDataError://	21	:
      mErrorDesc = "Invalid data (certificate, key, cypher, etc.) was provided and its use resulted in an error in the SSL library.";
      break;
    case QAbstractSocket::TemporaryError://	22	:
      mErrorDesc = "A temporary error occurred (e.g., operation would block and socket is non-blocking).";
      break;
    default:
      break;
  }
  if(logErrors && mErrorDesc != "") {
    prompt(QString("%1:%2 - %3").arg("error").arg(socketError).arg(mErrorDesc));
  }
}
void Smtp::disconnected()
{
  isConnected = false;
  //prompt(QString("%1 %2").arg("Disconnet").arg(socket->error()));
}
void Smtp::connected()
{
  isConnected = true;
  prompt(QString("%1").arg("Connected"));
}
void Smtp::readyRead()
{
}
void Smtp::logscreen(int vNumber ,QString vString) {
  prompt(QString("%1 - %2").arg(vNumber).arg(vString));
}
void Smtp::prompt(QString vString , const QColor vColor) {
  _Textout->setTextColor(vColor);
  _Textout->append(vString);
  _Textout->setTextColor(Qt::black);
  QApplication::processEvents();
}
Smtp::ResonseStruct Smtp::read(int vTimeout)
{
  QString mResponseLine;
  QString mResponse;
  ResonseStruct mResponseStrunct;

  socket->waitForReadyRead(vTimeout);
  do {
    mResponseLine = socket->readLine();
    mResponse += mResponseLine;
  } while ( socket->canReadLine());

  mResponseStrunct.code = mResponseLine.mid(0,4).toInt();
  mResponseStrunct.desc = mResponseLine.mid(4 , mResponseLine.length() - 4);
  if(logReplies) {
    prompt(mResponse);
  }
  return mResponseStrunct;
}
void Smtp::write(QString vString , bool toBase64 , bool vLogCommands, int vTimeout)
{
  if(logCommands) {
    if(vLogCommands) {
      prompt(">" + vString);
    }
  }
  if(toBase64) {
    *stream << QByteArray().append(vString).toBase64()  << CRLF;
  } else {
    *stream << vString  << CRLF;
  }
  stream->flush();
  socket  ->waitForBytesWritten(vTimeout);
}

uint Smtp::sendMail(const QString &vFrom, const QString &vTo, const QString &vSubject, const QString &vBody, QStringList vFiles,QTextEdit *vTextOut)
{
  if(!vTextOut) {
    _Textout = new QTextEdit();
  } else {
    _Textout = vTextOut;
  }
  Smtp::ResonseStruct mRes;
  QString mMessage = "";


  //-------------------------------------------- Connect
  prompt("Connect");
  if(isEncrypted) {
    socket->connectToHostEncrypted(host, port); //"smtp.gmail.com" and 465 for gmail TLS
  } else {
    socket->connectToHost(host, port);         //"mail.nsd-corp.com" and 25 for normal email servers
  }
  prompt("Init");
  stream = new QTextStream( socket );
  mRes = read();
  switch(mRes.code) {
    case eRC_Init: //220	<domain> Service ready
      break;
    default:
      logscreen(mRes.code , mRes.desc);
      goto lQuit;
      break;
  }

  //-------------------------------------------- EHLO
  prompt("Hand Shake");
  write("EHLO " + host);
  mRes = read();
  switch(mRes.code) {
    case eRc_Ok: //250
      break;
    default:
      logscreen(mRes.code , mRes.desc);
      goto lQuit; //return mRes.code;
      break;
  }

  //-------------------------------------------- AUTH LOGIN
  prompt("Login");
  write("AUTH LOGIN");
  mRes = read();
  switch(mRes.code) {
    case eRc_AuthLogin_GetUserPass: //334
      prompt("User");
      write(user,true , false);
      mRes = read();
      switch(mRes.code) {
        case eRc_AuthLogin_GetUserPass: //334
          prompt("Pass");
          write(pass,true,false);
          mRes = read();
          switch(mRes.code) {
            case eRc_AuthLogin_Accepted: //235
              break;
            default:
              logscreen(mRes.code , mRes.desc);
              goto lQuit; //return mRes.code;
              break;
          }
          break;
        default:
          logscreen(mRes.code , mRes.desc);
          goto lQuit; //return mRes.code;
          break;
      }
      break;
    default:
      logscreen(mRes.code , mRes.desc);
      goto lQuit; //return mRes.code;
      break;
  }

  //-------------------------------------------- MAIL
  prompt("From " + vFrom);
  write("MAIL FROM:<" + vFrom + ">");
  mRes = read();
  switch(mRes.code) {
    case eRc_Ok: //250
      break;
    default:
      logscreen(mRes.code , mRes.desc);
      goto lQuit; //return mRes.code;
      break;
  }

  //-------------------------------------------- RCPT
  prompt("To   " + vTo);
  write("RCPT TO:<" + vTo + ">");
  mRes = read();
  switch(mRes.code) {
    case eRc_Ok: //250
      break;
    default:
      logscreen(mRes.code , mRes.desc);
      goto lQuit; //return mRes.code;
      break;
  }

  //-------------------------------------------- DATA
  prompt("Data");
  write("DATA");
  mRes = read();
  switch(mRes.code) {
    case eRC_Data_Start: //354
      break;
    default:
      logscreen(mRes.code , mRes.desc);
      goto lQuit; //return mRes.code;
      break;
  }
  prompt("Body");
  //-------------------------------------------- MESSAGE
  mMessage.append("Date: " + QDateTime::currentDateTime().toString() + "\n");
  qDebug() << vSubject;
  mMessage.append("Subject: " + vSubject + "\n");
  mMessage.append("From: " + vFrom + "\n");
  mMessage.append("To: " + vTo + "\n");
  mMessage.append("MIME-Version: 1.0\n");
  mMessage.append("Content-Type: multipart/mixed; boundary=frontier\n\n");
  mMessage.append( "--frontier\n" );
  //  mMessage.append( "Content-Type: text/html\n\n" );  //Uncomment this for HTML formating, coment the line below
  mMessage.append( "Content-Type: text/plain\n\n" );
  //  mMessage.append(QString().fromLatin1(QByteArray().append(vBody.mid(0,1))));
  //BUG: Persian text messages has problem in some email clients
//  mMessage.append(QTextCodec::codecForName("UTF-8")->toUnicode(QByteArray().append(vBody)));
  mMessage.append(vBody);
  mMessage.append(APPSIGNATURE);
  mMessage.append("\n\n");

  prompt("Files");
  //-------------------------------------------- Attach Files
  if(vFiles.isEmpty()) { prompt(QString("%1").arg("No attachment found")); goto lFileBlockEnd;}
  if(vFiles.count() == 1 && vFiles[0].isEmpty()) { prompt(QString("%1").arg("No attachment found")); goto lFileBlockEnd;}

  prompt(QString("Files to be sent: %2").arg(vFiles.size()));
  foreach(QString mFile, vFiles)
  {
    QFile file(mFile);
    if(file.exists())
    {
      if (file.open(QIODevice::ReadOnly)) {
        prompt("File : " + mFile,Qt::darkBlue);
        QByteArray bytes = file.readAll();
        mMessage.append( "--frontier\n" );
//        qDebug() << QString().fromUtf8(QByteArray().append(QFileInfo(mFile).fileName().simplified().replace(" ","_")));
        QString mFileName = QFileInfo(mFile).fileName().simplified().replace(" ","_");
//        mFileName = QTextCodec::codecForName("UTF-8")->toUnicode(QByteArray().append(mFileName));
        qDebug() << mFileName;
        //BUG: filename has problem
        mMessage.append(
              "Content-Type: application/octet-stream\nContent-Disposition: attachment;"
              "filename=" + mFileName + ";"
              "\nContent-Transfer-Encoding: base64\n\n" );
        mMessage.append(bytes.toBase64());
        mMessage.append("\n");
      } else {
        prompt(QString("Couldn't open the file : %1").arg(mFile));
      }
    } else {
      prompt(QString("File doesn't exits! : %1").arg(mFile) , Qt::darkRed);
    }
  }

  mMessage.append( "--frontier--\n" );

lFileBlockEnd:

  mMessage.replace( QString::fromLatin1( "\n" ), QString::fromLatin1( CRLF ) );
  mMessage.replace( QString::fromLatin1( "\r\n.\r\n" ),QString::fromLatin1( "\r\n..\r\n" ) );

  prompt("Sending");
  write(mMessage + CRLF + "." ,false ,false ,100000);
  mRes = read(100000);
  switch(mRes.code) {
    case eRc_Ok: //250
      prompt("Message Sent to <<<< " + vTo + " >>>>" , Qt::darkGreen);
      break;
    default:
      logscreen(mRes.code , mRes.desc);
      goto lQuit;  //return mRes.code;
      break;
  }

lQuit:
  //-------------------------------------------- QUIT
  prompt("QUIT");
  if(isConnected) {
    write("QUIT");
    mRes = read();
    switch(mRes.code) {
      case eRC_Quit_Ok: //221
        break;
      default:
        logscreen(mRes.code , mRes.desc);
        return mRes.code;
        break;
    }
  }
  return mRes.code; //retrun 0;
}

