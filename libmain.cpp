/* -*- c-basic-offset: 4 indent-tabs-mode: nil -*-  vi:set ts=8 sts=4 sw=4: */

/*
    QM Vamp Plugin Set

    Centre for Digital Music, Queen Mary, University of London.
    All rights reserved.
*/

#include <vamp/vamp.h>
#include <vamp-sdk/PluginAdapter.h>

#include "plugins/BeatTrack.h"
#include "plugins/OnsetDetect.h"
#include "plugins/ChromagramPlugin.h"
#include "plugins/ConstantQSpectrogram.h"
#include "plugins/TonalChangeDetect.h"
#include "plugins/KeyDetect.h"

static Vamp::PluginAdapter<BeatTracker> beatTrackerAdapter;
static Vamp::PluginAdapter<OnsetDetector> onsetDetectorAdapter;
static Vamp::PluginAdapter<ChromagramPlugin> chromagramPluginAdapter;
static Vamp::PluginAdapter<ConstantQSpectrogram> constantQAdapter;
static Vamp::PluginAdapter<TonalChangeDetect> tonalChangeDetectorAdapter;
static Vamp::PluginAdapter<KeyDetector> keyDetectorAdapter;

const VampPluginDescriptor *vampGetPluginDescriptor(unsigned int vampApiVersion,
                                                    unsigned int index)
{
    if (vampApiVersion < 1) return 0;

    switch (index) {
    case  0: return beatTrackerAdapter.getDescriptor();
    case  1: return onsetDetectorAdapter.getDescriptor();
    case  2: return chromagramPluginAdapter.getDescriptor();
    case  3: return constantQAdapter.getDescriptor();
    case  4: return tonalChangeDetectorAdapter.getDescriptor();
    case  5: return keyDetectorAdapter.getDescriptor();
    default: return 0;
    }
}

