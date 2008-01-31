
TEMPLATE = lib

CONFIG += dll plugin warn_on release
CONFIG -= qt

linux-g++:QMAKE_CXXFLAGS_RELEASE += -DNDEBUG -O3 -march=pentium4 -msse -msse2
QMAKE_CXXFLAGS_RELEASE += -DNDEBUG -O2 -march=pentium3 -msse

OBJECTS_DIR = tmp_obj
MOC_DIR = tmp_moc

INCLUDEPATH += ../vamp-plugin-sdk ../qm-dsp
LIBPATH += ./build/mingw32 ../vamp-plugin-sdk/vamp-sdk ../qm-dsp ../qm-dsp/release

LIBS += -shared -Wl,-Bstatic -lqm-dsp -lvamp-sdk -llapack -lcblas -latlas -lf77blas -lg2cstubs -Wl,-Bdynamic

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

