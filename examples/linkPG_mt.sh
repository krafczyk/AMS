#!/bin/sh
icc  -I/$ROOTSYS/include -I../include -D__root__new  -D__WRITEROOT__ -D__ROOTSHAREDLIBRARY__ -openmp -g -axssse3 $1.C -c -o $1.o
icpc -static -openmp -openmp-link static  -axssse3  $1.o -L$AMSWD/lib/linuxicc -lntuple_slc4_PG  -g   -L$ROOTSYS/lib -lRoot -pthread -lpcre -lshift -lpcre  -lXrdClient -lXrd -lXrdNet -lXrdOuc -lXrdSys -o $AMSWD/exe/linux/$1
chmod +x $AMSWD/exe/linux/$1
