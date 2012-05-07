
MARCH:=`root-config --arch`
CXX:=`root-config --cxx`


SRC_U= analyze.C 


OBJS_U=$(SRC_U:%.C=%.o)

CPPFLAGS=-D_PGTRACK_ -I. -Ilib -I$(AMSSRC)/include -I$(ROOTSYS)/include
CXXFLAGS= -g -Wno-write-strings  $(CPPFLAGS)

main: static
static: analyze
shared: analyze_shared

analyze:  $(SRC_U) 
	$(CXX) -o $@ $(CXXFLAGS) $^  -L$(AMSWD)/lib/$(MARCH)/ -lntuple_slc4_PG `root-config --libs` -lMinuit -lNetx -lProof -lProofPlayer -lTMVA -lXMLIO -lMLP -lTreePlayer
analyze_shared:  $(SRC_U) 
	$(CXX) -o $@ $(CXXFLAGS) $^  -L$(AMSWD)/lib/$(MARCH)/ntuple_slc4_PG.so `root-config --libs` -lMinuit -lNetx -lProof -lProofP

clean:
	rm -fv *.o
	rm -fv analyze analyze_shared

