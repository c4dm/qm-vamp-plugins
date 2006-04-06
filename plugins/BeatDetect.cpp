/* -*- c-basic-offset: 4 indent-tabs-mode: nil -*-  vi:set ts=8 sts=4 sw=4: */

/*
    QM Vamp Plugin Set

    Centre for Digital Music, Queen Mary, University of London.
    All rights reserved.
*/

#include "BeatDetect.h"

#include <dsp/onsets/DetectionFunction.h>
#include <dsp/tempotracking/TempoTrack.h>

using std::string;
using std::vector;
using std::cerr;
using std::endl;

float BeatDetector::m_stepSecs = 0.01161;

class BeatDetectorData
{
public:
    BeatDetectorData(const DFConfig &config) : dfConfig(config) {
	df = new DetectionFunction(config);
    }
    ~BeatDetectorData() {
	delete df;
    }
    void reset() {
	delete df;
	df = new DetectionFunction(dfConfig);
	dfOutput.clear();
    }

    DFConfig dfConfig;
    DetectionFunction *df;
    vector<double> dfOutput;
};
    

BeatDetector::BeatDetector(float inputSampleRate) :
    Vamp::Plugin(inputSampleRate),
    m_d(0),
    m_dfType(DF_COMPLEXSD)
{
}

BeatDetector::~BeatDetector()
{
    delete m_d;
}

string
BeatDetector::getName() const
{
    return "beats";
}

string
BeatDetector::getDescription() const
{
    return "Beat Detector";
}

string
BeatDetector::getMaker() const
{
    return "QMUL";
}

int
BeatDetector::getPluginVersion() const
{
    return 1;
}

string
BeatDetector::getCopyright() const
{
    return "Copyright (c) 2006 - All Rights Reserved";
}

BeatDetector::ParameterList
BeatDetector::getParameterDescriptors() const
{
    ParameterList list;

    ParameterDescriptor desc;
    desc.name = "dftype";
    desc.description = "Onset Detection Function Type";
    desc.minValue = 0;
    desc.maxValue = 3;
    desc.defaultValue = 3;
    desc.isQuantized = true;
    desc.quantizeStep = 1;
    desc.valueNames.push_back("High-Frequency Content");
    desc.valueNames.push_back("Spectral Difference");
    desc.valueNames.push_back("Phase Deviation");
    desc.valueNames.push_back("Complex Domain");
    list.push_back(desc);

    return list;
}

float
BeatDetector::getParameter(std::string name) const
{
    if (name == "dftype") {
        switch (m_dfType) {
        case DF_HFC: return 0;
        case DF_SPECDIFF: return 1;
        case DF_PHASEDEV: return 2;
        default: case DF_COMPLEXSD: return 3;
        }
    }
    return 0.0;
}

void
BeatDetector::setParameter(std::string name, float value)
{
    if (name == "dftype") {
        switch (lrintf(value)) {
        case 0: m_dfType = DF_HFC; break;
        case 1: m_dfType = DF_SPECDIFF; break;
        case 2: m_dfType = DF_PHASEDEV; break;
        default: case 3: m_dfType = DF_COMPLEXSD; break;
        }
    }
}

bool
BeatDetector::initialise(size_t channels, size_t stepSize, size_t blockSize)
{
    if (m_d) {
	delete m_d;
	m_d = 0;
    }

    if (channels < getMinChannelCount() ||
	channels > getMaxChannelCount()) return false;

    DFConfig dfConfig;
    dfConfig.DFType = m_dfType;
    dfConfig.stepSecs = float(stepSize) / m_inputSampleRate;
    dfConfig.stepSize = stepSize;
    dfConfig.frameLength = blockSize;
    
    m_d = new BeatDetectorData(dfConfig);
    return true;
}

void
BeatDetector::reset()
{
    if (m_d) m_d->reset();
}

size_t
BeatDetector::getPreferredStepSize() const
{
    return size_t(m_inputSampleRate * m_stepSecs + 0.0001);
}

