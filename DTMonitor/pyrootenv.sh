
export Offline=/afs/cern.ch/ams/Offline
export ROOTSYS=/afs/cern.ch/exp/ams/Offline/root/$OS/527.icc64
export PATH=$PATH:$ROOTSYS/bin:/opt/intel/Compiler/11.1/073/bin/intel64:.:/usr/sbin
export LD_LIBRARY_PATH=.:/opt/intel/Compiler/11.1/073/idb/lib/intel64/:/opt/intel/Compiler/11.1/073/lib/intel64/:$ROOTSYS/lib:/opt/intel/compiler80/lib
export CVSROOT=$Offline/CVS
#setenv CVSROOT :ext:`whoami`@ams.cern.ch:/cvsroot
export CVS_RSH=ssh
export ProductionRunsDir=/s0dat0/Data/Deframing
export AMSDataDir=$Offline/AMSDataDirRW
export AMSDataDirRW=$Offline/AMSDataDirRW
export EDITOR=vim
export CVSEDITOR=vim
export RunsDir=$Offline/RunsDir
export AMSDataNtuplesDir=$Offline/ntuples
export AMSMCNtuplesDir=$Offline/amsmcntuplesdir
export ProductionLogDirLocal=$Offline/logs.local
export RunsSummaryFile=$Offline/runs_STS91.log
export SlowRateDataDir=$Offline/SlowRateDataDir
export AMSSRC=/Offline/vdev
export AMSGeoDir=$Offline/vdev/display/
export amsed=$Offline/vdev/exe/linuxx8664icc/amsed
export amsedc=$Offline/vdev/exe/linuxx8664icc/amsedc
export amsedPG=$Offline/vdev/exe/linuxx8664icc/amsedPG
export amsedcPG=$Offline/vdev/exe/linuxx8664icc/amsedcPG

export offmon=$Offline/vdev/exe/linux/offmon      
export offmonc=$Offline/vdev/exe/linux/offmonc      
export AMSICC=1
export AMSP=1
export STAGE_HOST= castorpublic
export RFIO_USE_CASTOR_V2=YES
export STAGE_SVCCLASS=amscdr
export CASTOR_INSTANCE=castorpublic
export LD_LIBRARY_PATH=$ROOTSYS/lib:$PYTHONDIR/lib:$LD_LIBRARY_PATH
export PYTHONPATH=$ROOTSYS/lib:$PYTHONPATH
