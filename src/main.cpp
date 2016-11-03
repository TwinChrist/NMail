#include "main.h"
#include "mainwindow.h"
#include <QApplication>
#include <QTextCodec>
#include <QCommandLineParser>

enum CommandLineParseResult
{
  eCl_Ok,
  eCl_Error,
  eCl_Version,
  eCl_Help
};

CommandLineParseResult parseCommandLine(QCommandLineParser &parser, QString *errorMessage)
{
  parser.setSingleDashWordOptionMode(QCommandLineParser::ParseAsLongOptions);
  //    // -n option
  //    const QCommandLineOption nameServerOption("n", "The name server to use.", "nameserver");
  //    parser.addOption(nameServerOption);
  //    // -t option
  //    const QCommandLineOption typeOption("t", "The lookup type.", "type");
  //    parser.addOption(typeOption);
  //    // -name option
  //    parser.addPositionalArgument("name", "The name to look up.");
  // -h option
  const QCommandLineOption helpOption = parser.addHelpOption();
  // -v option
  const QCommandLineOption versionOption = parser.addVersionOption();
  // parse
  if (!parser.parse(QCoreApplication::arguments())) {
    *errorMessage = parser.errorText();
    return eCl_Error;
  }

  if (parser.isSet(versionOption))
    return eCl_Version;

  if (parser.isSet(helpOption))
    return eCl_Help;

  //    if (parser.isSet(nameServerOption)) {
  //        const QString nameserver = parser.value(nameServerOption);
  //        query->nameServer = QHostAddress(nameserver);
  //        if (query->nameServer.isNull() || query->nameServer.protocol() == QAbstractSocket::UnknownNetworkLayerProtocol) {
  //            *errorMessage = "Bad nameserver address: " + nameserver;
  //            return eCl_Error;
  //        }
  //    }

  //    if (parser.isSet(typeOption)) {
  //        const QString typeParameter = parser.value(typeOption);
  //        const int type = typeFromParameter(typeParameter.toLower());
  //        if (type < 0) {
  //            *errorMessage = "Bad record type: " + typeParameter;
  //            return eCl_Error;
  //        }
  //        query->type = static_cast<QDnsLookup::Type>(type);
  //    }

  //    const QStringList positionalArguments = parser.positionalArguments();
  //    if (positionalArguments.isEmpty()) {
  //        *errorMessage = "Argument 'name' missing.";
  //        return eCl_Error;
  //    }
  //    if (positionalArguments.size() > 1) {
  //        *errorMessage = "Several 'name' arguments specified.";
  //        return eCl_Error;
  //    }
  //    query->name = positionalArguments.first();

  return eCl_Ok;
}

int main(int argc, char *argv[])
{
      QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
//  QTextCodec::setCodecForLocale(QTextCodec::codecForName("Windows-1256"));
  QLocale::setDefault(QLocale(QLocale::system()));

  QApplication app(argc, argv);

  //    QCommandLineParser parser;
  //    QString errorMessage;
  //    switch (parseCommandLine(parser, &errorMessage)) {
  //        case eCl_Ok:
  //            break;
  //        case eCl_Error:
  //            QMessageBox::warning(0, QGuiApplication::applicationDisplayName(),
  //                                 "<html><head/><body><h2>" + errorMessage + "</h2><pre>"
  //                                 + parser.helpText() + "</pre></body></html>");
  //            return 1;
  //        case eCl_Version:
  //            QMessageBox::information(0, QGuiApplication::applicationDisplayName(),
  //                                     QGuiApplication::applicationDisplayName() + ' '
  //                                     + QCoreApplication::applicationVersion());
  //            return 0;
  //        case eCl_Help:
  //            QMessageBox::warning(0, QGuiApplication::applicationDisplayName(),
  //                                 "<html><head/><body><pre>"
  //                                 + parser.helpText() + "</pre></body></html>");
  //            return 0;
  //        }
  MainWindow w;
  if(argc > 1) {
    w.do_Load(argv[1]);
  }
  w.show();

  return app.exec();
}
