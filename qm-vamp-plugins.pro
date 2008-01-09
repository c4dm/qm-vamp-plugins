
TEMPLATE = lib

CONFIG += plugin warn_on release
CONFIG -= qt

linux-g++:QMAKE_CXXFLAGS_RELEASE += -DNDEBUG -O2 -march=pentium3 -mfpmath=sse -ffast-math

OBJECTS_DIR = tmp_obj
MOC_DIR = tmp_moc

INCLUDEPATH += ../vamp-plugin-sdk ../qm-dsp
LIBPATH += ../vamp-plugin-sdk/vamp-sdk ../qm-dsp

linux-g++:LIBS += -Wl,-Bstatic -lqm-dsp -lvamp-sdk -L/usr/lib/atlas/sse -llapack -lblas -lg2c -Wl,-Bdynamic
#LIBS += -Wl,-Bstatic -lqm-dsp -lvamp-sdk -L/usr/lib/atlas/sse -lblas -llapack -lg2c -Wl,-Bdynamic

DEPENDPATH += plugins
INCLUDEPATH += . plugins

# Input
HEADERS += plugins/BeatTrack.h \
           plugins/OnsetDetect.h \
           plugins/ChromagramPlugin.h \
           plugins/ConstantQSpectrogram.h \
           plugins/KeyDetect.h \
           plugins/SegmenterPlugin.h \
           plugins/TonalChangeDetect.h
SOURCES += plugins/BeatTrack.cpp \
           plugins/OnsetDetect.cpp \
           plugins/ChromagramPlugin.cpp \
           plugins/ConstantQSpectrogram.cpp \
           plugins/KeyDetect.cpp \
           plugins/SegmenterPlugin.cpp \
           plugins/TonalChangeDetect.cpp \
           ./libmain.cpp

