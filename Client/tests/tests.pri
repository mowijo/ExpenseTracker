LIBS += -L../../src/lib -lmodel
PRE_TARGETDEPS += ../../src/lib/libmodel.a

QT += testlib network core

INCLUDEPATH += ../../src/lib

CONFIG += c++11
CONFIG += build
CONFIG += testcase

