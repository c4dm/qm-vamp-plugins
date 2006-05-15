/* -*- c-basic-offset: 4 indent-tabs-mode: nil -*-  vi:set ts=8 sts=4 sw=4: */

/*
    QM Vamp Plugin Set

    Centre for Digital Music, Queen Mary, University of London.
    All rights reserved.
*/

#include <vamp/vamp.h>
#include <vamp-sdk/PluginAdapter.h>

#include "plugins/BeatDetect.h"
#include "plugins/ChromagramPlugin.h"
#include "plugins/ConstantQSpectrogram.h"
#include "plugins/TonalChangeDetect.h"

static Vamp::PluginAdapter<BeatDetector> beatDetectorAdapter;
static Vamp::PluginAdapter<ChromagramPlugin> chromagramPluginAdapter;
static Vamp::PluginAdapter<ConstantQSpectrogram> constantQAdapter;
static Vamp::PluginAdapter<TonalChangeDetect> tonalChangeDetectorAdapter;

const VampPluginDescriptor *vampGetPluginDescriptor(unsigned int index)
{
    switch (index) {
    case  0: return beatDetectorAdapter.getDescriptor();
    case  1: return chromagramPluginAdapter.getDescriptor();
    case  2: return constantQAdapter.getDescriptor();
    case  3: return tonalChangeDetectorAdapter.getDescriptor();
    default: return 0;
    }
}

