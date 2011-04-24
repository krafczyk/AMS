MARCH:= $(shell root-config --arch)
CXX:= $(shell root-config --cxx)

ROOTLIBS:= $(shell root-config --cflags --libs) -lMinuit -lNetx -lGui
#ROOTSYS:=`echo $ROOTSYS`
ifdef AMSSRC
#AMSSRC:=$(AMSSRC)
AMSSRC:=/Offline/B500_patches/
else 
AMSSRC:=.. 
endif

LIBS= -L$(AMSSRC)/lib/$(MARCH)/ -lntuple_slc4_PG $(ROOTLIBS)
#LIBS= -L/afs/cern.ch/ams/Offline/vdev/lib/linuxx8664icc/ -lntuple_slc4_PG $(ROOTLIBS)
#LIBS= $(ROOTLIBS)
SRC:=$(wildcard *.cc)
DictSrc:=MenuDict.cc
DictOBJ:=MenuDict.o
OBJ:=$(patsubst %.cc,%.o,$(SRC)) $(DictOBJ)

all:prodmon
prodmon:$(OBJ)
	$(CXX) -o prodmon $(OBJ)  $(LIBS) -DSTANDALONE
.cc.o:$<
	$(CXX)  -I$(ROOTSYS)/include  -I$(AMSSRC)/include -c $< -o $@
%Dict.cc: Menu.h ProgBar.h myzoom.h InputDialog.h LinkDef.h
	rootcint -f $@  -c -I$(ROOTSYS)/include  -I$(AMSSRC)/include $?

clean:
	@rm -f *.o prodmon


