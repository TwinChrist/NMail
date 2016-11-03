#-------------------------------------------------
#
# Project created by QtCreator 2013-07-09T15:55:12
#
#-------------------------------------------------
#test of changing

QT      += widgets network
CONFIG  += C++14

TARGET = ../../../bin/NMail
TEMPLATE = app
RC_FILE = rc_file.rc
#VERSION = 1.2.3.4 # it doesn't wotk because of RC_FILE (Qt Uses RC_FILE and ignores then VERSION)

SOURCES += main.cpp\
        mainwindow.cpp \
    smtp.cpp

HEADERS  += mainwindow.h \
    smtp.h \
    version.h \
    main.h

FORMS    += mainwindow.ui

OTHER_FILES +=

RESOURCES += \
    NSDMailSender.qrc
