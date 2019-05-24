
PLUGIN_EXT	?= .so
PLUGIN	?= qm-vamp-plugins$(PLUGIN_EXT)
CXX	?= g++
CC	?= gcc

QM_DSP_DIR	:= lib/qm-dsp
VAMP_SDK_DIR    := lib/vamp-plugin-sdk
VAMP_SRC_DIR    := lib/vamp-plugin-sdk/src/vamp-sdk

CFLAGS		:= $(CFLAGS) -I. -I$(QM_DSP_DIR) -I$(VAMP_SDK_DIR)
CXXFLAGS	:= $(CXXFLAGS) -I. -I$(QM_DSP_DIR) -I$(VAMP_SDK_DIR)
LDFLAGS		:= -L$(QM_DSP_DIR) $(LDFLAGS)

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

VAMP_SOURCES := \
           $(VAMP_SRC_DIR)/PluginAdapter.cpp \
           $(VAMP_SRC_DIR)/RealTime.cpp \
           $(VAMP_SRC_DIR)/FFT.cpp

SOURCES    := $(SOURCES) $(VAMP_SOURCES)

OBJECTS := $(SOURCES:.cpp=.o)
OBJECTS := $(OBJECTS:.c=.o)

all: $(QM_DSP_DIR) $(PLUGIN)

MF   := $(wildcard build/*/Makefile$(MAKEFILE_EXT))

.PHONY: $(QM_DSP_DIR)
$(QM_DSP_DIR): 
		$(MAKE) -C $@ -f $(MF)

$(PLUGIN):	$(OBJECTS) $(QM_DSP_DIR)/libqm-dsp.a
		$(CXX) -o $@ $^ $(LDFLAGS)

test:		all
		bash test/regression.sh

clean:		
		$(MAKE) -C $(QM_DSP_DIR) -f $(MF) clean
		rm -f $(OBJECTS)

distclean:	clean
		rm $(PLUGIN)
