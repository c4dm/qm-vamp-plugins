/* -*- c-basic-offset: 4 indent-tabs-mode: nil -*-  vi:set ts=8 sts=4 sw=4: */

/*
    Vamp

    An API for audio analysis and feature extraction plugins.

    Centre for Digital Music, Queen Mary, University of London.
    Copyright 2006 Chris Cannam.
  
    Permission is hereby granted, free of charge, to any person
    obtaining a copy of this software and associated documentation
    files (the "Software"), to deal in the Software without
    restriction, including without limitation the rights to use, copy,
    modify, merge, publish, distribute, sublicense, and/or sell copies
    of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be
    included in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
    NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR
    ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
    CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
    WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

    Except as contained in this notice, the names of the Centre for
    Digital Music; Queen Mary, University of London; and Chris Cannam
    shall not be used in advertising or otherwise to promote the sale,
    use or other dealings in this Software without prior written
    authorization.
*/

#include "GetModePlugin.h"

using std::string;
using std::vector;
//using std::cerr;
using std::endl;

#include <cmath>


GetModePlugin::GetModePlugin(float inputSampleRate) :
    Plugin(inputSampleRate),
    m_stepSize(0),
    m_blockSize(32768),
    m_GetMode(0),
    m_InputFrame(0),
    m_BlockandHopSize(0)
{
    m_BlockandHopSize= 32768;
}

GetModePlugin::~GetModePlugin()
{
	if( m_GetMode )
	{
		delete m_GetMode;
		m_GetMode = 0;
	}

	if( m_InputFrame )
	{
		delete [] m_InputFrame;
		m_InputFrame = 0;
	}
}

string
GetModePlugin::getName() const
{
    return "qm-keymode";
}

string
GetModePlugin::getDescription() const
{
    return "Key Mode";
}

string
GetModePlugin::getMaker() const
{
    return "Katy Noland and Christian Landone";
}

int
GetModePlugin::getPluginVersion() const
{
    return 2;
}

string
GetModePlugin::getCopyright() const
{
    return "Centre for Digital Music QMUL";
}

bool
GetModePlugin::initialise(size_t channels, size_t stepSize, size_t blockSize)
{
    if (channels < getMinChannelCount() ||
	channels > getMaxChannelCount()) return false;

    m_stepSize = stepSize;
    m_blockSize = blockSize;

    if( stepSize != m_BlockandHopSize || blockSize != m_BlockandHopSize )
        return false;

    m_GetMode = new GetKeyMode( 10, 10 );
    
    m_InputFrame = new double[m_BlockandHopSize];
	
    return true;
}

void
GetModePlugin::reset()
{
    for( unsigned int i = 0; i < m_BlockandHopSize; i++ )
    {
        m_InputFrame[ i ] = 0.0;
    }
}


GetModePlugin::OutputList
GetModePlugin::getOutputDescriptors() const
{
    OutputList list;

    OutputDescriptor d;
    d.name = "mode";
    d.unit = "";
    d.description = "Key Mode";
    d.hasFixedBinCount = true;
    d.binCount = 1;
    d.hasKnownExtents = true;
    d.isQuantized = true;
    d.minValue = 0;
    d.maxValue = 1;
    d.quantizeStep = 1;
    d.binNames.push_back("Major = 0, Minor = 1");
    d.sampleType = OutputDescriptor::OneSamplePerStep;
    list.push_back(d);

    return list;
}

GetModePlugin::FeatureSet
GetModePlugin::process(const float *const *inputBuffers,
                       Vamp::RealTime)
{
    if (m_stepSize == 0) {
	return FeatureSet();
    }

    FeatureSet returnFeatures;

	for( unsigned int i = 0 ; i < m_BlockandHopSize; i++ )
	{
		m_InputFrame[i] = (double)inputBuffers[0][i];
	}


	int minor = m_GetMode->isModeMinor(m_GetMode->process( m_InputFrame ));

	Feature feature;
	feature.hasTimestamp = false;

	feature.values.push_back((float)minor);
	returnFeatures[0].push_back(feature);
        feature.values.clear();

    return returnFeatures;
}

GetModePlugin::FeatureSet
GetModePlugin::getRemainingFeatures()
{
    return FeatureSet();
}


size_t
GetModePlugin::getPreferredStepSize() const
{
    return 0;
}

size_t
GetModePlugin::getPreferredBlockSize() const
{
    return 32768;
}

