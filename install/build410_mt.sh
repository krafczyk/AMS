#!/bin/tcsh
if ($?AMSWD  )  then

else
setenv AMSWD ..
if (-e $AMSWD) then
    else
    mkdir -p $AMSWD
    endif
endif    
echo "$AMSWD"
unsetenv DEBUGFLAG 
setenv VERBOSE 1

source /Offline/vdev/install/amsvar
source $ROOTSYS/../root-v5-34-9-icc64.14-slc6/amsvar
setenv G4MULTITHREADED 1
source /Offline/vdev/install/g4i10.1
source /afs/cern.ch/sw/IntelSoftware/linux/all-setup.csh 
source /afs/cern.ch/sw/IntelSoftware/linux/x86_64/xe2015/composerxe/bin/compilervars.csh   intel64
setenv INTELDIR /afs/cern.ch/sw/IntelSoftware/linux/x86_64
setenv INTELVER xe2015/composer_xe_2015.1.133
setenv PGTRACK 1
setenv G4AMS 1
gmake -j 50

gmake producer
gmake producer6


