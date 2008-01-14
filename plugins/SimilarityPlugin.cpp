/* -*- c-basic-offset: 4 indent-tabs-mode: nil -*-  vi:set ts=8 sts=4 sw=4: */

/*
 * SegmenterPlugin.cpp
 *
 * Copyright 2008 Centre for Digital Music, Queen Mary, University of London.
 * All rights reserved.
 */

#include <iostream>
#include <sstream>

#include "SimilarityPlugin.h"
#include "base/Pitch.h"
#include "dsp/mfcc/MFCC.h"
#include "dsp/chromagram/Chromagram.h"
#include "dsp/rateconversion/Decimator.h"

using std::string;
using std::vector;
using std::cerr;
using std::endl;
using std::ostringstream;

SimilarityPlugin::SimilarityPlugin(float inputSampleRate) :
    Plugin(inputSampleRate),
    m_type(TypeMFCC),
    m_mfcc(0),
    m_chromagram(0),
    m_decimator(0),
    m_featureColumnSize(20),
    m_blockSize(0),
    m_channels(0)
{
	
}

SimilarityPlugin::~SimilarityPlugin()
{
    delete m_mfcc;
    delete m_chromagram;
    delete m_decimator;
}

string
SimilarityPlugin::getIdentifier() const
{
    return "qm-similarity";
}

string
SimilarityPlugin::getName() const
{
    return "Similarity";
}

string
SimilarityPlugin::getDescription() const
{
    return "Return a distance matrix for similarity between the input audio channels";
}

string
SimilarityPlugin::getMaker() const
{
    return "Chris Cannam, Queen Mary, University of London";
}

int
SimilarityPlugin::getPluginVersion() const
{
    return 1;
}

string
SimilarityPlugin::getCopyright() const
{
    return "Copyright (c) 2008 - All Rights Reserved";
}

size_t
SimilarityPlugin::getMinChannelCount() const
{
    return 2;
}

size_t
SimilarityPlugin::getMaxChannelCount() const
{
    return 1024;
}

bool
SimilarityPlugin::initialise(size_t channels, size_t stepSize, size_t blockSize)
{
    if (channels < getMinChannelCount() ||
	channels > getMaxChannelCount()) return false;

    if (stepSize != getPreferredStepSize()) {
        std::cerr << "SimilarityPlugin::initialise: supplied step size "
                  << stepSize << " differs from required step size "
                  << getPreferredStepSize() << std::endl;
        return false;
    }

    if (blockSize != getPreferredBlockSize()) {
        std::cerr << "SimilarityPlugin::initialise: supplied block size "
                  << blockSize << " differs from required block size "
                  << getPreferredBlockSize() << std::endl;
        return false;
    }        
    
    m_blockSize = blockSize;
    m_channels = channels;

    int decimationFactor = getDecimationFactor();
    if (decimationFactor > 1) {
        m_decimator = new Decimator(m_blockSize, decimationFactor);
    }

    if (m_type == TypeMFCC) {

        m_featureColumnSize = 20;

        MFCCConfig config;
        config.FS = lrintf(m_inputSampleRate) / decimationFactor;
        config.fftsize = 2048;
        config.nceps = m_featureColumnSize - 1;
        config.want_c0 = true;
        m_mfcc = new MFCC(config);
        m_fftSize = m_mfcc->getfftlength();

    } else if (m_type == TypeChroma) {

        m_featureColumnSize = 12;

        ChromaConfig config;
        config.FS = lrintf(m_inputSampleRate) / decimationFactor;
        config.min = Pitch::getFrequencyForPitch(24, 0, 440);
        config.max = Pitch::getFrequencyForPitch(96, 0, 440);
        config.BPO = 12;
        config.CQThresh = 0.0054;
        config.isNormalised = true;
        m_chromagram = new Chromagram(config);
        m_fftSize = m_chromagram->getFrameSize();

        std::cerr << "min = "<< config.min << ", max = " << config.max << std::endl;

    } else {

        std::cerr << "SimilarityPlugin::initialise: internal error: unknown type " << m_type << std::endl;
        return false;
    }
    
    for (int i = 0; i < m_channels; ++i) {
        m_values.push_back(FeatureMatrix());
    }

    return true;
}

void
SimilarityPlugin::reset()
{
    //!!!
}

int
SimilarityPlugin::getDecimationFactor() const
{
    int rate = lrintf(m_inputSampleRate);
    int internalRate = 22050;
    int decimationFactor = rate / internalRate;
    if (decimationFactor < 1) decimationFactor = 1;

    // must be a power of two
    while (decimationFactor & (decimationFactor - 1)) ++decimationFactor;

    return decimationFactor;
}

