/* -*- c-basic-offset: 4 indent-tabs-mode: nil -*-  vi:set ts=8 sts=4 sw=4: */

/*
    QM Vamp Plugin Set

    Centre for Digital Music, Queen Mary, University of London.
    All rights reserved.
*/

#include "AdaptiveSpectrogram.h"

#include <cstdlib>
#include <cstring>

#include <iostream>

#include <dsp/transforms/FFT.h>

using std::string;
using std::vector;
using std::cerr;
using std::endl;

using Vamp::RealTime;

AdaptiveSpectrogram::AdaptiveSpectrogram(float inputSampleRate) :
    Plugin(inputSampleRate),
    m_w(9),
    m_n(2)
{
}

AdaptiveSpectrogram::~AdaptiveSpectrogram()
{
}

string
AdaptiveSpectrogram::getIdentifier() const
{
    return "adaptivespectrogram";
}

string
AdaptiveSpectrogram::getName() const
{
    return "Adaptive Spectrogram";
}

string
AdaptiveSpectrogram::getDescription() const
{
    return "Produce an adaptive spectrogram by adaptive selection from spectrograms at multiple resolutions";
}

string
AdaptiveSpectrogram::getMaker() const
{
    return "Queen Mary, University of London";
}

int
AdaptiveSpectrogram::getPluginVersion() const
{
    return 1;
}

string
AdaptiveSpectrogram::getCopyright() const
{
    return "Plugin by Wen Xue and Chris Cannam.  Copyright (c) 2009 Wen Xue and QMUL - All Rights Reserved";
}

size_t
AdaptiveSpectrogram::getPreferredStepSize() const
{
    return ((2 << m_w) << m_n) / 2;
}

size_t
AdaptiveSpectrogram::getPreferredBlockSize() const
{
    return (2 << m_w) << m_n;
}

bool
AdaptiveSpectrogram::initialise(size_t channels, size_t stepSize, size_t blockSize)
{
    if (channels < getMinChannelCount() ||
	channels > getMaxChannelCount()) return false;

    return true;
}

void
AdaptiveSpectrogram::reset()
{

}

AdaptiveSpectrogram::ParameterList
AdaptiveSpectrogram::getParameterDescriptors() const
{
    ParameterList list;

    ParameterDescriptor desc;
    desc.identifier = "n";
    desc.name = "Number of resolutions";
    desc.description = "Number of consecutive powers of two to use as spectrogram resolutions, starting with the minimum resolution specified";
    desc.unit = "";
    desc.minValue = 1;
    desc.maxValue = 10;
    desc.defaultValue = 3;
    desc.isQuantized = true;
    desc.quantizeStep = 1;
    list.push_back(desc);

    ParameterDescriptor desc2;
    desc2.identifier = "w";
    desc2.name = "Smallest resolution";
    desc2.description = "Smallest of the consecutive powers of two to use as spectrogram resolutions";
    desc2.unit = "";
    desc2.minValue = 1;
    desc2.maxValue = 14;
    desc2.defaultValue = 10;
    desc2.isQuantized = true;
    desc2.quantizeStep = 1;
    // I am so lazy
    desc2.valueNames.push_back("2");
    desc2.valueNames.push_back("4");
    desc2.valueNames.push_back("8");
    desc2.valueNames.push_back("16");
    desc2.valueNames.push_back("32");
    desc2.valueNames.push_back("64");
    desc2.valueNames.push_back("128");
    desc2.valueNames.push_back("256");
    desc2.valueNames.push_back("512");
    desc2.valueNames.push_back("1024");
    desc2.valueNames.push_back("2048");
    desc2.valueNames.push_back("4096");
    desc2.valueNames.push_back("8192");
    desc2.valueNames.push_back("16384");
    list.push_back(desc2);

    return list;
}

float
AdaptiveSpectrogram::getParameter(std::string id) const
{
    if (id == "n") return m_n+1;
    else if (id == "w") return m_w+1;
    return 0.f;
}

void
AdaptiveSpectrogram::setParameter(std::string id, float value)
{
    if (id == "n") {
        int n = lrintf(value);
        if (n >= 1 && n <= 10) m_n = n-1;
    } else if (id == "w") {
        int w = lrintf(value);
        if (w >= 1 && w <= 14) m_w = w-1;
    }        
}

AdaptiveSpectrogram::OutputList
AdaptiveSpectrogram::getOutputDescriptors() const
{
    OutputList list;

    OutputDescriptor d;
    d.identifier = "output";
    d.name = "Output";
    d.description = "The output of the plugin";
    d.unit = "";
    d.hasFixedBinCount = true;
    d.binCount = ((2 << m_w) << m_n) / 2;
    d.hasKnownExtents = false;
    d.isQuantized = false;
    d.sampleType = OutputDescriptor::FixedSampleRate;
    d.sampleRate = m_inputSampleRate / ((2 << m_w) / 2);
    d.hasDuration = false;
    list.push_back(d);

    return list;
}

