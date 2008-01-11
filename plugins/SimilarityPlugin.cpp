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
#include "dsp/mfcc/MFCC.h"
#include "dsp/rateconversion/Decimator.h"

using std::string;
using std::vector;
using std::cerr;
using std::endl;
using std::ostringstream;

SimilarityPlugin::SimilarityPlugin(float inputSampleRate) :
    Plugin(inputSampleRate),
    m_mfcc(0),
    m_decimator(0),
    m_K(20),
    m_blockSize(0),
    m_channels(0)
{
	
}

SimilarityPlugin::~SimilarityPlugin()
{
    delete m_mfcc;
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
    return "Return a distance metric for overall timbral similarity between the input audio channels";
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
        m_decimator = new Decimator(getPreferredBlockSize(), decimationFactor);
    }

    MFCCConfig config;
    config.FS = lrintf(m_inputSampleRate) / decimationFactor;
    config.fftsize = 2048;
    config.nceps = m_K - 1;
    config.want_c0 = true;
    m_mfcc = new MFCC(config);
    
    for (int i = 0; i < m_channels; ++i) {
        m_mfeatures.push_back(MFCCFeatureVector());
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
    return 1024 * getDecimationFactor();
}

size_t
SimilarityPlugin::getPreferredBlockSize() const
{
    return 2048 * getDecimationFactor();
}

SimilarityPlugin::ParameterList SimilarityPlugin::getParameterDescriptors() const
{
    ParameterList list;
    return list;
}

float
SimilarityPlugin::getParameter(std::string param) const
{
    std::cerr << "WARNING: SimilarityPlugin::getParameter: unknown parameter \""
              << param << "\"" << std::endl;
    return 0.0;
}

void
SimilarityPlugin::setParameter(std::string param, float value)
{
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
    similarity.description = "Distance Metric for Timbral Similarity (smaller = more similar)";
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
    means.name = "MFCC Means";
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
    variances.name = "MFCC Variances";
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
    if (m_decimator) decbuf = new double[m_mfcc->getfftlength()];
    double *ceps = new double[m_K];

    for (size_t c = 0; c < m_channels; ++c) {

        for (int i = 0; i < m_blockSize; ++i) {
            dblbuf[i] = inputBuffers[c][i];
        }

        if (m_decimator) {
            m_decimator->process(dblbuf, decbuf);
        }
        
        m_mfcc->process(m_mfcc->getfftlength(), decbuf, ceps);
        
        MFCCFeature mf(m_K);
        for (int i = 0; i < m_K; ++i) mf[i] = ceps[i];

        m_mfeatures[c].push_back(mf);
    }

    if (m_decimator) delete[] decbuf;
    delete[] dblbuf;
    delete[] ceps;
	
    return FeatureSet();
}

SimilarityPlugin::FeatureSet
SimilarityPlugin::getRemainingFeatures()
{
    std::vector<MFCCFeature> m(m_channels);
    std::vector<MFCCFeature> v(m_channels);
    
    //!!! bail if m_mfeatures vectors are empty

    for (int i = 0; i < m_channels; ++i) {

        MFCCFeature mean(m_K), variance(m_K);

        for (int j = 0; j < m_K; ++j) {

            mean[j] = variance[j] = 0.0;
            int count;

            count = 0;
            for (int k = 0; k < m_mfeatures[i].size(); ++k) {
                double val = m_mfeatures[i][k][j];
//                std::cout << "val = " << val << std::endl;
                if (isnan(val) || isinf(val)) continue;
                mean[j] += val;
//                std::cout << "mean now = " << mean[j] << std::endl;
                ++count;
            }
            if (count > 0) mean[j] /= count;
//            std::cout << "divided by " << count << ", mean now " << mean[j] << std::endl;

            count = 0;
            for (int k = 0; k < m_mfeatures[i].size(); ++k) {
                double val = ((m_mfeatures[i][k][j] - mean[j]) *
                              (m_mfeatures[i][k][j] - mean[j]));
                if (isnan(val) || isinf(val)) continue;
                variance[j] += val;
                ++count;
            }
            if (count > 0) variance[j] /= count;
        }

        m[i] = mean;
        v[i] = variance;
    }

//    std::cout << "m[0][0] = " << m[0][0] << std::endl;

    // so we sorta return a matrix of the distances between channels,
    // but Vamp doesn't have a matrix return type so we actually
    // return a series of vectors

    std::vector<std::vector<double> > distances;

    for (int i = 0; i < m_channels; ++i) {
        distances.push_back(std::vector<double>());
        for (int j = 0; j < m_channels; ++j) {
            double d = -2.0 * m_K;
            for (int k = 0; k < m_K; ++k) {
                // m[i][k] is the mean of mfcc k for channel i
                // v[i][k] is the variance of mfcc k for channel i
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
        for (int k = 0; k < m_K; ++k) {
            feature.values.push_back(m[i][k]);
        }

        returnFeatures[1].push_back(feature);

        feature.values.clear();
        for (int k = 0; k < m_K; ++k) {
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
