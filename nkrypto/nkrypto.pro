#-------------------------------------------------
#
# Project created by QtCreator 2018-05-15T09:56:42
#
#-------------------------------------------------

QT       -= gui

TARGET = nkrypto
TEMPLATE = lib

DEFINES += NKRYPTO_LIBRARY

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        nkrypto.cpp \
    SnapShot.cpp \
    HarUsb150Info.cpp

HEADERS += \
        nkrypto.h \
        nkrypto_global.h \ 
    debug.h \
    HarUsb150Info.h \
    BaseInfo.h \
    CameraDefine.h \
    SnapShot.h \
    CameraStatus.h \
    CameraApi.h \
    krypto_trace_security_toolset.h \
    ndcode_decoder.h \
    ndcode_image_recognition.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

CONFIG(debug, debug|release) {
    DESTDIR = ../Debug/x32
    OBJECTS_DIR = ../Debug/x32
    MOC_DIR = ../Debug/x32
    RCC_DIR = ../Debug/x32
    UI_DIR = ../Debug/x32
} else {
    DESTDIR = ../Release/x32
    OBJECTS_DIR = ../Release/x32
    MOC_DIR = ../Release/x32
    RCC_DIR = ../Release/x32
    UI_DIR = ../Release/x32
}

LIBS += -L$$PWD/../nkrypto/ -lMVCAMSDK -lNDCode_Image_Recognition -lndcode_decoder -lkrypto_trace_security_toolset
