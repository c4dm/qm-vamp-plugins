/* -*- c-basic-offset: 4 indent-tabs-mode: nil -*-  vi:set ts=8 sts=4 sw=4: */

/*
    QM Vamp Plugin Set

    Centre for Digital Music, Queen Mary, University of London.
    All rights reserved.
*/

#ifndef _TONALCHANGEDETECT_
#define _TONALCHANGEDETECT_

#include <vamp-sdk/Plugin.h>

#include <dsp/chromagram/Chromagram.h>
#include <dsp/tonal/TonalEstimator.h>
#include <dsp/tonal/TCSgram.h>

#include <queue>
#include <vector>
#include <valarray>

class TonalChangeDetect : public Vamp::Plugin
{
public:
	TonalChangeDetect(float fInputSampleRate);
	virtual ~TonalChangeDetect();

    bool initialise(size_t channels, size_t stepSize, size_t blockSize);
    void reset();

    InputDomain getInputDomain() const { return TimeDomain; }

    std::string getName() const;
    std::string getDescription() const;
    std::string getMaker() const;
    int getPluginVersion() const;
    std::string getCopyright() const;

    ParameterList getParameterDescriptors() const;
    float getParameter(std::string) const;
    void setParameter(std::string, float);


    size_t getPreferredStepSize() const;
    size_t getPreferredBlockSize() const;

    OutputList getOutputDescriptors() const;

    FeatureSet process(const float *const *inputBuffers,
                       Vamp::RealTime timestamp);

    FeatureSet getRemainingFeatures();
	
private:
	void setupConfig();

    ChromaConfig m_config;
    Chromagram *m_chromagram;
	TonalEstimator m_TonalEstimator;
    mutable size_t m_step;
    mutable size_t m_block;
    size_t m_stepDelay;
    std::queue<ChromaVector> m_pending;
	ChromaVector m_vaCurrentVector;
	TCSGram m_TCSGram;
	
private:
	int m_iSmoothingWidth;  // smoothing window size
	
    int m_minMIDIPitch;     // chromagram parameters
    int m_maxMIDIPitch;
    float m_tuningFrequency;
	
};


#endif // _TONALCHANGEDETECT_
