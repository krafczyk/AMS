# Makefile for AMS Event Display for Alpha-osf/cxx

PLATFORM      = osf

# Alpha/OSF with cxx
CXX           = cxx
CXXFLAGS      = -D__osf__ -D__alpha -I$(ROOTSYS)/include -I/usr/include -I/usr/include/cxx
OPT           = -g -DROOT10308
NOOPT         =
LD            = cxx
LDFLAGS       = -g
SOFLAGS       = -Wl,-expect_unresolved,* -call_shared
SOGFLAGS      = -Wl,-expect_unresolved,* -call_shared

ROOTLIBS      = -L$(ROOTSYS)/lib -lNew -lBase -lCint -lClib -lCont -lFunc \
                -lGraf -lGraf3d -lHist -lHtml -lMatrix -lMeta -lMinuit -lNet \
                -lPostscript -lProof -lTree -lUnix -lZip
ROOTGLIBS     = -lGpad -lGui -lGX11 -lX3d

LIBS          = $(ROOTLIBS) -lm
GLIBS         = $(ROOTLIBS) $(ROOTGLIBS) -lXpm -lX11 -lm -lPW

SHLD          = ld -L/usr/lib/cmplrs/cxx -rpath /usr/lib/cmplrs/cxx \
                -expect_unresolved -g -shared /usr/lib/cmplrs/cc/crt0.o \
                /usr/lib/cmplrs/cxx/_main.o -lcxxstd -lcxx -lexc -lots -lc

DEPEND_INC    = -I$(ROOTSYS)/include -I$(ROOTSYS)/cint/include

##### RULES #####

.SUFFIXES: .cxx

.cxx.o:
	$(CXX) $(OPT) $(CXXFLAGS) -c $<


##### MACROS and TARGETS #####

include Make-bin


##### DEPENDENCIES #####

include Make-depend

