# Makefile for AMS Event Display for alpha/cxx 

PLATFORM      = alpha
#SHLD          = ld -L/usr/lib/cmplrs/cxx -rpath /usr/lib/cmplrs/cxx \
#                -expect_unresolved -g0 -O1 -shared /usr/lib/cmplrs/cc/crt0.o \
#                /usr/lib/cmplrs/cxx/_main.o
SHLD          = ld -L/usr/lib/cmplrs/cxx -rpath /usr/lib/cmplrs/cxx \
                -expect_unresolved -g -shared /usr/lib/cmplrs/cc/crt0.o \
                /usr/lib/cmplrs/cxx/_main.o -lcxxstd -lcxx -lexc -lots -lc


CXX           =  cxx
CXXFLAGS      =  -D__osf__ -D__alpha -I$(ROOTSYS)/include -I/usr/include
OPT           = -O4 -DROOT10308
NOOPT         =
LD            = cxx
LDFLAGS       = -g
SOFLAGS       = -Wl,-expect_unresolved,* -call_shared
SOGFLAGS      = -b

ROOTLIBS      = -L$(ROOTSYS)/lib -lNew -lBase -lCint -lClib -lCont -lFunc -lGraf \
                -lGraf3d -lHist -lHtml -lMatrix -lMeta -lMinuit -lNet -lPostscript \
                -lProof -lRint -lTree -lUnix -lZip   # -lEG -lEGPythia
ROOTGLIBS     = -lGpad -lGui -lGX11   -lX3d

LIBS          = $(ROOTLIBS) -lm
GLIBS         = $(ROOTLIBS) $(ROOTGLIBS) -lXpm  -lX11 -lm -lPW



PYTHIALIB     = -L$(PYTHIA) -lPythia


##### RULES #####

.SUFFIXES: .cxx

.cxx.o:
	$(CXX) $(OPT) $(CXXFLAGS) -c $<


##### MACROS and TARGETS #####

include Make-bin


##### DEPENDENCIES #####

include Make-depend

