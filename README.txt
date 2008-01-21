
QM Vamp Plugins
===============

Vamp audio feature extraction plugins from Queen Mary, University of London.
Version 1.4.

For more information about Vamp plugins, see http://www.vamp-plugins.org/
and http://www.sonicvisualiser.org/.


New In This Release
===================

This release contains a new plugin to estimate timbral and rhythmic
similarity between multiple audio tracks, a plugin for structural
segmentation of music audio, and a Mel-frequency cepstral coefficients
calculation plugin.

This release also includes fixes to the existing key detector and
chromagram plugins.


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


Note Onset Detector
-------------------

Identifier:	qm-onsetdetector
Authors:	Chris Duxbury, Juan Pablo Bello and Christian Landone
Category:	Time > Onsets

References:	C. Duxbury, J. P. Bello, M. Davies and M. Sandler.
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

This plugin analyses a single channel of audio and estimates the
locations of note onsets within the music.

It has three outputs: the note onset positions, the onset detection
function used in estimating onset positions, and a smoothed version of
the detection function that is used in the peak-picking phase.


Tempo and Beat Tracker
----------------------

Identifier:	qm-tempotracker
Authors:	Matthew Davies and Christian Landone
Category:       Time > Tempo

References:	M. E. P. Davies and M. D. Plumbley.
		Context-dependent beat tracking of musical audio.
		Technical Report C4DM-TR-06-02. 5 April 2006.

		M. E. P. Davies and M. D. Plumbley.
		Beat Tracking With A Two State Model.
		In Proceedings of the IEEE International Conference 
		on Acoustics, Speech and Signal Processing (ICASSP 2005),
		Vol. 3, pp241-244 Philadelphia, USA, March 19-23, 2005.

This plugin analyses a single channel of audio and estimates the
locations of metrical beats and the resulting tempo of the music.

It has three outputs: the beat positions, an ongoing estimate of tempo
where available, and the onset detection function used in estimating
beat positions.



Key Detector
------------

Identifier:	qm-keydetector
Authors:	Katy Noland and Christian Landone
Category:	Key and Tonality

References:	K. Noland and M. Sandler.
		Signal Processing Parameters for Tonality Estimation.
		In Proceedings of Audio Engineering Society 122nd Convention,
		Vienna, 2007.

This plugin analyses a single channel of audio and continuously
estimates the key of the music.

It has three outputs: the tonic pitch of the key; a major or minor
mode flag; and key (combining the tonic and major/minor into a single
value).  These outputs have the values:

  Tonic pitch: C = 1, C#/Db = 2, ..., B = 12
  Major/minor mode: major = 0, minor = 1
  Key: C major = 1, C#/Db major = 2, ..., B major = 12
       C minor = 13, C#/Db minor = 14, ..., B minor = 24

The outputs are also labelled with pitch or key as text.


Tonal Change
------------

Identifier:	qm-tonalchange
Authors:	Chris Harte and Martin Gasser
Category:	Key and Tonality

References:	C. A. Harte and M. Sandler.
		Automatic chord identification using a quantised chromagram.
		In Proceedings of the 118th Convention of the Audio
		Engineering Society, Barcelona, Spain, May 28-31 2005.


Segmenter
---------

Identifier:	qm-segmenter
Authors:	Mark Levy
Category:	Classification

References:	M. Levy and M. Sandler.
		Structural segmentation of musical audio by constrained
		clustering.
		IEEE Transactions on Audio, Speech, and Language Processing,
		February 2008.



Similarity
----------

Identifier:	qm-similarity
Authors:	Mark Levy, Kurt Jacobson and Chris Cannam
Category:	Classification

References:	M. Levy and M. Sandler.
		Lightweight measures for timbral similarity of musical audio.
		In Proceedings of the 1st ACM workshop on Audio and Music
		Computing Multimedia, Santa Barbara, 2006.

		K. Jacobson.
		A Multifaceted Approach to Music Similarity.
		In Proceedings of the Seventh International Conference on Music
		Information Retrieval (ISMIR), 2006.


Constant-Q Spectrogram
----------------------

Identifier:	qm-constantq
Authors:	Christian Landone
Category:	Visualisation

References:	J. Brown.
		Calculation of a constant Q spectral transform.
		Journal of the Acoustical Society of America, 89(1):
		425-434, 1991.


Chromagram
----------

Identifier:	qm-chromagram
Authors:	Christian Landone
Category:	Visualisation

References:	


Mel-Frequency Cepstral Coefficients
-----------------------------------

Identifier:	qm-mfcc
Authors:	Nicolas Chetry and Chris Cannam
Category:	Low Level Features

References:	B. Logan.
		Mel-Frequency Cepstral Coefficients for Music Modeling.
		In Proceedings of the First International Symposium on Music
		Information Retrieval (ISMIR), 2000.


