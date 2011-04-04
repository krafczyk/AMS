MARCH:= $(shell root-config --arch)
CXX:= $(shell root-config --cxx)

ROOTLIBS:= $(shell root-config --cflags --libs) -lMinuit -lNetx -lGui
#ROOTSYS:=`echo $ROOTSYS`
AMSSRC:=/scratch/kaiwu/AMS

LIBS= -L$(AMSWD)/lib/$(MARCH)/ -lntuple_slc4_PG $(ROOTLIBS)
#LIBS= $(ROOTLIBS)
SRC:=$(wildcard *.cc)
DictSrc:=MenuDict.cc
DictOBJ:=MenuDict.o
OBJ:=$(patsubst %.cc,%.o,$(SRC)) $(DictOBJ)

all:main
main:$(OBJ)
	$(CXX) -o main $(OBJ)  $(LIBS) -DSTANDALONE
.cc.o:$<
	$(CXX)  -I$(ROOTSYS)/include  -I$(AMSSRC)/include -c $< -o $@
%Dict.cc: Menu.h ProgBar.h myzoom.h LinkDef.h
	rootcint -f $@  -c -I$(ROOTSYS)/include  -I$(AMSSRC)/include $?

clean:
	@rm -f *.o main


