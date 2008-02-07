
TEMPLATE = lib

CONFIG += plugin warn_on release
CONFIG -= qt

linux-g++:QMAKE_CXXFLAGS_RELEASE += -DNDEBUG -O3 -fno-exceptions -fPIC -march=pentium3 -mfpmath=sse -msse -ffast-math

OBJECTS_DIR = tmp_obj
MOC_DIR = tmp_moc

INCLUDEPATH += ../vamp-plugin-sdk ../qm-dsp
LIBPATH += ../vamp-plugin-sdk/vamp-sdk ../qm-dsp

linux-g++:LIBS += -static-libgcc -Wl,-Bstatic -lqm-dsp -lvamp-sdk -L/usr/lib/atlas/sse -llapack -lblas -lg2c $(shell g++ -print-file-name=libstdc++.a) -Wl,-Bdynamic
#LIBS += -Wl,-Bstatic -lqm-dsp -lvamp-sdk -L/usr/lib/atlas/sse -lblas -llapack -lg2c -Wl,-Bdynamic

DEPENDPATH += plugins
INCLUDEPATH += . plugins

# Input
HEADERS += plugins/BeatTrack.h \
           plugins/OnsetDetect.h \
           plugins/ChromagramPlugin.h \
           plugins/ConstantQSpectrogram.h \
           plugins/KeyDetect.h \
           plugins/MFCCPlugin.h \
           plugins/SegmenterPlugin.h \
           plugins/SimilarityPlugin.h \
           plugins/TonalChangeDetect.h
SOURCES += plugins/BeatTrack.cpp \
           plugins/OnsetDetect.cpp \
           plugins/ChromagramPlugin.cpp \
           plugins/ConstantQSpectrogram.cpp \
           plugins/KeyDetect.cpp \
           plugins/MFCCPlugin.cpp \
           plugins/SegmenterPlugin.cpp \
           plugins/SimilarityPlugin.cpp \
           plugins/TonalChangeDetect.cpp \
           ./libmain.cpp

