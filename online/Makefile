#  $Id: Makefile,v 1.3 2001/01/22 17:32:52 choutko Exp $
# Makefile for AMS Event Display for alpha/cxx 

PLATFORM      = alpha
#SHLD          = ld -L/usr/lib/cmplrs/cxx -rpath /usr/lib/cmplrs/cxx \
#                -expect_unresolved -g0 -O1 -shared /usr/lib/cmplrs/cc/crt0.o \
#                /usr/lib/cmplrs/cxx/_main.o
SHLD          = ld -L/usr/lib/cmplrs/cxx -rpath /usr/lib/cmplrs/cxx \
                -expect_unresolved -g -shared /usr/lib/cmplrs/cc/crt0.o \
                /usr/lib/cmplrs/cxx/_main.o -lcxxstd -lcxx -lexc -lots -lc

CXX           =  cxx
CXXFLAGS      =  -I$(ROOTSYS)/include -I/usr/include
OPT           = -O4 -DROOT10308
NOOPT         = 
LD            = cxx 
LDFLAGS       = -g
SOFLAGS       = -b
SOGFLAGS      = -b

ROOTLIBS      = -L$(ROOTSYS)/lib -lBase -lCint -lClib -lCont -lFunc -lGraf \
                -lGraf3d -lHist -lHtml -lMeta -lMinuit -lNet -lPostscript \
                -lProof -lRint -lTree -lUnix -lZip   # -lEG -lEGPythia
ROOTGLIBS     = -lGpad -lGX11 -lMotif -lWidgets -lX3d

LIBS          = $(ROOTLIBS) -lm 
GLIBS         = $(ROOTLIBS) $(ROOTGLIBS) -lXm -lXt -lX11 -lm -lPW

PYTHIALIB     = -L$(PYTHIA) -lPythia


##### RULES #####

.SUFFIXES: .cxx

.cxx.o:
	$(CXX) $(OPT) $(CXXFLAGS) -c $<


##### MACROS and TARGETS #####

include Make-bin


##### DEPENDENCIES #####

include Make-depend

