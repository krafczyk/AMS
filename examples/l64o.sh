#!/bin/sh
if [ -f geocoor.o ]
then
echo ""
else
ifort -c geocoor.f
fi
if [ -f geocoor_alig.o ]
then
echo ""
else
ifort -c geocoor_alig.f
fi

icc  -I/$ROOTSYS/include -I$AMSINCLUDE/include -D__root__new  -D__WRITEROOT__ -D__ROOTSHAREDLIBRARY__ -openmp -O -axsse4.2,ssse3 $1.C -c -o $1.o
icpc -static -O -openmp -openmp-link static  -axssse3  $1.o geocoor_alig.o -L$AMSINCLUDE/lib/linuxx8664icc -lntuple_slc4    geocoor.o -L$ROOTSYS/lib -lRoot -L/afs/cern.ch/exp/ams/Offline/CERN/2005/lib -lgeant321 -lminuit -lpacklib -lmathlib -lkernlib  -pthread -lpcre -lshift -lpcre  -lfreetype -lXrdClient -lXrd -lXrdNet -lXrdOuc -lXrdSys -L/opt/intel/Compiler/11.1/073/lib/intel64 -lifport -lifcoremt -limf  -lipgo -lirc_s -lguide -Wl,-whole-archive,--allow-multiple-definition -lshift -Wl,-no-whole-archive  -o $AMSWD/exe/linux/$1
chmod +x $AMSWD/exe/linux/$1
