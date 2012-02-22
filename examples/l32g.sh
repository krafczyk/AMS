#!/bin/sh
if [ -f geocoor32.o ]
then
echo ""
else
g77 -m32 -c geocoor.f -o geocoor32.o
fi
if [ -f geocoor_alig32.o ]
then
echo ""
else
g77 -m32 -c geocoor_alig.f -o geocoor_alig32.o
fi
g++ -m32 -I/$ROOTSYS/include -I$AMSINCLUDE/include -D__root__new  -D__WRITEROOT__ -D__ROOTSHAREDLIBRARY__  -g  $1.C -c -o $1.o
g++ -m32 -static -g    $1.o geocoor_alig32.o -L$AMSINCLUDE/lib/linux -lntuple_slc4    geocoor32.o -L$ROOTSYS/lib -lRoot -L/afs/cern.ch/ams/Offline/AMSsoft/linux_slc5_gcc32/2005/lib -lgeant321  -lpacklib -lmathlib -lkernlib  -lgfortran -pthread -lpcre   -lfreetype -lXrdClient -lXrd -lXrdNet -lXrdOuc -lXrdSys -ldl -Wl,-whole-archive,--allow-multiple-definition -lshift -Wl,-no-whole-archive  -o $AMSWD/exe/linux/$1
chmod +x $AMSWD/exe/linux/$1
