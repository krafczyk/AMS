#!/bin/sh 

function mexport {
if [ "$MYSHELL" = "csh" ];then
echo setenv $1 $2 >>$FILEOUT
else
echo export $1=$2 >>$FILEOUT
fi
}

function munexport {
if [ "$MYSHELL" = "csh" ];then
echo unsetenv $1  >>$FILEOUT
else
echo unset $1 >>$FILEOUT
fi
}
function IfTest {
if [ "$MYSHELL" = "csh" ] ; then
echo "if ( ! \$?$1 ) then " >>$FILEOUT
else
echo "if [[ -z \"\$$1\" ]] ; then" >>$FILEOUT
fi

}

function IfTestNot {
if [ "$MYSHELL" = "csh" ] ; then
echo "if (  \$?$1 ) then " >>$FILEOUT
else
echo "if [[ ! -z \"\$$1\" ]] ; then" >>$FILEOUT
fi

}

function mecho {
echo $1 >>$FILEOUT    
}


function IfThen {
if [ "$MYSHELL" = "csh" ];then
echo "if ( $1 == $2 ) then"  >>$FILEOUT
else
echo "if [ $1 = $2 ] ; then">>$FILEOUT
fi
}

function ElseIfThen {
if [ "$MYSHELL" = "csh" ];then
echo "else if ( $1 == $2 ) then"  >>$FILEOUT
else
echo "elif [ $1 = $2 ] ; then">>$FILEOUT
fi
}

function Else {
if [ "$MYSHELL" = "csh" ];then
echo else >>$FILEOUT
else
echo else >>$FILEOUT
fi
}


function EndIf {
if [ "$MYSHELL" = "csh" ];then
echo endif >>$FILEOUT
else
echo fi >>$FILEOUT
fi

}
#####################################################################
echo 
echo 
echo  "   Configuration Script Generator for AMS  " 
echo 
echo 
echo " This scripts generate the configuration file (amsvar) for the "
echo " AMS software (GBATCH). Supported architecture are "
echo 
echo "   SLC 5  64 bits  (gcc, icc)"
echo "   SLC 6  64 bits  (gcc, icc)"
echo 
echo " the generated script automatically recognize if you are on SLC5 or SLC6"
echo " you need only to choose which shell you want: (ba)sh, (t)csh" 
echo " and which compiler (gcc, icc)"
echo 

OPTIONS="tcsh  bash"
select opt in $OPTIONS; do
  if [ "$opt" = "bash" ]; then
    export MYSHELL=sh
    break
  elif [ "$opt" = "tcsh" ];then
    export MYSHELL=csh
    break
  else
    echo bad option
  fi
done


echo Wich compiler suit GNU or INTEL?
OPTIONS="gcc icc"
select opt in $OPTIONS;do
  if [ "$opt" = "icc" ];then
     export COMP=1
     export COMPK=icc
     break
  elif [ "$opt" = "gcc" ];then
     export COMP=1
     export COMPK=gcc
     break
  else
     echo bad option
  fi
done
FILEOUT=amsvar_$COMPK.$MYSHELL
echo " The AMS Software dir is found under the directory pointed by the"
echo " variable \$Offline"
echo " In the created script this variable is set to:"
echo
echo " \$Offline=/cvmfs/ams.cern.ch/Offline/ "
echo
echo " you may want to edit $FILEOUT to customize it"
echo
echo






rm -f $FILEOUT
mecho "#Switch for gcc or icc"
mexport USE_ICC $COMP 
 
mecho 
mecho
mecho "#  Set the offline directory"
IfTest Offline 
mexport Offline  /cvmfs/ams.cern.ch/Offline/
echo -n "#" >>$FILEOUT
mexport Offline  /afs/cern.ch/ams/Offline/
echo -n "#" >>$FILEOUT
mexport Offline  /Offline
EndIf

mecho 
mecho "#Where is ICC ?"
mecho
IfTest INTEL 
#mexport INTEL  /afs/cern.ch/ams/opt/intel
mexport INTEL  /afs/cern.ch/ams/local2/opt/intel
EndIf

mecho 
mecho
mecho "#  Which SLC are we using ?"
mecho
#get SLC 5 or 6
mexport DD  "\`cat /etc/redhat-release|awk '{print \$6}'|cut -c1\`"
mexport SLC  slc5
IfThen  \$DD    5 
mexport INTEL_LICENSE_FILE  \$Offline/intel/licenses
mexport PATH  \$INTEL/Compiler/11.1/073/bin/intel64:\$PATH
mexport LD_LIBRARY_PATH  \$INTEL/Compiler/11.1/073/idb/lib/intel64/:\$INTEL/Compiler/11.1/073/lib/intel64/:\$INTEL/compiler80/lib:\$LD_LIBRARY_PATH


ElseIfThen \$DD  6 
mexport SLC  slc6
mexport INTEL_LICENSE_FILE  \$Offline/intel/licenses
mexport  LD_LIBRARY_PATH  \$INTEL/composer_xe_2013_sp1.1.106/compiler/lib/intel64/:\$LD_LIBRARY_PATH
mexport  PATH  \$INTEL/composer_xe_2013_sp1.1.106/compiler/bin/intel64:\$PATH

