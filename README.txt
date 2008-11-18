
QM Vamp Plugins
===============

Vamp audio feature extraction plugins from the Centre for Digital
Music at Queen Mary, University of London.

http://www.elec.qmul.ac.uk/digitalmusic/

Version 1.5.

For more information about Vamp plugins, see http://www.vamp-plugins.org/ .


About This Release
==================

This is a bugfix release only.  The plugins provided are unchanged
from 1.4.


Plugins Included
================

This plugin set includes the following plugins:

   * Note onset detector

   * Beat tracker and tempo estimator

   * Key estimator and tonal change detector

   * Segmenter, to divide a track into a consistent sequence of segments

   * Timbral and rhythmic similarity between audio tracks

   * Chromagram, constant-Q spectrogram, and MFCC calculation plugins

More details about the plugins follow.


License
=======

These plugins are provided in binary form only.  You may install and
use the plugin binaries without fee for any purpose commercial or
non-commercial.  You may redistribute the plugin binaries provided you
do so without fee and you retain this README file with your
distribution.  You may not bundle these plugins with a commercial
product or distribute them on commercial terms.  If you wish to
arrange commercial licensing terms, please contact the Centre for
Digital Music at Queen Mary, University of London.

Copyright (c) 2006-2008 Queen Mary, University of London.  All rights
reserved except as described above.


To Install
==========

Installation depends on your operating system.

    Windows -> Copy qm-vamp-plugins.dll and qm-vamp-plugins.cat to
               C:\Program Files\Vamp Plugins\

    OS/X    -> Copy qm-vamp-plugins.dylib and qm-vamp-plugins.cat to
               $HOME/Library/Audio/Plug-Ins/Vamp/
               or
               /Library/Audio/Plug-Ins/Vamp/

    Linux   -> Copy qm-vamp-plugins.so and qm-vamp-plugins.cat to
               $HOME/vamp/
               or
               /usr/local/lib/vamp/
               or 
               /usr/lib/vamp/


The Plugins
===========