AdaptiveSpectrogram::FeatureSet
AdaptiveSpectrogram::getRemainingFeatures()
{
    FeatureSet fs;
    return fs;
}

AdaptiveSpectrogram::FeatureSet
AdaptiveSpectrogram::process(const float *const *inputBuffers, RealTime ts)
{
    FeatureSet fs;

    int wid = (2 << m_w), WID = ((2 << m_w) << m_n);
    int Res = log2(WID/wid)+1;
    double ***specs = new double **[Res];
    int Wid = WID;
    int wi = 0;

    cerr << "wid = " << wid << ", WID = " << WID << endl;

    double *tmpin  = new double[WID];
    double *tmprout = new double[WID];
    double *tmpiout = new double[WID];

    while (Wid >= wid) {
        specs[wi] = new double *[WID/Wid];
        for (int i = 0; i < WID/Wid; ++i) {
            specs[wi][i] = new double[Wid/2];
            int origin = WID/4 - Wid/4; // for 50% overlap
            for (int j = 0; j < Wid; ++j) {
                double mul = 0.50 - 0.50 * cos((2 * M_PI * j) / Wid);
                tmpin[j] = inputBuffers[0][origin + i * Wid/2 + j] * mul;
            }
            FFT::process(Wid, false, tmpin, 0, tmprout, tmpiout);
            for (int j = 0; j < Wid/2; ++j) {
                double mag =
                    tmprout[j] * tmprout[j] +
                    tmpiout[j] * tmpiout[j];
                specs[wi][i][j] = sqrt(mag) / Wid;
            }
        }
        Wid /= 2;
        ++wi;
    }

    int *spl = new int[WID/2];
    double *spec = new double[WID/2];

    // This prefill makes it easy to see which elements are actually
    // set by the MixSpectrogramBlock2 call.  Turns out that, with
    // 1024, 2048 and 4096 as our widths, the spl array has elements
    // 0-4094 (incl) filled in and the spec array has elements 0-4095
    
    for (int i = 0; i < WID/2; ++i) {
        spl[i] = i;
        spec[i] = i;
    }

    MixSpectrogramBlock2(spl, spec, specs, WID/2, wid/2, false);

    Wid = WID;
    wi = 0;
    while (Wid >= wid) {
        for (int i = 0; i < WID/Wid; ++i) {
            delete[] specs[wi][i];
        }
        delete[] specs[wi];
        Wid /= 2;
        ++wi;
    }
    delete[] specs;

    std::cerr << "Results at " << ts << ":" << std::endl;
/*    for (int i = 0; i < WID/2; ++i) {
        if (spl[i] == i || spec[i] == i) {
            std::cerr << "\n***\n";
        }
        std::cerr << "[" << i << "] " << spl[i] << "," << spec[i] << " ";
    }
    std::cerr << std::endl;
*/
    vector<vector<float> > rmat(WID/wid);
    for (int i = 0; i < WID/wid; ++i) {
        rmat[i] = vector<float>(WID/2);
    }
    
    int y = 0, h = WID/2;
    int x = 0, w = WID/wid;
    unpackResultMatrix(rmat, x, y, w, h, spl, spec, WID/2, WID);

    delete[] spec;
    delete[] spl;

    for (int i = 0; i < rmat.size(); ++i) {
        Feature f;
        f.hasTimestamp = false;
        f.values = rmat[i];
        fs[0].push_back(f);
    }

/*
    if (m_stepSize == 0) {
	cerr << "ERROR: AdaptiveSpectrogram::process: "
	     << "AdaptiveSpectrogram has not been initialised"
	     << endl;
	return fs;
    }
*/
    return fs;
}

