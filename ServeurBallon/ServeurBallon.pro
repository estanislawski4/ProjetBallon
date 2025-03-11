QT       += core gui widgets websockets serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    aprsisclient.cpp \
    ax25converter.cpp \
    kisshandler.cpp \
    main.cpp \
    interface.cpp \
    serialportmanager.cpp \
    websocketserver.cpp

HEADERS += \
    aprsisclient.h \
    ax25converter.h \
    interface.h \
    kisshandler.h \
    serialportmanager.h \
    websocketserver.h

FORMS += \
    interface.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
