
TEMPLATE = lib

CONFIG += plugin warn_on release
CONFIG -= qt

OBJECTS_DIR = tmp_obj
MOC_DIR = tmp_moc

INCLUDEPATH += ../vamp-plugin-sdk ../qm-dsp
LIBPATH += ../vamp-plugin-sdk/vamp-sdk ../qm-dsp

LIBS += -lqm-dsp -lvamp-sdk

DEPENDPATH += plugins
INCLUDEPATH += . plugins

# Input
HEADERS += plugins/BeatDetect.h \
           plugins/ChromagramPlugin.h \
           plugins/ConstantQSpectrogram.h \
           plugins/TonalChangeDetect.h
SOURCES += plugins/BeatDetect.cpp \
           plugins/ChromagramPlugin.cpp \
           plugins/ConstantQSpectrogram.cpp \
           plugins/TonalChangeDetect.cpp \
           ./libmain.cpp
