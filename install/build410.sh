#!/bin/tcsh
#source /afs/cern.ch/sw/lcg/external/gcc/4.8.1/x86_64-slc6/setup.csh  /afs/cern.ch/sw/lcg/external
source /Offline/vdev/install/amsvar
source $ROOTSYS/../root-v5-34-9-icc64.14-slc6/amsvar
source /Offline/vdev/install/g4i10.1
setenv PGTRACK 1
setenv AMSWD  ../g410
setenv G4AMS 1
gmake -j 50

gmake producer
gmake producer6
source /Offline/vdev/install/amsvar
source $ROOTSYS/../root-v5-34-9-gcc64/amsvar
source /Offline/vdev/install/g4i10.1
setenv PGTRACK 1
setenv AMSWD  ../g410
setenv G4AMS 1
gmake -j 50
gmake producer
gmake producer6

