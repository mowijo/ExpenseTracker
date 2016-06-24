include ( ../../Client.pri )

TEMPLATE = lib
CONFIG += static
TARGET = model
QT += widgets

SOURCES += User.cpp \
    Account.cpp \
    HttpRequest.cpp \
    Helpers.cpp \
    Exception.cpp \
    UserModel.cpp \
    Expense.cpp \
    ExpenseModel.cpp \
    ReportBuilder.cpp
HEADERS += User.h \
    Account.h \
    HttpRequest.h \
    Helpers.h \
    Exception.h \
    UserModel.h \
    Expense.h \
    ExpenseModel.h \
    ReportBuilder.h
