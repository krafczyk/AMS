
export ROOTSYS=/afs/cern.ch/exp/ams/Offline/root/$OS/527.icc64
export PATH=$PATH:$ROOTSYS/bin:/opt/intel/Compiler/11.1/073/bin/intel64:.:/usr/sbin
export LD_LIBRARY_PATH=.:/opt/intel/Compiler/11.1/073/idb/lib/intel64/:/opt/intel/Compiler/11.1/073/lib/intel64/:$ROOTSYS/lib:/opt/intel/compiler80/lib
export LD_LIBRARY_PATH=$ROOTSYS/lib:$PYTHONDIR/lib:$LD_LIBRARY_PATH
export PYTHONPATH=$ROOTSYS/lib:$PYTHONPATH
