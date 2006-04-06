
TEMPLATE = lib

CONFIG += plugin warn_on release
CONFIG -= qt

OBJECTS_DIR = tmp_obj
MOC_DIR = tmp_moc

INCLUDEPATH += ../vamp-plugin-sdk ../qm-dsp
LIBPATH += ../qm-dsp

LIBS += -lqm-dsp

DEPENDPATH += plugins
INCLUDEPATH += . plugins

# Input
HEADERS += plugins/BeatDetect.h \
           plugins/ChromagramPlugin.h \
           plugins/TonalChangeDetect.h
SOURCES += plugins/BeatDetect.cpp \
           plugins/ChromagramPlugin.cpp \
           plugins/TonalChangeDetect.cpp \
           ../vamp-plugin-sdk/vamp-sdk/PluginAdapter.cpp \
           ../vamp-plugin-sdk/vamp-sdk/RealTime.cpp \
           ./libmain.cpp
