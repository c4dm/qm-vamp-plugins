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

#include <dsp/transforms/FFT.h>//!!!

#include "thread/Thread.h"

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

    class FFTThread : public AsynchronousTask
    {
    public:
        FFTThread() { }
        ~FFTThread() { }

        void calculate(const float *timeDomain, Spectrograms &s,
                       int res, int width, int maxwidth) {
            m_in = timeDomain;
            m_s = &s;
            m_res = res;
            m_w = width;
            m_maxwid = maxwidth;
            startTask();
        }

        void await() {
            awaitTask();
        }

    protected:
        void performTask() {

            double *tmpin   = new double[m_w];
            double *tmprout = new double[m_w];
            double *tmpiout = new double[m_w];

            //!!! use window object

            for (int i = 0; i < m_maxwid / m_w; ++i) {
                int origin = m_maxwid/4 - m_w/4; // for 50% overlap
                for (int j = 0; j < m_w; ++j) {
                    double mul = 0.50 - 0.50 * cos((2 * M_PI * j) / m_w);
                    tmpin[j] = m_in[origin + i * m_w/2 + j] * mul;
                }
                FFT::process(m_w, false, tmpin, 0, tmprout, tmpiout);
                for (int j = 0; j < m_w/2; ++j) {
                    int k = j+1; // include Nyquist but not DC
                    double mag = sqrt(tmprout[k] * tmprout[k] +
                                      tmpiout[k] * tmpiout[k]);
                    double scaled = mag / (m_w/2);
                    m_s->spectrograms[m_res]->data[i][j] = scaled;
                }
            }

            delete[] tmpin;
            delete[] tmprout;
            delete[] tmpiout;
        }

    private:
        const float *m_in;
        Spectrograms *m_s;
        int m_res;
        int m_w;
        int m_maxwid;
    };

    std::vector<FFTThread *> m_fftThreads;

    class CutThread : public AsynchronousTask
    {
    public:
        CutThread(const AdaptiveSpectrogram *as) : m_as(as), m_result(0) { }
        ~CutThread() { }
        
        void cut(const Spectrograms &s, int res, int x, int y, int h) {
            m_s = &s;
            m_res = res;
            m_x = x;
            m_y = y;
            m_h = h;
            startTask();
        }

        Cutting *get() {
            awaitTask();
            return m_result;
        }

    protected:
        void performTask() {
            m_result = m_as->cut(*m_s, m_res, m_x, m_y, m_h);
        }

    private:
        const AdaptiveSpectrogram *m_as;
        const Spectrograms *m_s;
        int m_res;
        int m_x;
        int m_y;
        int m_h;
        Cutting *m_result;
    };

    mutable std::vector<CutThread *> m_cutThreads;//!!! mutable blargh

///!!!    Mutex m_threadMutex;
mutable    bool m_first; //!!! gross

    double xlogx(double x) const {
        if (x == 0.0) return 0.0;
        else return x * log(x);
    }

    double cost(const Spectrogram &s, int x, int y) const {
        return xlogx(s.data[x][y]);
    }

    double value(const Spectrogram &s, int x, int y) const {
        return s.data[x][y];
    }

    Cutting *cut(const Spectrograms &, int res, int x, int y, int h) const;

    void getSubCuts(const Spectrograms &, int res, int x, int y, int h,
                    Cutting *&top, Cutting *&bottom,
                    Cutting *&left, Cutting *&right) const;

    void printCutting(Cutting *, std::string) const;

    void assemble(const Spectrograms &, const Cutting *,
                  std::vector<std::vector<float> > &,
                  int x, int y, int w, int h) const;
    };


#endif
