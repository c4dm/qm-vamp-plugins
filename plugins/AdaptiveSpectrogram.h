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

    struct Spectrogram
    {
        int resolution;
        int width;
        double **data;

        Spectrogram(int r, int w) :
            resolution(r), width(w) {
            data = new double *[width];
            for (int i = 0; i < width; ++i) data[i] = new double[resolution];
        }

        ~Spectrogram() {
            for (int i = 0; i < width; ++i) delete[] data[i];
            delete[] data;
        }            
    };

    struct Spectrograms
    {
        int minres;
        int maxres;
        int n;
        Spectrogram **spectrograms;

        Spectrograms(int mn, int mx, int widthofmax) :
            minres(mn), maxres(mx) {
            n = log2(maxres/minres) + 1;
            spectrograms = new Spectrogram *[n];
            int r = mn;
            for (int i = 0; i < n; ++i) {
                spectrograms[i] = new Spectrogram(r, widthofmax * (mx / r));
                r = r * 2;
            }
        }
        ~Spectrograms() {
            for (int i = 0; i < n; ++i) {
                delete spectrograms[i];
            }
            delete[] spectrograms;
        }
    };

    struct Cutting
    {
        enum Cut { Horizontal, Vertical, Finished };
        Cut cut;
        Cutting *first;
        Cutting *second;
        double cost;
        double value;

        ~Cutting() {
            delete first;
            delete second;
        }
    };

    double cost(const Spectrogram &s, int x, int y) {
        return xlogx(s.data[x][y]);
    }

    double value(const Spectrogram &s, int x, int y) {
        return s.data[x][y];
    }

    Cutting *cut(const Spectrograms &, int res, int x, int y, int h);

    void printCutting(Cutting *, std::string);

    void assemble(const Spectrograms &, const Cutting *, std::vector<std::vector<float> > &, int x, int y, int w, int h);
};


#endif
