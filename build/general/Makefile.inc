
PLUGIN_EXT	?= .so
PLUGIN	?= qm-vamp-plugins$(PLUGIN_EXT)
CXX	?= g++
CC	?= gcc

CFLAGS	:= $(CFLAGS) -I. -Iqm-dsp
CXXFLAGS	:= $(CXXFLAGS) -I. -Iqm-dsp
LDFLAGS		:= -Lqm-dsp $(LDFLAGS)

HEADERS := plugins/AdaptiveSpectrogram.h \
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
           plugins/TonalChangeDetect.h \
           plugins/Transcription.h

SOURCES := g2cstubs.c \
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
           plugins/Transcription.cpp \
           libmain.cpp

OBJECTS := $(SOURCES:.cpp=.o)
OBJECTS := $(OBJECTS:.c=.o)

all: qm-dsp $(PLUGIN)

MF   := $(wildcard build/*/Makefile$(MAKEFILE_EXT))

.PHONY: qm-dsp
qm-dsp: 
		$(MAKE) -C $@ -f $(MF)

$(PLUGIN):	$(OBJECTS)
		$(CXX) -o $@ $^ $(LDFLAGS)

clean:		
		$(MAKE) -C qm-dsp -f $(MF) clean
		rm -f $(OBJECTS)

distclean:	clean
		rm $(PLUGIN)
