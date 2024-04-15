QT       += core gui network

INCLUDEPATH += $$PWD\third_party\leveldb\include
LIBS += $$PWD\third_party\leveldb\lib\libleveldb.a

INCLUDEPATH += $$PWD\third_party\rapidjson\include
INCLUDEPATH += $$PWD\protobuf
INCLUDEPATH += $$PWD\include


# DEFINES += PROTOBUF_USE_DLLS
INCLUDEPATH += $$PWD\third_party\protobuf-3.11.2\src
LIBS += -L$$PWD\third_party\protobuf-3.11.2\build


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    protobuf/node.pb.cc \
    src/communicator.cpp \
    src/corelogic.cpp \
    src/directoryarea.cpp \
    src/directoryitem.cpp \
    src/getnamedialog.cpp \
    src/jsontool.cpp \
    src/kvstore.cpp \
    src/main.cpp \
    src/mainwindow.cpp \
    src/pathtree.cpp \
    src/tree.cpp \
    src/util.cpp

HEADERS += \
    include/communicator.h \
    include/corelogic.h \
    include/directoryarea.h \
    include/directoryitem.h \
    include/getnamedialog.h \
    include/global_cfg.h \
    include/jsontool.h \
    include/kvstore.h \
    include/mainwindow.h \
    include/pathtree.h \
    include/tree.h \
    include/util.h \
    protobuf/node.pb.h


FORMS += \
    getnamedialog.ui \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    images/disk.png \
    protobuf/node.proto \
    third_party/leveldb/lib/libleveldb.a
