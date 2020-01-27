#!/bin/bash 

set -eu

codesign -s "Developer ID Application: Chris Cannam" -fv qm-vamp-plugins.dylib

