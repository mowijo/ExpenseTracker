include ( ../../Client.pri )
TEMPLATE = app
CONFIG += static
TARGET = client
QT += widgets printsupport



INCLUDEPATH += ../lib
unix {
    LIBS += -lmodel -L../lib
    PRE_TARGETDEPS += ../lib/libmodel.a
}

SOURCES += main.cpp \
    LoginDialog.cpp \
    MainWindow.cpp \
    CreateNewUserDialog.cpp \
    CreateNewExpenseDialog.cpp \
    ExpenseView.cpp

FORMS += \
    LoginDialog.ui \
    MainWindow.ui \
    CreateNewUserDialog.ui \
    CreateNewExpenseDialog.ui

HEADERS += \
    LoginDialog.h \
    MainWindow.h \
    CreateNewUserDialog.h \
    CreateNewExpenseDialog.h \
    ExpenseView.h
