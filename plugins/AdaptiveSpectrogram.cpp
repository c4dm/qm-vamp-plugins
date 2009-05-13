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
    m_w(8),
    m_n(3),
    m_threaded(true),
    m_threadsInUse(false)
{
}

AdaptiveSpectrogram::~AdaptiveSpectrogram()
{
    for (int i = 0; i < m_cutThreads.size(); ++i) {
        delete m_cutThreads[i];
    }
    m_cutThreads.clear();

    for (FFTMap::iterator i = m_fftThreads.begin(); i != m_fftThreads.end(); ++i) {
        delete i->second;
    }
    m_fftThreads.clear();
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
    desc.defaultValue = 4;
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
    desc2.defaultValue = 9;
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

    ParameterDescriptor desc3;
    desc3.identifier = "threaded";
    desc3.name = "Multi-threaded processing";
    desc3.description = "Perform calculations in several threads in parallel";
    desc3.unit = "";
    desc3.minValue = 0;
    desc3.maxValue = 1;
    desc3.defaultValue = 1;
    desc3.isQuantized = true;
    desc3.quantizeStep = 1;
    list.push_back(desc3);

    return list;
}

float
AdaptiveSpectrogram::getParameter(std::string id) const
{
    if (id == "n") return m_n+1;
    else if (id == "w") return m_w+1;
    else if (id == "threaded") return (m_threaded ? 1 : 0);
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
    } else if (id == "threaded") {
        m_threaded = (value > 0.5);
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

    int minwid = (2 << m_w), maxwid = ((2 << m_w) << m_n);

#ifdef DEBUG_VERBOSE
    cerr << "widths from " << minwid << " to " << maxwid << " ("
         << minwid/2 << " to " << maxwid/2 << " in real parts)" << endl;
#endif

    Spectrograms s(minwid/2, maxwid/2, 1);

    int w = minwid;
    int index = 0;

    while (w <= maxwid) {
        if (m_fftThreads.find(w) == m_fftThreads.end()) {
            m_fftThreads[w] = new FFTThread(w);
        }
        if (m_threaded) {
            m_fftThreads[w]->startCalculation(inputBuffers[0], s, index, maxwid);
        } else {
            m_fftThreads[w]->setParameters(inputBuffers[0], s, index, maxwid);
            m_fftThreads[w]->performTask();
        }
        w *= 2;
        ++index;
    }

    if (m_threaded) {
        w = minwid;
        while (w <= maxwid) {
            m_fftThreads[w]->await();
            w *= 2;
        }
    }

    m_threadsInUse = false;

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

//    std::cerr << "process returning!\n" << std::endl;

    return fs;
}

void
AdaptiveSpectrogram::printCutting(Cutting *c, string pfx) const
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

void
AdaptiveSpectrogram::getSubCuts(const Spectrograms &s,
                                int res,
                                int x, int y, int h,
                                Cutting *&top, Cutting *&bottom,
                                Cutting *&left, Cutting *&right) const
{
    if (m_threaded && !m_threadsInUse) {

        m_threadsInUse = true;

        if (m_cutThreads.empty()) {
            for (int i = 0; i < 4; ++i) {
                CutThread *t = new CutThread(this);
                m_cutThreads.push_back(t);
            }
        }

        // Cut threads 0 and 1 calculate the top and bottom halves;
        // threads 2 and 3 calculate left and right.

        // The "vertical" division is a top/bottom split.
        // Splitting this way keeps us in the same resolution,
        // but with two vertical subregions of height h/2.
        m_cutThreads[0]->cut(s, res, x, y + h/2, h/2); // top
        m_cutThreads[1]->cut(s, res, x, y, h/2); // bottom
        m_cutThreads[2]->cut(s, res/2, 2 * x, y/2, h/2); // left
        m_cutThreads[3]->cut(s, res/2, 2 * x + 1, y/2, h/2); // right

        top    = m_cutThreads[0]->get();
        bottom = m_cutThreads[1]->get();
        left   = m_cutThreads[2]->get();
        right  = m_cutThreads[3]->get();

    } else {

        // unthreaded

        // The "vertical" division is a top/bottom split.
        // Splitting this way keeps us in the same resolution,
        // but with two vertical subregions of height h/2.

        top    = cut(s, res, x, y + h/2, h/2);
        bottom = cut(s, res, x, y, h/2);

        // The "horizontal" division is a left/right split.  Splitting
        // this way places us in resolution res/2, which has lower
        // vertical resolution but higher horizontal resolution.  We
        // need to double x accordingly.
        
        left   = cut(s, res/2, 2 * x, y/2, h/2);
        right  = cut(s, res/2, 2 * x + 1, y/2, h/2);
    }
}

AdaptiveSpectrogram::Cutting *
AdaptiveSpectrogram::cut(const Spectrograms &s,
                         int res,
                         int x, int y, int h) const
{
//    cerr << "res = " << res << ", x = " << x << ", y = " << y << ", h = " << h << endl;

    if (h > 1 && res > s.minres) {

        Cutting *top = 0, *bottom = 0, *left = 0, *right = 0;
        getSubCuts(s, res, x, y, h, top, bottom, left, right);

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
                              int x, int y, int w, int h) const
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
            
