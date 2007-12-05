/* -*- c-basic-offset: 4 indent-tabs-mode: nil -*-  vi:set ts=8 sts=4 sw=4: */

/*
    Vamp

    An API for audio analysis and feature extraction plugins.

    Centre for Digital Music, Queen Mary, University of London.
    Copyright 2006-2007 QMUL.
  
    Permission is hereby granted, free of charge, to any person
    obtaining a copy of this software and associated documentation
    files (the "Software"), to deal in the Software without
    restriction, including without limitation the rights to use, copy,
    modify, merge, publish, distribute, sublicense, and/or sell copies
    of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be
    included in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
    NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR
    ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
    CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
    WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

    Except as contained in this notice, the names of the Centre for
    Digital Music; Queen Mary, University of London; and Chris Cannam
    shall not be used in advertising or otherwise to promote the sale,
    use or other dealings in this Software without prior written
    authorization.
*/

#include "KeyDetect.h"

using std::string;
using std::vector;
//using std::cerr;
using std::endl;

#include <cmath>


KeyDetector::KeyDetector(float inputSampleRate) :
    Plugin(inputSampleRate),
    m_stepSize(0),
    m_blockSize(0),
    m_tuningFrequency(440),
    m_length(10),
    m_getKeyMode(0),
    m_inputFrame(0),
    m_prevKey(-1)
{
}

KeyDetector::~KeyDetector()
{
    delete m_getKeyMode;
    if ( m_inputFrame ) {
        delete [] m_inputFrame;
    }
}

string
KeyDetector::getIdentifier() const
{
    return "qm-keydetector";
}

string
KeyDetector::getName() const
{
    return "Key Detector";
}

string
KeyDetector::getDescription() const
{
    //!!!
    return "";
}

string
KeyDetector::getMaker() const
{
    return "Katy Noland and Christian Landone, Queen Mary, University of London";
}

int
KeyDetector::getPluginVersion() const
{
    return 3;
}

string
KeyDetector::getCopyright() const
{
    return "Copyright (c) 2006-2007 - All Rights Reserved";
}

KeyDetector::ParameterList
KeyDetector::getParameterDescriptors() const
{
    ParameterList list;

    ParameterDescriptor desc;
    desc.identifier = "tuning";
    desc.name = "Tuning Frequency";
    desc.unit = "Hz";
    desc.minValue = 420;
    desc.maxValue = 460;
    desc.defaultValue = 440;
    desc.isQuantized = false;
    list.push_back(desc);
    
    desc.identifier = "length";
    desc.name = "Window Length";
    desc.unit = "chroma frames";
    desc.minValue = 1;
    desc.maxValue = 30;
    desc.defaultValue = 10;
    desc.isQuantized = true;
    desc.quantizeStep = 1;
    list.push_back(desc);

    return list;
}

float
KeyDetector::getParameter(std::string param) const
{
    if (param == "tuning") {
        return m_tuningFrequency;
    }
    if (param == "length") {
        return m_length;
    }
    std::cerr << "WARNING: KeyDetect::getParameter: unknown parameter \""
              << param << "\"" << std::endl;
    return 0.0;
}

void
KeyDetector::setParameter(std::string param, float value)
{
    if (param == "tuning") {
        m_tuningFrequency = value;
    } else if (param == "length") {
        m_length = int(value + 0.1);
    } else {
        std::cerr << "WARNING: KeyDetect::setParameter: unknown parameter \""
                  << param << "\"" << std::endl;
    }
}

bool
KeyDetector::initialise(size_t channels, size_t stepSize, size_t blockSize)
{
    if (m_getKeyMode) {
        delete m_getKeyMode;
        m_getKeyMode = 0;
    }

    if (channels < getMinChannelCount() ||
	channels > getMaxChannelCount()) return false;

    m_getKeyMode = new GetKeyMode(int(m_inputSampleRate + 0.1),
                                  m_tuningFrequency,
                                  m_length, m_length);

    m_stepSize = m_getKeyMode->getHopSize();
    m_blockSize = m_getKeyMode->getBlockSize();

    if (stepSize != m_stepSize || blockSize != m_blockSize) {
        std::cerr << "KeyDetector::initialise: step/block sizes "
                  << stepSize << "/" << blockSize << " differ from required "
                  << m_stepSize << "/" << m_blockSize << std::endl;
        delete m_getKeyMode;
        m_getKeyMode = 0;
        return false;
    }

    m_inputFrame = new double[m_blockSize];

    m_prevKey = -1;
	
    return true;
}

void
KeyDetector::reset()
{
    if (m_getKeyMode) {
        delete m_getKeyMode;
        m_getKeyMode = new GetKeyMode(int(m_inputSampleRate + 0.1),
                                      m_tuningFrequency,
                                      m_length, m_length);
    }

    if (m_inputFrame) {
        for( unsigned int i = 0; i < m_blockSize; i++ ) {
            m_inputFrame[ i ] = 0.0;
        }
    }

    m_prevKey = -1;
}


