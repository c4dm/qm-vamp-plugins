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

    class CutThread : public Thread
    {
    public:
        CutThread(const AdaptiveSpectrogram *as) :
            m_as(as),
//            m_busy(false),
//            m_computed(false),
            m_result(0),
            m_workToDoC("CutThread: work to do"),
            m_workToDo(false),
            m_workDoneC("CutThread: work done"),
            m_workDone(false),
            m_finishing(false)
        { }
        ~CutThread() { }

        void cut(const Spectrograms &s, int res, int x, int y, int h) {
            m_workToDoC.lock();
//            std::cerr << "locked in main thread" << std::endl;
            m_s = &s;
            m_res = res;
            m_x = x;
            m_y = y;
            m_h = h;
//            m_busy = true;
//            m_computed = false;
            m_workToDo = true;
            m_workDone = false;
            m_workToDoC.signal();
            m_workDoneC.lock();
            m_workToDoC.unlock();
        }

        Cutting *get() {
//            std::cerr << "about to wait within main thread" << std::endl;
            // m_workDoneC must be locked from prior call to cut()
            while (!m_workDone) m_workDoneC.wait();
//            std::cerr << "waited within main thread" << std::endl;
//            m_workDoneC.lock();
//            while (!m_computed) {
//                std::cerr << "waiting within main thread" << std::endl;
//                m_condition.wait();
//            }
            Cutting *c = m_result;
            m_result = 0;
            m_workDoneC.unlock();
            return c;
        }
/*
        bool busy() {
            return m_busy;
        }
*/
        void finish() {
            m_finishing = true;
            m_workToDoC.signal();
        }

    protected:
        virtual void run() {
            m_workToDoC.lock();
//            std::cerr << "locked within run function" << std::endl;
            while (!m_finishing) {
//                if (!m_busy) {
//                    std::cerr << "waiting within run function" << std::endl;
//                    m_condition.wait();
//                }
                    while (!m_workToDo && !m_finishing) m_workToDoC.wait();
//                    std::cerr << "waited within run function" << std::endl;
                if (m_finishing) {
                    break;
                }
                if (m_workToDo) {
//                    std::cerr << "cut thread " << this << ": calling cut" << std::endl;
                    m_result = m_as->cut(*m_s, m_res, m_x, m_y, m_h);
//                    std::cerr << "cut returning" << std::endl;
//                    m_computed = true;
//                    m_busy = false;
                    m_workToDo = false;
                    m_workDone = true;
//                    std::cerr << "signalling completion from run function" << std::endl;
                    m_workDoneC.signal();
                }
            }
            m_workToDoC.unlock();
        }

        const AdaptiveSpectrogram *m_as;
        const Spectrograms *m_s;
        int m_res;
        int m_x;
        int m_y;
        int m_h;
//        bool m_busy;
//        bool m_computed;
        Cutting *m_result;
        Condition m_workToDoC;
        bool m_workToDo;
        Condition m_workDoneC;
        bool m_workDone;
        bool m_finishing;
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