void
AdaptiveSpectrogram::unpackResultMatrix(vector<vector<float> > &rmat,
                                  int x, int y, int w, int h,
                                  int *spl,
                                  double *spec, int sz,
                                  int res
    )
{

    cerr << "x = " << x << ", y = " << y << ", w = " << w << ", h = " << h 
         << ", sz = " << sz << ", *spl = " << *spl << ", *spec = " << *spec << ", res = " << res <<  endl;

    if (sz <= 1) {

        for (int i = 0; i < w; ++i) {
            for (int j = 0; j < h; ++j) {
//                rmat[x+i][y+j] = (off ? 0 : *spec);
                if (rmat[x+i][y+j] != 0) {
                    cerr << "WARNING: Overwriting value " << rmat[x+i][y+j] 
                         << " with " << res + i + j << " at " << x+i << "," << y+j << endl;
                }
//                cerr << "[" << x+i << "][" << y+j << "] <= " << res+i+j << endl;
                rmat[x+i][y+j] = *spec;
            }
        }

//        cerr << " (done)" << endl;
        return;
    }
//    cerr << endl;

    if (*spl == 0) {

        unpackResultMatrix(rmat,
                           x, y,
                           w, h/2,
                           spl + 1,
                           spec,
                           sz/2,
                           res);

        unpackResultMatrix(rmat,
                           x, y + h/2,
                           w, h/2,
                           spl + sz/2,
                           spec + sz/2,
                           sz/2,
                           res);

    } else if (*spl == 1) {

        unpackResultMatrix(rmat,
                           x, y,
                           w/2, h,
                           spl + 1,
                           spec,
                           sz/2,
                           res/2);

        unpackResultMatrix(rmat,
                           x + w/2, y,
                           w/2, h,
                           spl + sz/2,
                           spec + sz/2,
                           sz/2,
                           res/2);

    } else {
        cerr << "ERROR: *spl = " << *spl << endl;
    }
}

//spl[Y-1]
//Specs[R0][x0:x0+x-1][Y0:Y0+Y-1]
//Specs[R0+1][2x0:2x0+2x-1][Y0/2:Y0/2+Y/2-1]
//...
//Specs[R0+?][Nx0:Nx0+Nx-1][Y0/N:Y0/N+Y/N-1]
//N=WID/wid

/**
 * DoCutSpectrogramBlock2 finds the optimal "cutting" and returns it in spl.
 */
double
AdaptiveSpectrogram::DoCutSpectrogramBlock2(int* spl, double*** Specs, int Y, int R0,
                                      int x0, int Y0, int N, double& ene)
{
    double ent = 0; 

    if (Y > N) {

        spl[0] = 0;
        double ene1, ene2;

        ent += DoCutSpectrogramBlock2
            (&spl[1], Specs, Y/2, R0, x0, Y0, N, ene1);

        ent += DoCutSpectrogramBlock2
            (&spl[Y/2], Specs, Y/2, R0, x0, Y0+Y/2, N, ene2);

        ene = ene1+ene2;

    } else if (N == 1) {

        double tmp = Specs[R0][x0][Y0];
        ene = tmp;
        ent = xlogx(tmp);

    } else {
        // Y == N, the square case

        double enel, ener, enet, eneb, entl, entr, entt, entb;
        int* tmpspl = new int[Y];

        entl = DoCutSpectrogramBlock2
            (&spl[1], Specs, Y/2, R0+1, 2*x0, Y0/2, N/2, enel);

        entr = DoCutSpectrogramBlock2
            (&spl[Y/2], Specs, Y/2, R0+1, 2*x0+1, Y0/2, N/2, ener);

        entb = DoCutSpectrogramBlock2
            (&tmpspl[1], Specs, Y/2, R0, x0, Y0, N/2, eneb);

        entt = DoCutSpectrogramBlock2
            (&tmpspl[Y/2], Specs, Y/2, R0, x0, Y0+Y/2, N/2, enet);

        double
            ene0 = enet + eneb,
            ene1 = enel + ener,
            ent0 = entt + entb, 
            ent1 = entl + entr;

        // normalize

        double eneres = 1 - (ene0+ene1)/2, norment0, norment1;
        double a0 = 1 / (ene0+eneres), a1 = 1 / (ene1+eneres);

        // quasi-global normalization

        norment0 = (ent0 - ene0 * log(ene0+eneres)) / (ene0+eneres);
        norment1 = (ent1 - ene1 * log(ene1+eneres)) / (ene1+eneres);

        // local normalization

        if (norment1 < norment0) {
            spl[0] = 0;
            ent = ent0, ene = ene0;
            memcpy(&spl[1], &tmpspl[1], sizeof(int)*(Y-2));
        } else {
            spl[0] = 1;
            ent = ent1, ene = ene1;
        }
    }
    return ent;
}
 
/**
 * DoMixSpectrogramBlock2 collects values from the multiple
 * spectrograms Specs into a linear array Spec.
 */
