#!/bin/tcsh
unsetenv DEBUGFLAG 
setenv VERBOSE 1
unsetenv G4MULTITHREADED
source $Offline/vdev/install/amsvar
source $ROOTSYS/../root-v5-34-9-icc64.14-slc6/amsvar
source $Offline/vdev/install/g4i10.1
source /afs/cern.ch/sw/IntelSoftware/linux/all-setup.csh 
source /afs/cern.ch/sw/IntelSoftware/linux/x86_64/xe2015/composerxe/bin/compilervars.csh   intel64
setenv INTELDIR /afs/cern.ch/sw/IntelSoftware/linux/x86_64
setenv INTELVER xe2015/composer_xe_2015.1.133
setenv PGTRACK 1
setenv AMSWD  ../g410
setenv G4AMS 1
gmake -j 50

gmake producer -j
gmake producer6 -j
source $Offline/vdev/install/amsvar
source $ROOTSYS/../root-v5-34-9-gcc64/amsvar
source $Offline/vdev/install/g4i10.1
source /afs/cern.ch/sw/lcg/external/gcc/4.8.1/x86_64-slc6/setup.csh  /afs/cern.ch/sw/lcg/external

setenv PGTRACK 1
setenv AMSWD  ../g410
setenv G4AMS 1
gmake -j 50
gmake producer -j
gmake producer6 -j

