QT       += core gui network

INCLUDEPATH += $$PWD\third_party\leveldb\include
LIBS += $$PWD\third_party\leveldb\lib\libleveldb.a
INCLUDEPATH += $$PWD\third_party\rapidjson\include

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    communicator.cpp \
    corelogic.cpp \
    directoryarea.cpp \
    directoryitem.cpp \
    getnamedialog.cpp \
    jsontool.cpp \
    kvstore.cpp \
    main.cpp \
    mainwindow.cpp \
    pathtree.cpp

HEADERS += \
    communicator.h \
    corelogic.h \
    directoryarea.h \
    directoryitem.h \
    getnamedialog.h \
    jsontool.h \
    kvstore.h \
    mainwindow.h \
    pathtree.h \
    util.h

FORMS += \
    getnamedialog.ui \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