Else

echo "echo Your system is not suppoerted" >>$FILEOUT
echo "cat /etc/redhat-release" >>$FILEOUT
echo "exit -1" >>$FILEOUT
EndIf

mecho 
mecho
mexport CXX  g++
mexport CC  gcc
mexport FC  gfortran

IfThen \$USE_ICC  1

mexport CXX  icc
mexport CC  icc
mexport FC  ifort
mexport AMSICC  1

EndIf
mecho
mecho
mecho "# Set Up ROOT Enviroment"
mecho
# setup ROOT
mexport ROOTSYS  \$Offline/AMSsoft/linux_\$SLC\\_\$CC\\64/root534/
mexport PATH  \$Offline/AMSsoft/linux_\$SLC\\_\$CC\\64/root534/bin/:\$Offline/AMSsoft/linux_\$SLC\\_\$CC\\64/xrootd/bin:\$PATH
mexport LD_LIBRARY_PATH  \$Offline/AMSsoft/linux_\$SLC\\_\$CC\\64/root534/lib:\$Offline/AMSsoft/linux_\$SLC\\_\$CC\\64/xrootd/lib64:\$LD_LIBRARY_PATH

mecho
mecho
mecho "# Set Up GEANT4 Enviroment"
mecho

#setup GEANT4
mexport CCWD \$PWD
mecho "cd \$Offline/AMSsoft/linux_\$SLC\_\$CC\64/geant4_ams/bin/"
mecho "source geant4.$MYSHELL"
mecho "cd $CCWD"
mexport G4INSTALL  \$Offline/AMSsoft/linux_\$SLC\\_\$CC\\64/geant4_ams
mexport G4SYSTEM  Linux-\$CXX
mexport G4DPMJET2_5DATA  \$Offline/AMSsoft/shared/geant4/data/DPMJET/GlauberData

mecho
mecho
mecho

IfThen  "\`\$ROOTSYS/bin/root-config --arch\`"  \"linuxx8664icc\"
mexport G4LIB_BUILD_GDML  1
mexport G4SYSTEM  Linux-icc
EndIf

mecho


IfThen "\`\$ROOTSYS/bin/root-config --arch\`"  \"linuxx8664gcc\"
mexport G4LIB_BUILD_GDML  1
mexport G4SYSTEM  Linux-g++
IfTestNot DEBUGFLAG 
mexport G4SYSTEM  Linux-g++.debug
EndIf
EndIf

mecho

IfThen "\`\$ROOTSYS/bin/root-config --arch\`"   \"linuxia64ecc\" 
mexport G4SYSTEM  Linux-icc.ia64
EndIf
mecho

echo -n # >>$FILEOUT

IfThen \$USE_ICC  1
munexport VGM_INSTALL 
Else
mexport VGM_INSTALL \$G4INSTALL/../vgmams 

EndIf


mecho
mecho
mexport CVSROOT  \$Offline/CVS
mexport CVS_RSH  ssh
mexport AMSDataDir  \$Offline/AMSDataDirRW
mexport AMSDataDirRW  \$Offline/AMSDataDirRW
mexport CASTORSTATIC  1

mexport RunsDir  \$Offline/RunsDir
mexport AMSDataNtuplesDir  \$Offline/ntuples
mexport AMSMCNtuplesDir  \$Offline/amsmcntuplesdir
mexport ProductionLogDirLocal  \$Offline/logs.local
mexport RunsSummaryFile  \$Offline/runs_STS91.log
mexport SlowRateDataDir  \$Offline/SlowRateDataDir
mexport AMSGeoDir  \$Offline/vdev/display/
mexport amsed  \$Offline/vdev/exe/linuxicc/amsed
mexport amsedc  \$Offline/vdev/exe/linuxicc/amsedc
mexport amsedPG  \$Offline/vdev/exe/linux/amsedPG
mexport amsedcPG  \$Offline/vdev/exe/linux/amsedcPG

mexport offmon  \$Offline/vdev/exe/linuxicc/offmon      
mexport offmonc  \$Offline/vdev/exe/linuxicc/offmonc      

mexport AMSDynAlignment  \$AMSDataDirRW/ExtAlig/AlignmentFiles/

mecho
mecho
mecho

mexport PGTRACK  1
mexport G4AMS  1
mexport GEANT4NEW  1
mexport AMSP  1

mecho
mecho

IfTest AMSWD 
mexport AMSWD  ..
EndIf

mecho
mecho

mecho "# Enable Aachen TrdQt interface"
mexport AMS_ACQT_INTERFACE  1
mexport ACROOTSOFTWARE  \$AMSDataDir/v5.00/TRD
mexport ACROOTLOOKUPS  \$ACROOTSOFTWARE/acroot/data/
mexport QTDIR_STATIC  \$Offline/AMSsoft/AMSQtCore/qt-4.8.3/
mexport QTDIR_STATIC_32  \$Offline/AMSsoft/AMSQtCore/qt-4.8.3-32bit/

mecho
mecho




mexport EDITOR  emacs


mexport CVSEDITOR  emacs

echo
echo "  The file  $FILEOUT has been created."
echo
echo
