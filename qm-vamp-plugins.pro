
TEMPLATE = lib

CONFIG += plugin warn_on release
CONFIG -= qt

linux-g++:QMAKE_CXXFLAGS_RELEASE += -DNDEBUG -O2 -march=pentium3 -mfpmath=sse -ffast-math

OBJECTS_DIR = tmp_obj
MOC_DIR = tmp_moc

INCLUDEPATH += ../vamp-plugin-sdk ../qm-dsp
LIBPATH += ../vamp-plugin-sdk/vamp-sdk ../qm-dsp

LIBS += -Wl,-Bstatic -lqm-dsp -lvamp-sdk -Wl,-Bdynamic

DEPENDPATH += plugins
INCLUDEPATH += . plugins

# Input
HEADERS += plugins/BeatDetect.h \
           plugins/ChromagramPlugin.h \
           plugins/ConstantQSpectrogram.h \
           plugins/GetModePlugin.h \
           plugins/TonalChangeDetect.h
SOURCES += plugins/BeatDetect.cpp \
           plugins/ChromagramPlugin.cpp \
           plugins/ConstantQSpectrogram.cpp \
           plugins/GetModePlugin.cpp \
           plugins/TonalChangeDetect.cpp \
           ./libmain.cpp