size_t
BeatDetector::getPreferredBlockSize() const
{
    return getPreferredStepSize() * 2;
}

BeatDetector::OutputList
BeatDetector::getOutputDescriptors() const
{
    OutputList list;

    OutputDescriptor beat;
    beat.name = "beats";
    beat.unit = "";
    beat.description = "Detected Beats";
    beat.hasFixedBinCount = true;
    beat.binCount = 0;
    beat.sampleType = OutputDescriptor::VariableSampleRate;
    beat.sampleRate = 1.0 / m_stepSecs;

    OutputDescriptor df;
    df.name = "detection_fn";
    df.unit = "";
    df.description = "Beat Detection Function";
    df.hasFixedBinCount = true;
    df.binCount = 1;
    df.hasKnownExtents = false;
    df.isQuantized = false;
    df.sampleType = OutputDescriptor::OneSamplePerStep;

    list.push_back(beat);
    list.push_back(df);

    return list;
}

BeatDetector::FeatureSet
BeatDetector::process(float **inputBuffers, Vamp::RealTime /* timestamp */)
{
    if (!m_d) {
	cerr << "ERROR: BeatDetector::process: "
	     << "BeatDetector has not been initialised"
	     << endl;
	return FeatureSet();
    }

    // convert float* to double*
    double *tempBuffer = new double[m_d->dfConfig.frameLength];
    for (size_t i = 0; i < m_d->dfConfig.frameLength; ++i) {
	tempBuffer[i] = inputBuffers[0][i];
    }

    double output = m_d->df->process(tempBuffer);
    delete[] tempBuffer;

    m_d->dfOutput.push_back(output);

    FeatureSet returnFeatures;

    Feature feature;
    feature.hasTimestamp = false;
    feature.values.push_back(output);

    returnFeatures[1].push_back(feature); // detection function is output 1
    return returnFeatures;
}

BeatDetector::FeatureSet
BeatDetector::getRemainingFeatures()
{
    if (!m_d) {
	cerr << "ERROR: BeatDetector::getRemainingFeatures: "
	     << "BeatDetector has not been initialised"
	     << endl;
	return FeatureSet();
    }

    double aCoeffs[] = { 1.0000, -0.5949, 0.2348 };
    double bCoeffs[] = { 0.1600,  0.3200, 0.1600 };

    TTParams ttParams;
    ttParams.winLength = 512;
    ttParams.lagLength = 128;
    ttParams.LPOrd = 2;
    ttParams.LPACoeffs = aCoeffs;
    ttParams.LPBCoeffs = bCoeffs;
    ttParams.alpha = 9;
    ttParams.WinT.post = 8;
    ttParams.WinT.pre = 7;

    TempoTrack tempoTracker(ttParams);
    vector<int> beats = tempoTracker.process(m_d->dfOutput);

    FeatureSet returnFeatures;

    for (size_t i = 0; i < beats.size(); ++i) {

	size_t frame = beats[i] * m_d->dfConfig.stepSize;

	Feature feature;
	feature.hasTimestamp = true;
	feature.timestamp = Vamp::RealTime::frame2RealTime
	    (frame, lrintf(m_inputSampleRate));

	float bpm = 0.0;
	int frameIncrement = 0;

	if (i < beats.size() - 1) {

	    frameIncrement = (beats[i+1] - beats[i]) * m_d->dfConfig.stepSize;

	    // one beat is frameIncrement frames, so there are
	    // samplerate/frameIncrement bps, so
	    // 60*samplerate/frameIncrement bpm

	    if (frameIncrement > 0) {
		bpm = (60.0 * m_inputSampleRate) / frameIncrement;
		bpm = int(bpm * 100.0 + 0.5) / 100.0;
                static char label[100];
                sprintf(label, "%f bpm", bpm);
                feature.label = label;
	    }
	}

	returnFeatures[0].push_back(feature); // beats are output 0
    }

    return returnFeatures;
}

