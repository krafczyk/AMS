#!/bin/sh
icc  -I/$ROOTSYS/include -I$AMSWD/include -D__root__new  -D__WRITEROOT__ -D__ROOTSHAREDLIBRARY__ -openmp -O3 -axssse3 $1.C -c -o $1.o
icpc -static -openmp -openmp-link static  -axssse3  $1.o -L$AMSWD/lib/linuxx8664icc -lntuple_slc4_PG  -O   -L$ROOTSYS/lib -lRoot -pthread -lpcre -lfreetype -lshift -lpcre  -lXrdClient -lXrd -lXrdNet -lXrdOuc -lXrdSys -Wl,-whole-archive,--allow-multiple-definition -lshift -Wl,-no-whole-archive -o $AMSWD/exe/linuxx8664icc/$1
chmod +x $AMSWD/exe/linuxx8664icc/$1
