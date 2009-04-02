/* -*- c-basic-offset: 4 indent-tabs-mode: nil -*-  vi:set ts=8 sts=4 sw=4: */

/*
  QM Vamp Plugin Set

  Centre for Digital Music, Queen Mary, University of London.
  This file copyright 2009 Thomas Wilmering.
  All rights reserved.
*/

#ifndef _DWT_PLUGIN_H_
#define _DWT_PLUGIN_H_

#include <vamp-sdk/Plugin.h>

#include <dsp/wavelet/Wavelet.h>

using std::vector;

class DWT : public Vamp::Plugin
{
public:
    DWT(float inputSampleRate);
    virtual ~DWT();
	
    bool initialise(size_t channels, size_t stepSize, size_t blockSize);
    void reset();
	
    InputDomain getInputDomain() const { return TimeDomain; }
	
    std::string getIdentifier() const;
    std::string getName() const;
    std::string getDescription() const;
    std::string getMaker() const;
    int getPluginVersion() const;
    std::string getCopyright() const;
    size_t getPreferredStepSize() const;
	
    OutputList getOutputDescriptors() const;
		
    ParameterList getParameterDescriptors() const;
    float getParameter(std::string paramid) const;
    void setParameter(std::string paramid, float newval);
	 	
    FeatureSet process(const float *const *inputBuffers,
                       Vamp::RealTime timestamp);
	
    FeatureSet getRemainingFeatures();
	
protected:
    size_t m_stepSize;
    size_t m_blockSize;
    
    int m_scales;
    int m_flength;
    Wavelet::Type m_wavelet;
    float m_threshold;
    float m_absolute;

    vector<float> m_lpd;
    vector<float> m_hpd;
	
    vector< vector<float> > m_samplePass;	
};


#endif
