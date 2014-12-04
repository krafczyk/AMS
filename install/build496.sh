#!/bin/tcsh
source $Offline/vdev/install/amsvar
source $ROOTSYS/../root-v5-34-9-icc64.14-slc6/amsvar
source $Offline/vdev/install/g4i9.63
setenv PGTRACK 1
setenv AMSWD ..
setenv G4AMS 1
#source /afs/cern.ch/sw/lcg/external/gcc/4.8.1/x86_64-slc6/setup.csh  /afs/cern.ch/sw/lcg/external

gmake -j 50
setenv LD_LIBRARY_PATH /afs/cern.ch/ams/local2/opt/Orbit2/lib:$LD_LIBRARY_PATH
gmake producer
gmake producer6