size_t
SimilarityPlugin::getPreferredStepSize() const
{
    if (m_blockSize == 0) calculateBlockSize();
    return m_blockSize/2;
}

size_t
SimilarityPlugin::getPreferredBlockSize() const
{
    if (m_blockSize == 0) calculateBlockSize();
    return m_blockSize;
}

void
SimilarityPlugin::calculateBlockSize() const
{
    if (m_blockSize != 0) return;
    int decimationFactor = getDecimationFactor();
    if (m_type == TypeChroma) {
        ChromaConfig config;
        config.FS = lrintf(m_inputSampleRate) / decimationFactor;
        config.min = Pitch::getFrequencyForPitch(24, 0, 440);
        config.max = Pitch::getFrequencyForPitch(96, 0, 440);
        config.BPO = 12;
        config.CQThresh = 0.0054;
        config.isNormalised = false;
        Chromagram *c = new Chromagram(config);
        size_t sz = c->getFrameSize();
        delete c;
        m_blockSize = sz * decimationFactor;
    } else {
        m_blockSize = 2048 * decimationFactor;
    }
}

SimilarityPlugin::ParameterList SimilarityPlugin::getParameterDescriptors() const
{
    ParameterList list;

    ParameterDescriptor desc;
    desc.identifier = "featureType";
    desc.name = "Feature Type";
    desc.description = "";//!!!
    desc.unit = "";
    desc.minValue = 0;
    desc.maxValue = 1;
    desc.defaultValue = 0;
    desc.isQuantized = true;
    desc.quantizeStep = 1;
    desc.valueNames.push_back("Timbral (MFCC)");
    desc.valueNames.push_back("Chromatic (Chroma)");
    list.push_back(desc);	
	
    return list;
}

float
SimilarityPlugin::getParameter(std::string param) const
{
    if (param == "featureType") {
        if (m_type == TypeMFCC) return 0;
        else if (m_type == TypeChroma) return 1;
        else return 0;
    }

    std::cerr << "WARNING: SimilarityPlugin::getParameter: unknown parameter \""
              << param << "\"" << std::endl;
    return 0.0;
}

void
SimilarityPlugin::setParameter(std::string param, float value)
{
    if (param == "featureType") {
        int v = int(value + 0.1);
        Type prevType = m_type;
        if (v == 0) m_type = TypeMFCC;
        else if (v == 1) m_type = TypeChroma;
        if (m_type != prevType) m_blockSize = 0;
        return;
    }

    std::cerr << "WARNING: SimilarityPlugin::setParameter: unknown parameter \""
              << param << "\"" << std::endl;
}

SimilarityPlugin::OutputList
SimilarityPlugin::getOutputDescriptors() const
{
    OutputList list;
	
    OutputDescriptor similarity;
    similarity.identifier = "distance";
    similarity.name = "Distance";
    similarity.description = "Distance Metric for Similarity (smaller = more similar)";
    similarity.unit = "";
    similarity.hasFixedBinCount = true;
    similarity.binCount = m_channels;
    similarity.hasKnownExtents = false;
    similarity.isQuantized = false;
    similarity.sampleType = OutputDescriptor::FixedSampleRate;
    similarity.sampleRate = 1;
	
    list.push_back(similarity);
	
    OutputDescriptor means;
    means.identifier = "means";
    means.name = "Feature Means";
    means.description = "";
    means.unit = "";
    means.hasFixedBinCount = true;
    means.binCount = m_channels;
    means.hasKnownExtents = false;
    means.isQuantized = false;
    means.sampleType = OutputDescriptor::VariableSampleRate;
    means.sampleRate = m_inputSampleRate / getPreferredStepSize();
	
    list.push_back(means);
    
    OutputDescriptor variances;
    variances.identifier = "variances";
    variances.name = "Feature Variances";
    variances.description = "";
    variances.unit = "";
    variances.hasFixedBinCount = true;
    variances.binCount = m_channels;
    variances.hasKnownExtents = false;
    variances.isQuantized = false;
    variances.sampleType = OutputDescriptor::VariableSampleRate;
    variances.sampleRate = m_inputSampleRate / getPreferredStepSize();
	
    list.push_back(variances);
    
    return list;
}

