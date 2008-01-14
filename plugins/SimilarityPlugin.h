/* -*- c-basic-offset: 4 indent-tabs-mode: nil -*-  vi:set ts=8 sts=4 sw=4: */

/*
 * SimilarityPlugin.h
 *
 * Copyright 2008 Centre for Digital Music, Queen Mary, University of London.
 * All rights reserved.
 */

#ifndef _SIMILARITY_PLUGIN_H_
#define _SIMILARITY_PLUGIN_H_

#include <vamp-sdk/Plugin.h>
#include <vamp-sdk/RealTime.h>

class MFCC;
class Chromagram;
class Decimator;

class SimilarityPlugin : public Vamp::Plugin
{
public:
    SimilarityPlugin(float inputSampleRate);
    virtual ~SimilarityPlugin();
	
    bool initialise(size_t channels, size_t stepSize, size_t blockSize);
    void reset();
	
    std::string getIdentifier() const;
    std::string getName() const;
    std::string getDescription() const;
    std::string getMaker() const;
    int getPluginVersion() const;
    std::string getCopyright() const;
	
    size_t getPreferredStepSize() const;
    size_t getPreferredBlockSize() const;
    InputDomain getInputDomain() const { return TimeDomain; }
    
    size_t getMinChannelCount() const;
    size_t getMaxChannelCount() const;

    SimilarityPlugin::ParameterList getParameterDescriptors() const;
    float getParameter(std::string param) const;
    void setParameter(std::string param, float value);
    
    OutputList getOutputDescriptors() const;
    
    FeatureSet process(const float *const *inputBuffers, Vamp::RealTime timestamp);
    
    FeatureSet getRemainingFeatures();
	
protected:
    int getDecimationFactor() const;
    
    enum Type {
        TypeMFCC,
        TypeChroma
    };

    void calculateBlockSize() const;

    Type m_type;
    MFCC *m_mfcc;
    Chromagram *m_chromagram;
    Decimator *m_decimator;
    int m_featureColumnSize;
    mutable size_t m_blockSize;
    size_t m_fftSize;
    int m_channels;

    typedef std::vector<double> FeatureColumn;
    typedef std::vector<FeatureColumn> FeatureMatrix;
    typedef std::vector<FeatureMatrix> FeatureMatrixSet;

    FeatureMatrixSet m_values;
};

#endif

