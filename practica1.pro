#-------------------------------------------------
#
# Project created by QtCreator 2013-04-05T19:35:15
#
#-------------------------------------------------

QT       += core gui serialport widgets svg

TARGET = practica1
TEMPLATE = app


SOURCES += main.cpp\
    crc.c \
    qtivarpc.cpp \
    serialprotocol.c \
    mainusergui.cpp

HEADERS  += \
    crc.h \
    qtivarpc.h \
    serialprotocol.h \
    mainusergui.h \
    rpc_commands.h

FORMS    += \
    mainusergui.ui

CONFIG   +=qwt # Añadir para usar Qwidgets


win32:debug {
    LIBS += -lqwtd -lanalogwidgetsd -lqmqttd -lColorWidgets-qt5d# Añadido Necesario para encontrar implementación de librerias (si estoy en windows y en modo debug)!!
}

win32:release {
    LIBS += -lqwt -lanalogwidgets -lqmqtt -lColorWidgets-qt5 # Añadido Necesario para encontrar implementación de librerias (si estoy en windows y en modo release)!!
}

unix:!macx {
    LIBS += -lqwt -lanalogwidgets -lqmqtt -lColorWidgets-qt5# Añadido Necesario para encontrar implementación de librerias (Linux)!!
}

#Nota MacOS: puede ser necesario cambiar $(QTDIR) por el EL PATH hacia Qt!!!
macx: {
LIBS += -framework qwt -lanalogwidgets -lqmqtt -L$(QTDIR)/lib/ -lColorWidgets-qt5.1
INCLUDEPATH += $(QTDIR)/include/
INCLUDEPATH += $(QTDIR)/lib/qwt.framework/Headers
}
