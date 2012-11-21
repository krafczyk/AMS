
MARCH:=$(shell root-config --arch)
CXX:=`root-config --cxx`

SRC_U=preselect_ele_pos.C trdqt.C

OBJS_U=$(SRC_U:%.C=%.o)

CPPFLAGS=-DAMS_ACQT_INTERFACE -D_PGTRACK_ -I. -Ilib -I$(AMSWD)/include -I$(AMSWD)/ACsoft/acroot -I$(AMSWD)/ACsoft/interface -I$(ROOTSYS)/include
CXXFLAGS=-Wno-write-strings  $(CPPFLAGS)

main: static
static: trdqt_example

ifeq ($(MARCH),macosx64)
  EXTRA_FLAGS=-F$(QTDIR)/lib -framework QtCore 
endif

trdqt_example: $(OBJS_U)
	$(CXX) -o $@ $(CXXFLAGS) $^ $(EXTRA_FLAGS) $(AMSWD)/lib/$(MARCH)/ntuple_slc4_PG.so `root-config --libs` -lMinuit -lNetx -lProof -lProofPlayer -lTMVA -lXMLIO -lMLP -lTreePlayer

%.o: %.C
	$(CXX) -o $@ $(CXXFLAGS) -c $^ $(EXTRA_FLAGS)

clean:
	rm -fv *.o
	rm -fv trdqt_example
