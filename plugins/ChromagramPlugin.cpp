/* -*- c-basic-offset: 4 indent-tabs-mode: nil -*-  vi:set ts=8 sts=4 sw=4: */

/*
    QM Vamp Plugin Set

    Centre for Digital Music, Queen Mary, University of London.
    All rights reserved.
*/

#include "ChromagramPlugin.h"

#include <base/Pitch.h>
#include <dsp/chromagram/Chromagram.h>

using std::string;
using std::vector;
using std::cerr;
using std::endl;

ChromagramPlugin::ChromagramPlugin(float inputSampleRate) :
    Vamp::Plugin(inputSampleRate),
    m_chromagram(0),
    m_step(0),
    m_block(0)
{
    m_minMIDIPitch = 12;
    m_maxMIDIPitch = 96;
    m_tuningFrequency = 440;
    m_normalise = MathUtilities::NormaliseUnitMax;
    m_bpo = 12;

    setupConfig();
}

void
ChromagramPlugin::setupConfig()
{
    m_config.FS = lrintf(m_inputSampleRate);
    m_config.min = Pitch::getFrequencyForPitch
        (m_minMIDIPitch, 0, m_tuningFrequency);
    m_config.max = Pitch::getFrequencyForPitch
        (m_maxMIDIPitch, 0, m_tuningFrequency);
    m_config.BPO = m_bpo;
    m_config.CQThresh = 0.0054;
    m_config.normalise = m_normalise;

    m_step = 0;
    m_block = 0;
}

ChromagramPlugin::~ChromagramPlugin()
{
    delete m_chromagram;
}

string
ChromagramPlugin::getIdentifier() const
{
    return "qm-chromagram";
}

string
ChromagramPlugin::getName() const
{
    return "Chromagram";
}

string
ChromagramPlugin::getDescription() const
{
    return "Extract a series of tonal chroma vectors from the audio";
}

string
ChromagramPlugin::getMaker() const
{
    return "Queen Mary, University of London";
}

int
ChromagramPlugin::getPluginVersion() const
{
    return 3;
}

string
ChromagramPlugin::getCopyright() const
{
    return "Plugin by Chris Cannam and Christian Landone.  Copyright (c) 2006-2008 QMUL - All Rights Reserved";
}

ChromagramPlugin::ParameterList
ChromagramPlugin::getParameterDescriptors() const
{
    ParameterList list;

    ParameterDescriptor desc;
    desc.identifier = "minpitch";
    desc.name = "Minimum Pitch";
    desc.unit = "MIDI units";
    //!!! descriptions
    desc.minValue = 0;
    desc.maxValue = 127;
    desc.defaultValue = 12;
    desc.isQuantized = true;
    desc.quantizeStep = 1;
    list.push_back(desc);

    desc.identifier = "maxpitch";
    desc.name = "Maximum Pitch";
    desc.unit = "MIDI units";
    desc.minValue = 0;
    desc.maxValue = 127;
    desc.defaultValue = 96;
    desc.isQuantized = true;
    desc.quantizeStep = 1;
    list.push_back(desc);

    desc.identifier = "tuning";
    desc.name = "Tuning Frequency";
    desc.unit = "Hz";
    desc.minValue = 420;
    desc.maxValue = 460;
    desc.defaultValue = 440;
    desc.isQuantized = false;
    list.push_back(desc);
    
    desc.identifier = "bpo";
    desc.name = "Bins per Octave";
    desc.unit = "bins";
    desc.minValue = 2;
    desc.maxValue = 36;
    desc.defaultValue = 12;
    desc.isQuantized = true;
    desc.quantizeStep = 1;
    list.push_back(desc);

    desc.identifier = "normalization";
    desc.name = "Normalization";
    desc.unit = "";
    desc.minValue = 0;
    desc.maxValue = 2;
    desc.defaultValue = 2;
    desc.isQuantized = true;
    desc.quantizeStep = 1;
    desc.valueNames.push_back("None");
    desc.valueNames.push_back("Unit Sum");
    desc.valueNames.push_back("Unit Maximum");
    list.push_back(desc);

    return list;
}

float
ChromagramPlugin::getParameter(std::string param) const
{
    if (param == "minpitch") {
        return m_minMIDIPitch;
    }
    if (param == "maxpitch") {
        return m_maxMIDIPitch;
    }
    if (param == "tuning") {
        return m_tuningFrequency;
    }
    if (param == "bpo") {
        return m_bpo;
    }
    if (param == "normalization") {
        return int(m_normalise);
    }
    std::cerr << "WARNING: ChromagramPlugin::getParameter: unknown parameter \""
              << param << "\"" << std::endl;
    return 0.0;
}

void
ChromagramPlugin::setParameter(std::string param, float value)
{
    if (param == "minpitch") {
        m_minMIDIPitch = lrintf(value);
    } else if (param == "maxpitch") {
        m_maxMIDIPitch = lrintf(value);
    } else if (param == "tuning") {
        m_tuningFrequency = value;
    } else if (param == "bpo") {
        m_bpo = lrintf(value);
    } else if (param == "normalization") {
        m_normalise = MathUtilities::NormaliseType(int(value + 0.0001));
    } else {
        std::cerr << "WARNING: ChromagramPlugin::setParameter: unknown parameter \""
                  << param << "\"" << std::endl;
    }

    setupConfig();
}


