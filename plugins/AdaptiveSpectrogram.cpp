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

//#define DEBUG_VERBOSE 1

AdaptiveSpectrogram::AdaptiveSpectrogram(float inputSampleRate) :
    Plugin(inputSampleRate),
    m_w(9),
    m_n(2)
//    m_w(0),
//    m_n(2)
{
}

AdaptiveSpectrogram::~AdaptiveSpectrogram()
{
}

string
AdaptiveSpectrogram::getIdentifier() const
{
    return "qm-adaptivespectrogram";
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

#ifdef NOT_DEFINED
AdaptiveSpectrogram::FeatureSet
AdaptiveSpectrogram::process(const float *const *inputBuffers, RealTime ts)
{
    FeatureSet fs;

    int wid = (2 << m_w), WID = ((2 << m_w) << m_n);
    int Res = log2(WID/wid)+1;
    double ***specs = new double **[Res];
    int Wid = WID;
    int wi = 0;

#ifdef DEBUG_VERBOSE
    cerr << "wid = " << wid << ", WID = " << WID << endl;
#endif

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
                int k = j+1; // include Nyquist but not DC
                double mag = sqrt(tmprout[k] * tmprout[k] +
                                  tmpiout[k] * tmpiout[k]);
                double scaled = mag / (Wid/2);
//                double power = scaled*scaled;
//                if (k < Wid/2) power = power*2;
                specs[wi][i][j] = scaled;
            }
        }
        Wid /= 2;
        ++wi;
    }

/*    
    while (Wid >= wid) {
        specs[wi] = new double *[WID/Wid];
        cerr << "filling width " << Wid << endl;
        for (int i = 0; i < WID/Wid; ++i) {
            specs[wi][i] = new double[Wid/2];
            for (int j = 0; j < Wid/2; ++j) {

                specs[wi][i][j] = 0;
                int x0 = i * Wid/2;
                int x1 = (i+1) * Wid/2 - 1;
                int y0 = j * (WID/Wid);
                int y1 = (j+1) * (WID/Wid) - 1;

                cerr << "box at " << i  << "," << j << " covers [" << x0 << "," << y0 << "] to [" << x1 << "," << y1 << "]" << endl;

                for (int y = WID/4; y < WID/2; ++y) {
                    for (int x = WID/4-2; x < WID/4; ++x) {
                        if (x >= x0 && x <= x1 && y >= y0 && y <= y1) {
                            ++specs[wi][i][j];
                        }
                    }
                }

                for (int x = 0; x < WID/2; ++x) {
                    int y = 0;
                    if (x >= x0 && x <= x1 && y >= y0 && y <= y1) {
                        ++specs[wi][i][j];
                    }
                }
            }
        }
        cerr << "this spectrogram:" << endl;
        for (int j = Wid/2-1; j >= 0; --j) {
            for (int i = 0; i < WID/Wid; ++i) {
                cerr << specs[wi][i][j] << " ";
            }
            cerr << endl;
        }
        Wid /= 2;
        ++wi;
    }
*/

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

#ifdef DEBUG_VERBOSE
    std::cerr << "Results at " << ts << ":" << std::endl;
    for (int i = 0; i < WID/2; ++i) {
//        if (spl[i] == i || spec[i] == i) {
//            std::cerr << "\n***\n";
//        }
        std::cerr << "[" << i << "] " << spl[i] << "," << spec[i] << " ";
    }
    std::cerr << std::endl;
#endif

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
#endif

AdaptiveSpectrogram::FeatureSet
AdaptiveSpectrogram::process(const float *const *inputBuffers, RealTime ts)
{
    FeatureSet fs;

    int minwid = (2 << m_w), maxwid = ((2 << m_w) << m_n);

#ifdef DEBUG_VERBOSE
    cerr << "widths from " << minwid << " to " << maxwid << " ("
         << minwid/2 << " to " << maxwid/2 << " in real parts)" << endl;
#endif

    Spectrograms s(minwid/2, maxwid/2, 1);
    
    double *tmpin  = new double[maxwid];
    double *tmprout = new double[maxwid];
    double *tmpiout = new double[maxwid];

    int w = minwid;
    int index = 0;

    while (w <= maxwid) {
        for (int i = 0; i < maxwid / w; ++i) {
            int origin = maxwid/4 - w/4; // for 50% overlap
            for (int j = 0; j < w; ++j) {
                double mul = 0.50 - 0.50 * cos((2 * M_PI * j) / w);
                tmpin[j] = inputBuffers[0][origin + i * w/2 + j] * mul;
            }
            FFT::process(w, false, tmpin, 0, tmprout, tmpiout);
            for (int j = 0; j < w/2; ++j) {
                int k = j+1; // include Nyquist but not DC
                double mag = sqrt(tmprout[k] * tmprout[k] +
                                  tmpiout[k] * tmpiout[k]);
                double scaled = mag / (w/2);
                s.spectrograms[index]->data[i][j] = scaled;
            }
        }
        w *= 2;
        ++index;
    }

    delete[] tmpin;
    delete[] tmprout;
    delete[] tmpiout;

    Cutting *cutting = cut(s, maxwid/2, 0, 0, maxwid/2);

#ifdef DEBUG_VERBOSE
    printCutting(cutting, "  ");
#endif

    vector<vector<float> > rmat(maxwid/minwid);
    for (int i = 0; i < maxwid/minwid; ++i) {
        rmat[i] = vector<float>(maxwid/2);
    }
    
    assemble(s, cutting, rmat, 0, 0, maxwid/minwid, maxwid/2);

    delete cutting;

    for (int i = 0; i < rmat.size(); ++i) {
        Feature f;
        f.hasTimestamp = false;
        f.values = rmat[i];
        fs[0].push_back(f);
    }

    return fs;
}

