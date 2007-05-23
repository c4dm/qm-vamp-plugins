/* -*- c-basic-offset: 4 indent-tabs-mode: nil -*-  vi:set ts=8 sts=4 sw=4: */

/*
    QM Vamp Plugin Set

    Centre for Digital Music, Queen Mary, University of London.
    All rights reserved.
*/

#include "BeatTrack.h"

#include <dsp/onsets/DetectionFunction.h>
#include <dsp/onsets/PeakPicking.h>
#include <dsp/tempotracking/TempoTrack.h>

using std::string;
using std::vector;
using std::cerr;
using std::endl;

float BeatTracker::m_stepSecs = 0.01161;

class BeatTrackerData
{
public:
    BeatTrackerData(const DFConfig &config) : dfConfig(config) {
	df = new DetectionFunction(config);
    }
    ~BeatTrackerData() {
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
    

BeatTracker::BeatTracker(float inputSampleRate) :
    Vamp::Plugin(inputSampleRate),
    m_d(0),
    m_dfType(DF_COMPLEXSD)
{
}

BeatTracker::~BeatTracker()
{
    delete m_d;
}

string
BeatTracker::getIdentifier() const
{
    return "qm-tempotracker";
}

string
BeatTracker::getName() const
{
    return "Tempo and Beat Tracker";
}

string
BeatTracker::getDescription() const
{
    return "Estimate beat locations and tempo";
}

string
BeatTracker::getMaker() const
{
    return "Christian Landone and Matthew Davies, Queen Mary, University of London";
}

int
BeatTracker::getPluginVersion() const
{
    return 3;
}

string
BeatTracker::getCopyright() const
{
    return "Copyright (c) 2006-2007 - All Rights Reserved";
}

BeatTracker::ParameterList
BeatTracker::getParameterDescriptors() const
{
    ParameterList list;

    ParameterDescriptor desc;
    desc.identifier = "dftype";
    desc.name = "Onset Detection Function Type";
    desc.description = "Method used to calculate the onset detection function";
    desc.minValue = 0;
    desc.maxValue = 3;
    desc.defaultValue = 3;
    desc.isQuantized = true;
    desc.quantizeStep = 1;
    desc.valueNames.push_back("High-Frequency Content");
    desc.valueNames.push_back("Spectral Difference");
    desc.valueNames.push_back("Phase Deviation");
    desc.valueNames.push_back("Complex Domain");
    desc.valueNames.push_back("Broadband Energy Rise");
    list.push_back(desc);

    return list;
}

float
BeatTracker::getParameter(std::string name) const
{
    if (name == "dftype") {
        switch (m_dfType) {
        case DF_HFC: return 0;
        case DF_SPECDIFF: return 1;
        case DF_PHASEDEV: return 2;
        default: case DF_COMPLEXSD: return 3;
        case DF_BROADBAND: return 4;
        }
    }
    return 0.0;
}

void
BeatTracker::setParameter(std::string name, float value)
{
    if (name == "dftype") {
        switch (lrintf(value)) {
        case 0: m_dfType = DF_HFC; break;
        case 1: m_dfType = DF_SPECDIFF; break;
        case 2: m_dfType = DF_PHASEDEV; break;
        default: case 3: m_dfType = DF_COMPLEXSD; break;
        case 4: m_dfType = DF_BROADBAND; break;
        }
    }
}

bool
BeatTracker::initialise(size_t channels, size_t stepSize, size_t blockSize)
{
    if (m_d) {
	delete m_d;
	m_d = 0;
    }

    if (channels < getMinChannelCount() ||
	channels > getMaxChannelCount()) {
        std::cerr << "BeatTracker::initialise: Unsupported channel count: "
                  << channels << std::endl;
        return false;
    }

    if (stepSize != getPreferredStepSize()) {
        std::cerr << "ERROR: BeatTracker::initialise: Unsupported step size for this sample rate: "
                  << stepSize << " (wanted " << (getPreferredStepSize()) << ")" << std::endl;
        return false;
    }

    if (blockSize != getPreferredBlockSize()) {
        std::cerr << "WARNING: BeatTracker::initialise: Unsupported block size for this sample rate: "
                  << blockSize << " (wanted " << getPreferredBlockSize() << ")" << std::endl;
//        return false;
    }

    DFConfig dfConfig;
    dfConfig.DFType = m_dfType;
    dfConfig.stepSecs = float(stepSize) / m_inputSampleRate;
    dfConfig.stepSize = stepSize;
    dfConfig.frameLength = blockSize;
    dfConfig.dbRise = 3;
    
    m_d = new BeatTrackerData(dfConfig);
    return true;
}

void
BeatTracker::reset()
{
    if (m_d) m_d->reset();
}

size_t
BeatTracker::getPreferredStepSize() const
{
    size_t step = size_t(m_inputSampleRate * m_stepSecs + 0.0001);
//    std::cerr << "BeatTracker::getPreferredStepSize: input sample rate is " << m_inputSampleRate << ", step size is " << step << std::endl;
    return step;
}

size_t
BeatTracker::getPreferredBlockSize() const
{
    size_t theoretical = getPreferredStepSize() * 2;

    //!!! need power of 2
    return theoretical;
}

BeatTracker::OutputList
BeatTracker::getOutputDescriptors() const
{
    OutputList list;

    OutputDescriptor beat;
    beat.identifier = "beats";
    beat.name = "Beats";
    beat.description = "Estimated metrical beat locations";
    beat.unit = "";
    beat.hasFixedBinCount = true;
    beat.binCount = 0;
    beat.sampleType = OutputDescriptor::VariableSampleRate;
    beat.sampleRate = 1.0 / m_stepSecs;

    OutputDescriptor df;
    df.identifier = "detection_fn";
    df.name = "Onset Detection Function";
    df.description = "Probability function of note onset likelihood";
    df.unit = "";
    df.hasFixedBinCount = true;
    df.binCount = 1;
    df.hasKnownExtents = false;
    df.isQuantized = false;
    df.sampleType = OutputDescriptor::OneSamplePerStep;

    OutputDescriptor tempo;
    tempo.identifier = "tempo";
    tempo.name = "Tempo";
    tempo.description = "Locked tempo estimates";
    tempo.unit = "bpm";
    tempo.hasFixedBinCount = true;
    tempo.binCount = 1;
    tempo.sampleType = OutputDescriptor::VariableSampleRate;
    tempo.sampleRate = 1.0 / m_stepSecs;

    list.push_back(beat);
    list.push_back(df);
    list.push_back(tempo);

    return list;
}

BeatTracker::FeatureSet
BeatTracker::process(const float *const *inputBuffers,
                     Vamp::RealTime /* timestamp */)
{
    if (!m_d) {
	cerr << "ERROR: BeatTracker::process: "
	     << "BeatTracker has not been initialised"
	     << endl;
	return FeatureSet();
    }

    size_t len = m_d->dfConfig.frameLength / 2;

    double *magnitudes = new double[len];
    double *phases = new double[len];

    // We only support a single input channel

    for (size_t i = 0; i < len; ++i) {

        magnitudes[i] = sqrt(inputBuffers[0][i*2  ] * inputBuffers[0][i*2  ] +
                             inputBuffers[0][i*2+1] * inputBuffers[0][i*2+1]);

	phases[i] = atan2(-inputBuffers[0][i*2+1], inputBuffers[0][i*2]);
    }

    double output = m_d->df->process(magnitudes, phases);

    delete[] magnitudes;
    delete[] phases;

    m_d->dfOutput.push_back(output);

    FeatureSet returnFeatures;

    Feature feature;
    feature.hasTimestamp = false;
    feature.values.push_back(output);

    returnFeatures[1].push_back(feature); // detection function is output 1
    return returnFeatures;
}

BeatTracker::FeatureSet
BeatTracker::getRemainingFeatures()
{
    if (!m_d) {
	cerr << "ERROR: BeatTracker::getRemainingFeatures: "
	     << "BeatTracker has not been initialised"
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

    vector<double> tempos;
    vector<int> beats = tempoTracker.process(m_d->dfOutput, &tempos);

    FeatureSet returnFeatures;

    char label[100];

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
                sprintf(label, "%.2f bpm", bpm);
                feature.label = label;
	    }
	}

	returnFeatures[0].push_back(feature); // beats are output 0
    }

    double prevTempo = 0.0;

    for (size_t i = 0; i < tempos.size(); ++i) {

        size_t frame = i * m_d->dfConfig.stepSize * ttParams.lagLength;

//        std::cerr << "unit " << i << ", step size " << m_d->dfConfig.stepSize << ", hop " << ttParams.lagLength << ", frame = " << frame << std::endl;
        
        if (tempos[i] > 1 && int(tempos[i] * 100) != int(prevTempo * 100)) {
            Feature feature;
            feature.hasTimestamp = true;
            feature.timestamp = Vamp::RealTime::frame2RealTime
                (frame, lrintf(m_inputSampleRate));
            feature.values.push_back(tempos[i]);
            sprintf(label, "%.2f bpm", tempos[i]);
            feature.label = label;
            returnFeatures[2].push_back(feature); // tempo is output 2
        }
    }

    return returnFeatures;
}

