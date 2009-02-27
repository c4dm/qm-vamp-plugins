/* -*- c-basic-offset: 4 indent-tabs-mode: nil -*-  vi:set ts=8 sts=4 sw=4: */

/*
    QM Vamp Plugin Set

    Centre for Digital Music, Queen Mary, University of London.
    All rights reserved.
*/

#ifndef _ADAPTIVE_SPECTROGRAM_H_
#define _ADAPTIVE_SPECTROGRAM_H_

#include <vamp-sdk/Plugin.h>
#include <cmath>
#include <vector>

class AdaptiveSpectrogram : public Vamp::Plugin
{
public:
    AdaptiveSpectrogram(float inputSampleRate);
    virtual ~AdaptiveSpectrogram();

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
    size_t getPreferredBlockSize() const;

    ParameterList getParameterDescriptors() const;
    float getParameter(std::string id) const;
    void setParameter(std::string id, float value);

    OutputList getOutputDescriptors() const;

    FeatureSet process(const float *const *inputBuffers,
                       Vamp::RealTime timestamp);

    FeatureSet getRemainingFeatures();

protected:
    int m_w;
    int m_n;

    inline double xlogx(double x) {
        if (x == 0.0) return 0.0;
        else return x * log(x);
    }

    void unpackResultMatrix(std::vector<std::vector<float> > &rmat,
                            int x, int y, int w, int h,
                            int *spl,
                            double *spec, int specsz, int res);

    double DoCutSpectrogramBlock2(int* spl, double*** Specs, int Y, int R0,
                                  int x0, int Y0, int N, double& ene);

    double DoMixSpectrogramBlock2(int* spl, double* Spec, double*** Specs,
                                  int Y, int R0, int x0, int Y0,
                                  bool normmix, int res, double* e);

    double MixSpectrogramBlock2(int* spl, double* Spec, double*** Specs,
                                int WID, int wid, bool normmix);

    double MixSpectrogram2(int** spl, double** Spec, double*** Specs, int Fr,
                           int WID, int wid, bool norm, bool normmix);
};


#endif