void
AdaptiveSpectrogram::printCutting(Cutting *c, string pfx)
{
    if (c->first) {
        if (c->cut == Cutting::Horizontal) {
            cerr << pfx << "H" << endl;
        } else if (c->cut == Cutting::Vertical) {
            cerr << pfx << "V" << endl;
        }
        printCutting(c->first, pfx + "  ");
        printCutting(c->second, pfx + "  ");
    } else {
        cerr << pfx << "* " << c->value << endl;
    }
}

AdaptiveSpectrogram::Cutting *
AdaptiveSpectrogram::cut(const Spectrograms &s,
                         int res,
                         int x, int y, int h)
{
//    cerr << "res = " << res << ", x = " << x << ", y = " << y << ", h = " << h << endl;

    if (h > 1 && res > s.minres) {

        // The "vertical" division is a top/bottom split.
        // Splitting this way keeps us in the same resolution,
        // but with two vertical subregions of height h/2.

        Cutting *top    = cut(s, res, x, y + h/2, h/2);
        Cutting *bottom = cut(s, res, x, y, h/2);

        // The "horizontal" division is a left/right split.  Splitting
        // this way places us in resolution res/2, which has lower
        // vertical resolution but higher horizontal resolution.  We
        // need to double x accordingly.

        Cutting *left   = cut(s, res/2, 2 * x, y/2, h/2);
        Cutting *right  = cut(s, res/2, 2 * x + 1, y/2, h/2);

        double vcost = top->cost + bottom->cost;
        double hcost = left->cost + right->cost;

        bool normalize = true;

        if (normalize) {

            double venergy = top->value + bottom->value;
            vcost = (vcost + (venergy * log(venergy))) / venergy;

            double henergy = left->value + right->value;
            hcost = (hcost + (henergy * log(henergy))) / henergy;
        }            

        if (vcost > hcost) {

            // cut horizontally (left/right)

            Cutting *cutting = new Cutting;
            cutting->cut = Cutting::Horizontal;
            cutting->first = left;
            cutting->second = right;
            cutting->cost = hcost;
            cutting->value = left->value + right->value;
            delete top;
            delete bottom;
            return cutting;

        } else {

            Cutting *cutting = new Cutting;
            cutting->cut = Cutting::Vertical;
            cutting->first = top;
            cutting->second = bottom;
            cutting->cost = vcost;
            cutting->value = top->value + bottom->value;
            delete left;
            delete right;
            return cutting;
        }

    } else {

        // no cuts possible from this level

        Cutting *cutting = new Cutting;
        cutting->cut = Cutting::Finished;
        cutting->first = 0;
        cutting->second = 0;

        int n = 0;
        for (int r = res; r > s.minres; r /= 2) ++n;
        const Spectrogram *spectrogram = s.spectrograms[n];

        cutting->cost = cost(*spectrogram, x, y);
        cutting->value = value(*spectrogram, x, y);

//        cerr << "cost for this cell: " << cutting->cost << endl;

        return cutting;
    }
}

void
AdaptiveSpectrogram::assemble(const Spectrograms &s,
                              const Cutting *cutting,
                              vector<vector<float> > &rmat,
                              int x, int y, int w, int h)
{
    switch (cutting->cut) {

    case Cutting::Finished:
        for (int i = 0; i < w; ++i) {
            for (int j = 0; j < h; ++j) {
                rmat[x+i][y+j] = cutting->value;
            }
        }
        return;

    case Cutting::Horizontal:
        assemble(s, cutting->first, rmat, x, y, w/2, h);
        assemble(s, cutting->second, rmat, x+w/2, y, w/2, h);
        break;
        
    case Cutting::Vertical:
        assemble(s, cutting->first, rmat, x, y+h/2, w, h/2);
        assemble(s, cutting->second, rmat, x, y, w, h/2);
        break;
    }        
}
            
