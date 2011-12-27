#!/bin/sh
icc  -I/$ROOTSYS/include -I$AMSINCLUDE/include -D__root__new  -D__WRITEROOT__ -D__ROOTSHAREDLIBRARY__ -openmp -O3 -axssse3 $1.C -c -o $1.o
icpc -static -openmp -openmp-link static  -axssse3  $1.o -L$AMSINCLUDE/lib/linuxx8664icc -lntuple_slc4_PG  -O   -L$ROOTSYS/lib -lRoot -pthread -lpcre -lshift -lpcre  -lfreetype   -lXrdClient -lXrd -lXrdNet -lXrdOuc -lXrdSys -Wl,-whole-archive,--allow-multiple-definition -lshift -Wl,-no-whole-archive geocoor_alig.o geocoor.o -o $AMSWD/exe/$1
chmod +x $AMSWD/exe/$1
