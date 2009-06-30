
QM Vamp Plugins
===============

Vamp audio feature extraction plugins from the Centre for Digital
Music at Queen Mary, University of London.

http://www.elec.qmul.ac.uk/digitalmusic/

Version 1.6.

For more information about Vamp plugins, see http://www.vamp-plugins.org/ .


About This Release
==================

This is a major feature release, adding four new plugins (adaptive
spectrogram, polyphonic transcription, wavelet scalogram, and
bar-and-beat tracker) as well as a new method for the beat tracker.


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

These plugins are provided in binary form only.  You may install and
use the plugin binaries without fee for any purpose commercial or
non-commercial.  You may redistribute the plugin binaries provided you
do so without fee and you retain this README file with your
distribution.  You may not bundle these plugins with a commercial
product or distribute them on commercial terms.  If you wish to
arrange commercial licensing terms, please contact the Centre for
Digital Music at Queen Mary, University of London.

Copyright (c) 2006-2009 Queen Mary, University of London.  All rights
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

