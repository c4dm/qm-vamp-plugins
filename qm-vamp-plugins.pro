
TEMPLATE = lib

CONFIG += plugin warn_on release
CONFIG -= qt

linux-g++:QMAKE_CXXFLAGS_RELEASE += -DNDEBUG -O3 -fPIC -march=pentium3 -mfpmath=sse -msse
linux-g++-64:QMAKE_CXXFLAGS_RELEASE += -DNDEBUG -O3 -fPIC -ffast-math -fno-exceptions

OBJECTS_DIR = tmp_obj
MOC_DIR = tmp_moc

INCLUDEPATH += ../vamp-plugin-sdk ../qm-dsp
LIBPATH += ../vamp-plugin-sdk/vamp-sdk ../qm-dsp

linux-g++:LIBS += -static-libgcc -Wl,-Bstatic -lqm-dsp -lvamp-sdk -L/usr/lib/sse2/atlas -L/usr/lib/atlas/sse -llapack -lblas $$system(g++ -print-file-name=libstdc++.a) -lc -Wl,-Bdynamic -Wl,--version-script=vamp-plugin.map

linux-g++-64:LIBS += -Lbuild/linux/amd64  -Wl,-Bstatic -lqm-dsp -lvamp-sdk -llapack -lcblas -latlas -Wl,-Bdynamic -Wl,--version-script=vamp-plugin.map

macx-g++:LIBS += -framework Accelerate -exported_symbols_list=vamp-plugin.list

#LIBS += -Wl,-Bstatic -lqm-dsp -lvamp-sdk -L/usr/lib/atlas/sse -lblas -llapack -Wl,-Bdynamic

DEPENDPATH += plugins
INCLUDEPATH += . plugins

# Input
HEADERS += plugins/AdaptiveSpectrogram.h \
           plugins/BarBeatTrack.h \
           plugins/BeatTrack.h \
           plugins/DWT.h \
           plugins/OnsetDetect.h \
           plugins/ChromagramPlugin.h \
           plugins/ConstantQSpectrogram.h \
           plugins/KeyDetect.h \
           plugins/MFCCPlugin.h \
           plugins/SegmenterPlugin.h \
           plugins/SimilarityPlugin.h \
           plugins/TonalChangeDetect.h
SOURCES += g2cstubs.c \
           plugins/AdaptiveSpectrogram.cpp \
           plugins/BarBeatTrack.cpp \
           plugins/BeatTrack.cpp \
           plugins/DWT.cpp \
           plugins/OnsetDetect.cpp \
           plugins/ChromagramPlugin.cpp \
           plugins/ConstantQSpectrogram.cpp \
           plugins/KeyDetect.cpp \
           plugins/MFCCPlugin.cpp \
           plugins/SegmenterPlugin.cpp \
           plugins/SimilarityPlugin.cpp \
           plugins/TonalChangeDetect.cpp \
           ./libmain.cpp

