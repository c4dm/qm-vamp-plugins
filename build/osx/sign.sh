#!/bin/bash 

set -eu

codesign -s "Developer ID Application: Chris Cannam" --options runtime -fv qm-vamp-plugins.dylib