KeyDetector::OutputList
KeyDetector::getOutputDescriptors() const
{
    OutputList list;

    OutputDescriptor d;
    d.identifier = "tonic";
    d.name = "Tonic Pitch";
    d.unit = "";
    d.hasFixedBinCount = true;
    d.binCount = 1;
    d.hasKnownExtents = true;
    d.isQuantized = true;
    d.minValue = 0;
    d.maxValue = 11;
    d.quantizeStep = 1;
    d.sampleType = OutputDescriptor::OneSamplePerStep;
    list.push_back(d);

    d.identifier = "mode";
    d.name = "Key Mode";
    d.unit = "";
    d.hasFixedBinCount = true;
    d.binCount = 1;
    d.hasKnownExtents = true;
    d.isQuantized = true;
    d.minValue = 0;
    d.maxValue = 1;
    d.quantizeStep = 1;
    d.binNames.push_back("Major = 0, Minor = 1");
    d.sampleType = OutputDescriptor::OneSamplePerStep;
    list.push_back(d);

    d.identifier = "key";
    d.name = "Key";
    d.unit = "";
    d.hasFixedBinCount = true;
    d.binCount = 1;
    d.hasKnownExtents = true;
    d.isQuantized = true;
    d.minValue = 0;
    d.maxValue = 23;
    d.quantizeStep = 1;
    d.binNames.erase(d.binNames.begin(),d.binNames.end());
    d.sampleType = OutputDescriptor::OneSamplePerStep;
    list.push_back(d);

    return list;
}

KeyDetector::FeatureSet
KeyDetector::process(const float *const *inputBuffers,
                     Vamp::RealTime now)
{
    if (m_stepSize == 0) {
	return FeatureSet();
    }

    FeatureSet returnFeatures;

    for ( unsigned int i = 0 ; i < m_blockSize; i++ ) {
        m_inputFrame[i] = (double)inputBuffers[0][i];
    }

//    int key = (m_getKeyMode->process(m_inputFrame) % 24);
    int key = m_getKeyMode->process(m_inputFrame);
    int minor = m_getKeyMode->isModeMinor(key);
    int tonic = key;
    if (tonic > 12) tonic -= 12;


    int prevTonic = m_prevKey;
    if (prevTonic > 12) prevTonic -= 12;

    if (tonic != prevTonic) {
        Feature feature;
        feature.hasTimestamp = false;
//        feature.timestamp = now;
        feature.values.push_back((float)tonic);
        feature.label = getKeyName(tonic);
        returnFeatures[0].push_back(feature); // tonic
    }

    if (minor != (m_getKeyMode->isModeMinor(m_prevKey))) {
        Feature feature;
        feature.hasTimestamp = false;
        feature.values.push_back((float)minor);
        feature.label = (minor ? "Minor" : "Major");
        returnFeatures[1].push_back(feature); // mode
    }

    if (key != m_prevKey) {
        Feature feature;
//        feature.hasTimestamp = true;
        feature.hasTimestamp = false;
//        feature.timestamp = now;
        feature.values.push_back((float)key);
        feature.label = std::string(getKeyName(tonic));
        if (minor) feature.label += " minor";
        else feature.label += " major";
        returnFeatures[2].push_back(feature); // key
		cerr << "int key = "<<key<<endl;
		cerr << "int tonic = "<<tonic<<endl;
		cerr << "feature label = "<<feature.label<<endl;
    }

    m_prevKey = key;

    return returnFeatures;
}

KeyDetector::FeatureSet
KeyDetector::getRemainingFeatures()
{
    return FeatureSet();
}


size_t
KeyDetector::getPreferredStepSize() const
{
    if (!m_stepSize) {
        GetKeyMode gkm(int(m_inputSampleRate + 0.1),
                       m_tuningFrequency, m_length, m_length);
        m_stepSize = gkm.getHopSize();
        m_blockSize = gkm.getBlockSize();
    }
    return m_stepSize;
}

size_t
KeyDetector::getPreferredBlockSize() const
{
    if (!m_blockSize) {
        GetKeyMode gkm(int(m_inputSampleRate + 0.1),
                       m_tuningFrequency, m_length, m_length);
        m_stepSize = gkm.getHopSize();
        m_blockSize = gkm.getBlockSize();
    }
    return m_blockSize;
}

const char *
KeyDetector::getKeyName(int index)
{
	// Keys are numbered with 1 => C, 12 => B
	// This is based on chromagram base set to a C in qm-dsp's GetKeyMode.cpp
    static const char *names[] = {
        "C", "C# / Db", "D", "D# / Eb",
        "E", "F", "F# / Gb", "G",
        "G# / Ab", "A", "A# / Bb", "B"
    };
    if (index < 1 || index > 12) {
        return "(unknown)";
    }
    return names[index - 1]; //'-1' because our names array starts from 0 
}

