TEMPLATE = lib
CONFIG += release warn_on dll
OBJECTS_DIR = tmp_obj
MOC_DIR = tmp_moc

INCLUDEPATH += ../../sonic-visualiser/plugin/vamp-plugin-sdk ../../qm-dsp/trunk

DEPENDPATH += plugins
INCLUDEPATH += . plugins

# Input
HEADERS += plugins/BeatDetect.h \
           plugins/ChromagramPlugin.h \
           plugins/TonalChangeDetect.h
SOURCES += plugins/BeatDetect.cpp \
           plugins/ChromagramPlugin.cpp \
           plugins/TonalChangeDetect.cpp
