
TEMPLATE = lib

CONFIG += plugin warn_on release
CONFIG -= qt

OBJECTS_DIR = tmp_obj
MOC_DIR = tmp_moc

INCLUDEPATH += build/linux/amd64 ../vamp-plugin-sdk ../qm-dsp
LIBPATH += build/linux/amd64 ../vamp-plugin-sdk/src ../qm-dsp

LIBS += -static-libgcc -Wl,-Bstatic -lqm-dsp -lvamp-sdk -llapack -lcblas -latlas -lc -Wl,-Bdynamic -Wl,--version-script=vamp-plugin.map

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
SOURCES += g2cstubs.c \
           plugins/BeatTrack.cpp \
           plugins/OnsetDetect.cpp \
           plugins/ChromagramPlugin.cpp \
           plugins/ConstantQSpectrogram.cpp \
           plugins/KeyDetect.cpp \
           plugins/MFCCPlugin.cpp \
           plugins/SegmenterPlugin.cpp \
           plugins/SimilarityPlugin.cpp \
           plugins/TonalChangeDetect.cpp \
           ./libmain.cpp

