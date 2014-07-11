export Offline=/afs/cern.ch/exp/ams/Offline/
export ROOTSYS=/afs/cern.ch/exp/ams/Offline/root/$OS/527.icc64
export PATH=$ROOTSYS/bin:/afs/cern.ch/ams/opt/intel/Compiler/11.1/073/bin/intel64:/usr/sbin:$PATH 
export LD_LIBRARY_PATH=/afs/cern.ch/ams/opt/intel/Compiler/11.1/073/idb/lib/intel64/:/afs/cern.ch/ams/local2/opt/intel/Compiler/11.1/073/lib/intel64/:$ROOTSYS/lib:/opt/intel/compiler80/lib:$LD_LIBRARY_PATH
export AMSDataDir=$Offline/AMSDataDirRW
export AMSDataDirRW=$Offline/AMSDataDirRW
export AMSDynAlignment=$AMSDataDirRW/ExtAlig/AlignmentFiles/
export ACROOTSOFTWARE=$AMSDataDir/v5.00/TRD
export ACROOTLOOKUPS=$ACROOTSOFTWARE/acroot/data/
export AMSWD=$Offline/vdev/
export AC=$Offline/vdev/ACsoft
export AMSana=$PWD/