void
AdaptiveSpectrogram::unpackResultMatrix(vector<vector<float> > &rmat,
                                  int x, int y, int w, int h,
                                  int *spl,
                                  double *spec, int sz,
                                  int res
    )
{

#ifdef DEBUG_VERBOSE
    cerr << "x = " << x << ", y = " << y << ", w = " << w << ", h = " << h 
         << ", sz = " << sz << ", *spl = " << *spl << ", *spec = " << *spec << ", res = " << res <<  endl;
#endif

    if (sz <= 1) {

        for (int i = 0; i < w; ++i) {
            for (int j = 0; j < h; ++j) {
                if (rmat[x+i][y+j] != 0) {
                    cerr << "WARNING: Overwriting value " << rmat[x+i][y+j] 
                         << " with " << res + i + j << " at " << x+i << "," << y+j << endl;
                }
                rmat[x+i][y+j] = *spec;
            }
        }

        return;
    }

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
                                            int x0, int Y0, int N, double& ene, string pfx)
{
    double ent = 0; 

#ifdef DEBUG_VERBOSE
    cerr << pfx << "cutting with Y = " << Y << ", R0 = " << R0 << ", x0 = " << x0 << ", Y0 = " << Y0 << ", N = " << N << endl;
#endif

    if (Y > N) {

#ifdef DEBUG_VERBOSE
        cerr << pfx << "Y > N case, making top/bottom cut" << endl;
#endif

        spl[0] = 0;
        double ene1, ene2;

        ent += DoCutSpectrogramBlock2
            (&spl[1], Specs, Y/2, R0, x0, Y0, N, ene1, pfx + "  ");

        ent += DoCutSpectrogramBlock2
            (&spl[Y/2], Specs, Y/2, R0, x0, Y0+Y/2, N, ene2, pfx + "  ");

        ene = ene1+ene2;

    } else if (N == 1) {

        double tmp = Specs[R0][x0][Y0];

#ifdef DEBUG_VERBOSE
        cerr << pfx << "N == 1 case (value here = " << tmp << ")" << endl;
#endif

        ene = tmp;
        ent = xlogx(tmp);

    } else {
        // Y == N, the square case

#ifdef DEBUG_VERBOSE
        cerr << pfx << "Y == N case, testing left/right cut" << endl;
#endif

        double enel, ener, enet, eneb, entl, entr, entt, entb;
        int* tmpspl = new int[Y];

        entl = DoCutSpectrogramBlock2
            (&spl[1], Specs, Y/2, R0+1, 2*x0, Y0/2, N/2, enel, pfx + "  ");

        entr = DoCutSpectrogramBlock2
            (&spl[Y/2], Specs, Y/2, R0+1, 2*x0+1, Y0/2, N/2, ener, pfx + "  ");

#ifdef DEBUG_VERBOSE
        cerr << pfx << "Y == N case, testing top/bottom cut" << endl;
#endif

        entb = DoCutSpectrogramBlock2
            (&tmpspl[1], Specs, Y/2, R0, x0, Y0, N/2, eneb, pfx + "  ");

        entt = DoCutSpectrogramBlock2
            (&tmpspl[Y/2], Specs, Y/2, R0, x0, Y0+Y/2, N/2, enet, pfx + "  ");

        double
            ene0 = enet + eneb,
            ene1 = enel + ener,
            ent0 = entt + entb, 
            ent1 = entl + entr;

        // normalize

        double eneres = 1 - (ene0+ene1)/2, norment0, norment1;
        double a0 = 1 / (ene0+eneres), a1 = 1 / (ene1+eneres);

        // quasi-global normalization

//        norment0 = (ent0 - ene0 * log(ene0+eneres)) / (ene0+eneres);
//        norment1 = (ent1 - ene1 * log(ene1+eneres)) / (ene1+eneres);
        norment0 = ene0;
        norment1 = ene1;

        // local normalization

        if (norment1 < norment0) {
#ifdef DEBUG_VERBOSE
            cerr << pfx << "top/bottom cut wins (" << norment0 << " > " << norment1 << "), returning sum ent " << ent0 << " and ene " << ene0 << endl;
#endif
            spl[0] = 0;
            ent = ent0, ene = ene0;
            memcpy(&spl[1], &tmpspl[1], sizeof(int)*(Y-2));
        } else {
#ifdef DEBUG_VERBOSE
            cerr << pfx << "left/right cut wins (" << norment1 << " >= " << norment0 << "), returning sum ent " << ent1 << " and ene " << ene1 << endl;
#endif
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

