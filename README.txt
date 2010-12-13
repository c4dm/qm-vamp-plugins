
QM Vamp Plugins
===============

Vamp audio feature extraction plugins from the Centre for Digital
Music at Queen Mary, University of London.

http://www.elec.qmul.ac.uk/digitalmusic/

Version 1.6.1.

For more information about Vamp plugins, see http://www.vamp-plugins.org/ .


About This Release
==================

This is a bug-fix release, fixing a failure to correctly smooth the
onset detection function which caused the onset and beat tracking
plugins occasionally to miss onsets or find spurious ones.


Plugins Included
================

This plugin set includes the following plugins:

   * Note onset detector

   * Beat tracker and tempo estimator

   * Key estimator and tonal change detector

   * Adaptive multi-resolution FFT spectrogram

   * Polyphonic note transcription estimator

   * Segmenter, to divide a track into a consistent sequence of segments

   * Timbral and rhythmic similarity between audio tracks

   * Wavelet scalogram (discrete wavelet transform)

   * Chromagram, constant-Q spectrogram, and MFCC calculation plugins

For full details about the plugins, with references, please see

  http://vamp-plugins.org/plugin-doc/qm-vamp-plugins.html


License
=======

These plugins are provided under the terms of the GNU General Public
License.  You may install and use the plugin binaries without fee for
any purpose commercial or non-commercial.  You may also modify and
redistribute the plugins in source or binary form, provided you comply
with the terms given by the GNU General Public License.  See the file
COPYING for more details.

If you wish to use these plugins in a proprietary application or
product for which the terms of the GPL are not appropriate, please
contact the Centre for Digital Music at Queen Mary, University of
London for further licensing terms.

Copyright (c) 2006-2010 Queen Mary, University of London.


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