bool
ChromagramPlugin::initialise(size_t channels, size_t stepSize, size_t blockSize)
{
    if (m_chromagram) {
	delete m_chromagram;
	m_chromagram = 0;
    }

    if (channels < getMinChannelCount() ||
	channels > getMaxChannelCount()) return false;

    std::cerr << "ChromagramPlugin::initialise: step " << stepSize << ", block "
	      << blockSize << std::endl;

    m_chromagram = new Chromagram(m_config);
    m_binsums = vector<double>(m_config.BPO);

    for (int i = 0; i < m_config.BPO; ++i) {
        m_binsums[i] = 0.0;
    }

    m_count = 0;

    m_step = m_chromagram->getHopSize();
    m_block = m_chromagram->getFrameSize();

    if (stepSize != m_step ||
        blockSize != m_block) {
        delete m_chromagram;
        m_chromagram = 0;
        return false;
    }

    return true;
}

void
ChromagramPlugin::reset()
{
    if (m_chromagram) {
	delete m_chromagram;
	m_chromagram = new Chromagram(m_config);
    }
}

size_t
ChromagramPlugin::getPreferredStepSize() const
{
    if (!m_step) {
	Chromagram chroma(m_config);
	m_step = chroma.getHopSize();
	m_block = chroma.getFrameSize();
    }

    return m_step;
}

size_t
ChromagramPlugin::getPreferredBlockSize() const
{
    if (!m_block) {
	Chromagram chroma(m_config);
	m_step = chroma.getHopSize();
	m_block = chroma.getFrameSize();
    }

    return m_block;
}

ChromagramPlugin::OutputList
ChromagramPlugin::getOutputDescriptors() const
{
    OutputList list;

    OutputDescriptor d;
    d.identifier = "chromagram";
    d.name = "Chromagram";
    d.unit = "";
    d.hasFixedBinCount = true;
    d.binCount = m_config.BPO;
    
    const char *names[] =
	{ "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" };

    if (d.binCount % 12 == 0) {
        for (int i = 0; i < 12; ++i) {
            int ipc = m_minMIDIPitch % 12;
            int index = (i + ipc) % 12;
            d.binNames.push_back(names[index]);
            for (int j = 0; j < int(d.binCount) / 12 - 1; ++j) {
                d.binNames.push_back("");
            }
        }
    } else {
        d.binNames.push_back(names[m_minMIDIPitch % 12]);
    }

    d.hasKnownExtents = (m_normalise != MathUtilities::NormaliseNone);
    d.minValue = 0.0;
    d.maxValue = (d.hasKnownExtents ? 1.0 : 0.0);
    d.isQuantized = false;
    d.sampleType = OutputDescriptor::OneSamplePerStep;
    list.push_back(d);

    d.identifier = "chromameans";
    d.name = "Chroma Means";
    //!!! descriptions
    d.sampleType = OutputDescriptor::FixedSampleRate;
    d.sampleRate = 1;
    list.push_back(d);

    return list;
}

ChromagramPlugin::FeatureSet
ChromagramPlugin::process(const float *const *inputBuffers,
                          Vamp::RealTime /* timestamp */)
{
    if (!m_chromagram) {
	cerr << "ERROR: ChromagramPlugin::process: "
	     << "Chromagram has not been initialised"
	     << endl;
	return FeatureSet();
    }

    double *real = new double[m_block];
    double *imag = new double[m_block];

    for (size_t i = 0; i < m_block/2; ++i) {
	real[i] = inputBuffers[0][i*2];
	if (i > 0) real[m_block - i] = real[i];
        imag[i] = inputBuffers[0][i*2+1];
        if (i > 0) imag[m_block - i] = imag[i];
    }

    double *output = m_chromagram->process(real, imag);

    delete[] real;
    delete[] imag;

    Feature feature;
    feature.hasTimestamp = false;
    for (size_t i = 0; i < m_config.BPO; ++i) {
        double value = output[i];
        if (isnan(value)) value = 0.0;
        m_binsums[i] += value;
	feature.values.push_back(value);
    }
    feature.label = "";
    ++m_count;

    FeatureSet returnFeatures;
    returnFeatures[0].push_back(feature);
    return returnFeatures;
}

ChromagramPlugin::FeatureSet
ChromagramPlugin::getRemainingFeatures()
{
    Feature feature;
    feature.hasTimestamp = true;
    feature.timestamp = Vamp::RealTime::zeroTime;
  
    for (size_t i = 0; i < m_config.BPO; ++i) {
        double v = m_binsums[i];
        if (m_count > 0) v /= m_count;
        feature.values.push_back(v);
    }
    feature.label = "Chromagram bin means";

    FeatureSet returnFeatures;
    returnFeatures[1].push_back(feature);
    return returnFeatures;
}