SimilarityPlugin::FeatureSet
SimilarityPlugin::process(const float *const *inputBuffers, Vamp::RealTime /* timestamp */)
{
    double *dblbuf = new double[m_blockSize];
    double *decbuf = dblbuf;
    if (m_decimator) decbuf = new double[m_fftSize];

    double *raw = 0;
    bool ownRaw = false;

    if (m_type == TypeMFCC) {
        raw = new double[m_featureColumnSize];
        ownRaw = true;
    }

    for (size_t c = 0; c < m_channels; ++c) {

        for (int i = 0; i < m_blockSize; ++i) {
            dblbuf[i] = inputBuffers[c][i];
        }

        if (m_decimator) {
            m_decimator->process(dblbuf, decbuf);
        }

        if (m_type == TypeMFCC) {
            m_mfcc->process(m_fftSize, decbuf, raw);
        } else if (m_type == TypeChroma) {
            raw = m_chromagram->process(decbuf);
        }                
        
        FeatureColumn mf(m_featureColumnSize);
        for (int i = 0; i < m_featureColumnSize; ++i) mf[i] = raw[i];

        m_values[c].push_back(mf);
    }

    if (m_decimator) delete[] decbuf;
    delete[] dblbuf;

    if (ownRaw) delete[] raw;
	
    return FeatureSet();
}

SimilarityPlugin::FeatureSet
SimilarityPlugin::getRemainingFeatures()
{
    std::vector<FeatureColumn> m(m_channels);
    std::vector<FeatureColumn> v(m_channels);
    
    for (int i = 0; i < m_channels; ++i) {

        FeatureColumn mean(m_featureColumnSize), variance(m_featureColumnSize);

        for (int j = 0; j < m_featureColumnSize; ++j) {

            mean[j] = variance[j] = 0.0;
            int count;

//            std::cout << i << "," << j << ":" << std::endl;

            count = 0;
            for (int k = 0; k < m_values[i].size(); ++k) {
                double val = m_values[i][k][j];
//                std::cout << val << " ";
                if (isnan(val) || isinf(val)) continue;
                mean[j] += val;
                ++count;
            }
            if (count > 0) mean[j] /= count;

//            std::cout << std::endl;

            count = 0;
            for (int k = 0; k < m_values[i].size(); ++k) {
                double val = ((m_values[i][k][j] - mean[j]) *
                              (m_values[i][k][j] - mean[j]));
                if (isnan(val) || isinf(val)) continue;
                variance[j] += val;
                ++count;
            }
            if (count > 0) variance[j] /= count;
        }

        m[i] = mean;
        v[i] = variance;
    }

    // we want to return a matrix of the distances between channels,
    // but Vamp doesn't have a matrix return type so we actually
    // return a series of vectors

    std::vector<std::vector<double> > distances;

    // "Despite the fact that MFCCs extracted from music are clearly
    // not Gaussian, [14] showed, somewhat surprisingly, that a
    // similarity function comparing single Gaussians modelling MFCCs
    // for each track can perform as well as mixture models.  A great
    // advantage of using single Gaussians is that a simple closed
    // form exists for the KL divergence." -- Mark Levy, "Lightweight
    // measures for timbral similarity of musical audio"
    // (http://www.elec.qmul.ac.uk/easaier/papers/mlevytimbralsimilarity.pdf)
    //
    // This code calculates a symmetrised distance metric based on the
    // KL divergence of Gaussian models of the MFCC values.

    for (int i = 0; i < m_channels; ++i) {
        distances.push_back(std::vector<double>());
        for (int j = 0; j < m_channels; ++j) {
            double d = -2.0 * m_featureColumnSize;
            for (int k = 0; k < m_featureColumnSize; ++k) {
                // m[i][k] is the mean of feature bin k for channel i
                // v[i][k] is the variance of feature bin k for channel i
                d += v[i][k] / v[j][k] + v[j][k] / v[i][k];
                d += (m[i][k] - m[j][k])
                    * (1.0 / v[i][k] + 1.0 / v[j][k])
                    * (m[i][k] - m[j][k]);
            }
            d /= 2.0;
            distances[i].push_back(d);
        }
    }

    FeatureSet returnFeatures;

    for (int i = 0; i < m_channels; ++i) {

        Feature feature;
        feature.hasTimestamp = true; // otherwise hosts will tend to stamp them at the end of the file, which is annoying
        feature.timestamp = Vamp::RealTime(i, 0);

        feature.values.clear();
        for (int k = 0; k < m_featureColumnSize; ++k) {
            feature.values.push_back(m[i][k]);
        }

        returnFeatures[1].push_back(feature);

        feature.values.clear();
        for (int k = 0; k < m_featureColumnSize; ++k) {
            feature.values.push_back(v[i][k]);
        }

        returnFeatures[2].push_back(feature);

        feature.values.clear();
        for (int j = 0; j < m_channels; ++j) {
            feature.values.push_back(distances[i][j]);
        }
        ostringstream oss;
        oss << "Distance from " << (i + 1);
        feature.label = oss.str();
		
        returnFeatures[0].push_back(feature);
    }

    return returnFeatures;
}