Note Onset Detector
-------------------

 Identifier:    qm-onsetdetector
 Authors:       Chris Duxbury, Juan Pablo Bello and Christian Landone
 Category:      Time > Onsets

 References:    C. Duxbury, J. P. Bello, M. Davies and M. Sandler.
                Complex domain Onset Detection for Musical Signals.
                In Proceedings of the 6th Conference on Digital Audio
                Effects (DAFx-03). London, UK. September 2003.

                D. Stowell and M. D. Plumbley.
                Adaptive whitening for improved real-time audio onset
                detection.
                In Proceedings of the International Computer Music
                Conference (ICMC'07), August 2007.

                D. Barry, D. Fitzgerald, E. Coyle and B. Lawlor.
                Drum Source Separation using Percussive Feature
                Detection and Spectral Modulation.
                ISSC 2005

The Note Onset Detector plugin analyses a single channel of audio and
estimates the locations of note onsets within the music.

It calculates an onset likelihood function for each spectral frame,
and picks peaks in a smoothed version of this function.  The plugin is
non-causal, returning all results at the end of processing.

It has three outputs: the note onset positions, the onset detection
function used in estimating onset positions, and a smoothed version of
the detection function that is used in the peak-picking phase.


Tempo and Beat Tracker
----------------------

 Identifier:    qm-tempotracker
 Authors:       Matthew Davies and Christian Landone
 Category:      Time > Tempo

 References:    M. E. P. Davies and M. D. Plumbley.
                Context-dependent beat tracking of musical audio.
                In IEEE Transactions on Audio, Speech and Language
                Processing. Vol. 15, No. 3, pp1009-1020, 2007.

                M. E. P. Davies and M. D. Plumbley.
                Beat Tracking With A Two State Model.
                In Proceedings of the IEEE International Conference 
                on Acoustics, Speech and Signal Processing (ICASSP 2005),
                Vol. 3, pp241-244 Philadelphia, USA, March 19-23, 2005.

The Tempo and Beat Tracker plugin analyses a single channel of audio
and estimates the locations of metrical beats and the resulting tempo.

It has three outputs: the beat positions, an ongoing estimate of tempo
where available, and the onset detection function used in estimating
beat positions.


Key Detector
------------

 Identifier:    qm-keydetector
 Authors:       Katy Noland and Christian Landone
 Category:      Key and Tonality

 References:    K. Noland and M. Sandler.
                Signal Processing Parameters for Tonality Estimation.
                In Proceedings of Audio Engineering Society 122nd
                Convention, Vienna, 2007.

The Key Detector plugin analyses a single channel of audio and
continuously estimates the key of the music.

It has four outputs: the tonic pitch of the key; a major or minor mode
flag; the key (combining the tonic and major/minor into a single
value); and a key strength plot which reports the degree to which the
chroma vector extracted from each input block correlates to the stored
key profiles for each major and minor key.  The key profiles are drawn
from analysis of Book I of the Well Tempered Klavier by J S Bach,
recorded at A=440 equal temperament.

The outputs have the values:

  Tonic pitch: C = 1, C#/Db = 2, ..., B = 12

  Major/minor mode: major = 0, minor = 1

  Key: C major = 1, C#/Db major = 2, ..., B major = 12
       C minor = 13, C#/Db minor = 14, ..., B minor = 24

  Key Strength Plot: 25 separate bins per feature, separated into 1-12
       (major from C) and 14-25 (minor from C).  Bin 13 is unused, not
       for superstitious reasons but simply so as to delimit the major
       and minor areas if they are displayed on a single plot by the
       plugin host.  Higher bin values show increased correlation with
       the key profile for that key.

The outputs are also labelled with pitch or key as text.


Tonal Change
------------

 Identifier:    qm-tonalchange
 Authors:       Chris Harte and Martin Gasser
 Category:      Key and Tonality

 References:    C. A. Harte, M. Gasser, and M. Sandler.
                Detecting harmonic change in musical audio.
                In Proceedings of the 1st ACM workshop on Audio and Music
                Computing Multimedia, Santa Barbara, 2006.

                C. A. Harte and M. Sandler.
                Automatic chord identification using a quantised chromagram.
                In Proceedings of the 118th Convention of the Audio
                Engineering Society, Barcelona, Spain, May 28-31 2005.

The Tonal Change plugin analyses a single channel of audio, detecting
harmonic changes such as chord boundaries.

It has three outputs: a representation of the musical content in a
six-dimensional tonal space onto which the algorithm maps 12-bin
chroma vectors extracted from the audio; a function representing the
estimated likelihood of a tonal change occurring in each spectral
frame; and the resulting estimated positions of tonal changes.


Segmenter
---------

 Identifier:    qm-segmenter
 Authors:       Mark Levy
 Category:      Classification

 References:    M. Levy and M. Sandler.
                Structural segmentation of musical audio by constrained
                clustering.
                IEEE Transactions on Audio, Speech, and Language Processing,
                February 2008.

The Segmenter plugin divides a single channel of music up into
structurally consistent segments.  Its single output contains a
numeric value (the segment type) for each moment at which a new
segment starts.

For music with clearly tonally distinguishable sections such as verse,
chorus, etc., the segments with the same type may be expected to be
similar to one another in some structural sense (e.g. repetitions of
the chorus).

The type of feature used in segmentation can be selected using the
Feature Type parameter.  The default Hybrid (Constant-Q) is generally
effective for modern studio recordings, while the Chromatic option may
be preferable for live, acoustic, or older recordings, in which
repeated sections may be less consistent in sound.  Also available is
a timbral (MFCC) feature, which is more likely to result in
classification by instrumentation rather than musical content.

Note that this plugin does a substantial amount of processing after
receiving all of the input audio data, before it produces any results.


Similarity
----------

 Identifier:    qm-similarity
 Authors:       Mark Levy, Kurt Jacobson and Chris Cannam
 Category:      Classification

 References:    M. Levy and M. Sandler.
                Lightweight measures for timbral similarity of musical audio.
                In Proceedings of the 1st ACM workshop on Audio and Music
                Computing Multimedia, Santa Barbara, 2006.

                K. Jacobson.
                A Multifaceted Approach to Music Similarity.
                In Proceedings of the Seventh International Conference on
                Music Information Retrieval (ISMIR), 2006.

The Similarity plugin treats each channel of its audio input as a
separate "track", and estimates how similar the tracks are to one
another using a selectable similarity measure.

The plugin also returns the intermediate data used as a basis of the
similarity measure; it can therefore be used on a single channel of
input (with the resulting intermediate data then being applied in some
other similarity or clustering algorithm, for example) if desired, as
well as with multiple inputs.

The underlying audio features used for the similarity measure can be
selected using the Feature Type parameter.  The available features are
Timbre (in which the distance between tracks is a symmetrised
Kullback-Leibler divergence between Gaussian-modelled MFCC means and
variances across each track); Chroma (KL divergence of mean chroma
histogram); Rhythm (cosine distance between "beat spectrum" measures
derived from a short sampled section of the track); and combined
"Timbre and Rhythm" and "Chroma and Rhythm".

The plugin has six outputs: a matrix of the distances between input
channels; a vector containing the distances between the first input
channel and each of the input channels; a pair of vectors containing
the indices of the input channels in the order of their similarity to
the first input channel, and the distances between the first input
channel and each of those channels; the means of the underlying
feature bins (MFCCs or chroma); the variances of the underlying
feature bins; and the beat spectra used for the rhythmic feature.

Because Vamp does not have the capability to return features in matrix
form explicitly, the matrix output is returned as a series of vector
features timestamped at one-second intervals.  Likewise, the
underlying feature outputs contain one vector feature per input
channel, timestamped at one-second intervals (so the feature for the
first channel is at time 0, and so on).  Examining the features that
the plugin actually returns, when run on some test data, may make this
arrangement more clear.

Note that the underlying feature values are only returned if the
relevant feature type is selected.  That is, the means and variances
outputs are valid provided the pure rhythm feature is not selected;
the beat spectra output is valid provided rhythm is included in the
selected feature type.


Constant-Q Spectrogram
----------------------

 Identifier:    qm-constantq
 Authors:       Christian Landone
 Category:      Visualisation

 References:    J. Brown.
                Calculation of a constant Q spectral transform.
                Journal of the Acoustical Society of America, 89(1):
                425-434, 1991.

The Constant-Q Spectrogram plugin calculates a spectrogram based on a
short-time windowed constant Q spectral transform.  This is a
spectrogram in which the ratio of centre frequency to resolution is
constant for each frequency bin.  The frequency bins correspond to the
frequencies of "musical notes" rather than being linearly spaced in
frequency as they are for the conventional DFT spectrogram.

The pitch range and the number of frequency bins per octave may be
adjusted using the plugin's parameters.  Note that the plugin's
preferred step and block sizes depend on these parameters, and the
plugin will not accept any other block size.


Chromagram
----------

 Identifier:    qm-chromagram
 Authors:       Christian Landone
 Category:      Visualisation

The Chromagram plugin calculates a constant Q spectral transform (as
above) and then wraps the frequency bin values into a single octave,
with each bin containing the sum of the magnitudes from the
corresponding bin in all octaves.  The number of values in each
feature vector returned by the plugin is therefore the same as the
number of bins per octave configured for the underlying constant Q
transform.

The pitch range and the number of frequency bins per octave for the
transform may be adjusted using the plugin's parameters.  Note that
the plugin's preferred step and block sizes depend on these
parameters, and the plugin will not accept any other block size.


Mel-Frequency Cepstral Coefficients
-----------------------------------

 Identifier:    qm-mfcc
 Authors:       Nicolas Chetry and Chris Cannam
 Category:      Low Level Features

 References:    B. Logan.
                Mel-Frequency Cepstral Coefficients for Music Modeling.
                In Proceedings of the First International Symposium on Music
                Information Retrieval (ISMIR), 2000.

The Mel-Frequency Cepstral Coefficients plugin calculates MFCCs from a
single channel of audio, returning one MFCC vector from each process
call.  It also returns the overall means of the coefficient values
across the length of the audio input, as a separate output at the end
of processing.