double
AdaptiveSpectrogram::DoMixSpectrogramBlock2(int* spl, double* Spec, double*** Specs, int Y,
                                      int R0, int x0, int Y0, bool normmix, int res,
                                      double* e)
{
    if (Y == 1) {

        Spec[0] = Specs[R0][x0][Y0]*e[0];

    } else {

        double le[32];

        if (normmix && Y < (1<<res)) {

            for (int i = 0, j = 1, k = Y;
                 i < res;
                 i++, j *= 2, k /= 2) {

                double lle = 0;

                for (int fr = 0; fr < j; fr++) {
                    for (int n = 0; n < k; n++) {
                        lle +=
                            Specs[i+R0][x0+fr][Y0+n] *
                            Specs[i+R0][x0+fr][Y0+n];
                    }
                }

                lle = sqrt(lle)*e[i];

                if (i == 0) {
                    le[0] = lle;
                } else if (lle > le[0]*2) {
                    le[i] = e[i]*le[0]*2/lle;
                } else {
                    le[i] = e[i];
                }
            }
 
            le[0] = e[0];

        } else {

            memcpy(le, e, sizeof(double)*res);
        }
 
        if (spl[0] == 0) {

            int newres;
            if (Y >= (1<<res)) newres = res;
            else newres = res-1;

            DoMixSpectrogramBlock2
                (&spl[1], Spec, Specs, Y/2, R0, x0, Y0,
                 normmix, newres, le);

            DoMixSpectrogramBlock2
                (&spl[Y/2], &Spec[Y/2], Specs, Y/2, R0, x0, Y0+Y/2,
                 normmix, newres, le);

        } else {

            DoMixSpectrogramBlock2
                (&spl[1], Spec, Specs, Y/2, R0+1, x0*2, Y0/2,
                 normmix, res-1, &le[1]);

            DoMixSpectrogramBlock2
                (&spl[Y/2], &Spec[Y/2], Specs, Y/2, R0+1, x0*2+1, Y0/2,
                 normmix, res-1, &le[1]);
        }
    }

    return 0;
}
 
/**
 * MixSpectrogramBlock2 calls the two Do...() to do the real work.
 *
 * At this point:
 * spl is... what?  the returned "cutting", organised how?
 * Spec is... what?  the returned spectrogram, organised how?
 * Specs is an array of input spectrograms
 * WID is the maximum window size
 * wid is the minimum window size
 * normmix is... what?
 */
double
AdaptiveSpectrogram::MixSpectrogramBlock2(int* spl, double* Spec, double*** Specs, int
                                    WID, int wid, bool normmix)
{
    double ene[32];

    // find the total energy and normalize

    for (int i = 0, Fr = 1, Wid = WID; Wid >= wid; i++, Fr *= 2, Wid /= 2) {

        double lene = 0;

        for (int fr = 0; fr < Fr; fr++) {
            for (int k = 0; k < Wid; k++) {
                lene += Specs[i][fr][k]*Specs[i][fr][k];
            }
        }

        ene[i] = lene;

        if (lene != 0) {
            double ilene = 1.0/lene;
            for (int fr = 0; fr < Fr; fr++) {
                for (int k = 0; k < Wid; k++) {
                    Specs[i][fr][k] = Specs[i][fr][k]*Specs[i][fr][k]*ilene;
                }
            }
        }
    }
 

    double result = DoCutSpectrogramBlock2
        (spl, Specs, WID, 0, 0, 0, WID/wid, ene[31]);
 
    // de-normalize

    for (int i = 0, Fr = 1, Wid = WID; Wid >= wid; i++, Fr *= 2, Wid /= 2) {
        double lene = ene[i];
        if (lene != 0) {
            for (int fr = 0; fr < Fr; fr++) {
                for (int k = 0; k < Wid; k++) {
                    Specs[i][fr][k] = sqrt(Specs[i][fr][k]*lene);
                }
            }
        }
    }
    
    double e[32];
    for (int i = 0; i < 32; i++) e[i] = 1;

    DoMixSpectrogramBlock2
        (spl, Spec, Specs, WID, 0, 0, 0, normmix, log2(WID/wid)+1, e);

    return result;
}
 
/**
 * MixSpectrogram2 does the work for Fr frames (the largest frame),
 * which basically calls MixSpectrogramBlock2 Fr times.
 *
 * the 3-D array Specs is the multiple spectrograms calculated with
 * window sizes between wid and WID, Specs[0] is the 0th spectrogram,
 * etc.
 *
 * spl and Spec for all frames are returned by MixSpectrogram2, each
 * as a 2-D array.
 */
double
AdaptiveSpectrogram::MixSpectrogram2(int** spl, double** Spec, double*** Specs, int Fr,
				     int WID, int wid, bool norm, bool normmix)
{
    // totally Fr frames of WID samples
    // each frame is divided into wid VERTICAL parts

    int Res = log2(WID/wid)+1;
    double*** lSpecs = new double**[Res];

    for (int i = 0; i < Fr; i++) {

        for (int j = 0, nfr = 1; j < Res; j++, nfr *= 2) {
            lSpecs[j] = &Specs[j][i*nfr];
        }

        MixSpectrogramBlock2(spl[i], Spec[i], lSpecs, WID, wid, norm);
    }
 
    delete[] lSpecs;
    return 0;
}

