#!/bin/bash

if [ "$#" == "1" ]; then
  file=$1
else
  echo "You have to provide the name of a C++ file. For instance: \"ana.C\"; EXIT"
  exit
fi

if [ -e "${file}" ]; then
    suffix2=`echo $file | awk '{print substr($1,length($1)-1,2)}'`
    suffix3=`echo $file | awk '{print substr($1,length($1)-2,3)}'`
    suffix4=`echo $file | awk '{print substr($1,length($1)-3,4)}'`
    if [ "$suffix2" = ".C" ]; then
      FILE_EXE=`echo $file | awk '{print substr($1,1,length($1)-2)}'`.exe
    elif [ "$suffix3" = ".cc" ]; then
      FILE_EXE=`echo $file | awk '{print substr($1,1,length($1)-3)}'`.exe
    elif [ "$suffix4" = ".cxx" ]; then
      FILE_EXE=`echo $file | awk '{print substr($1,1,length($1)-4)}'`.exe
    else
      echo "Wrong input file \""${file}\""; EXIT "
      exit
    fi
else
    echo "Wrong input file \""${file}\""; EXIT "
    exit
fi

GC="g++ -Wno-deprecated"
AMSWD="/afs/ams.cern.ch/Offline/vdev"
ROOTSYS="/afs/cern.ch/exp/ams/Offline/root/Linux/new"
INCS="-I${AMSWD}/include -I${ROOTSYS}/include"
ROOTLIBS="-L${ROOTSYS}/lib -lRoot"
EXTRALIBS="-ldl -lcrypt"
AMSOBJS="${AMSWD}/bin/linux/icc/root_rs.o ${AMSWD}/bin/linux/icc/rootdict_s.o"

echo -e "\n>>> COMPILING and LINKING: ${file}"

/usr/bin/make -f - -s <<!
SHELL=/bin/sh

${file}.exe:	${file}
	${GC} ${INCS} -o ${FILE_EXE} ${file} ${ROOTLIBS} ${EXTRALIBS} ${AMSOBJS}
	chmod 755 ${FILE_EXE}

!

echo -e "\n>>> EXECUTING file: ${FILE_EXE}"
${FILE_EXE}
