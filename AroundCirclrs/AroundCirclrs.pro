#-------------------------------------------------
#
# Project created by QtCreator 2019-06-17T16:44:49
#
#-------------------------------------------------

QT     += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG  += c++11
TARGET   = AroundCirclrs
TEMPLATE = app

# Output directory
CONFIG(debug, debug|release) {
    output = debug
}
CONFIG(release, debug|release) {
    output = release
}

DESTDIR     = bin
OBJECTS_DIR = $$output
MOC_DIR     = $$output
RCC_DIR     = $$output
UI_DIR      = $$output

FORMS += \
    ArrangeDevicesWidget.ui

HEADERS += \
    AroundCirclesWidget.h \
    ArrangeDevicesWidget.h

SOURCES += \
    AroundCirclesWidget.cpp \
    ArrangeDevicesWidget.cpp \
    main.cpp
