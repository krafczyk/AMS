
# ------------------------------
# Makefile for AMS Event Display
# ------------------------------
#
# Requirements:
#                1) Gnu-make should be used
#                2) ROOTSYS should be defined for location of ROOT
#****************************************************************

#--gcc used to produce dependencies
GCC  = gcc

####################
#                  #
# operating system #
#                  #
####################

OS  = $(shell uname)
#--Directory to put object files, executive and library produced
OBJDIR = ./$(OS)/bin

##############
#            #
#  IBM/AIX   #
#  =======   #
##############
ifeq  "$(OS)"  "AIX"
 PLATFORM      = aix
 CXX           = xlC
 CXXFLAGS      = -O -qnoro -qnoroconst -qmaxmem=-1 -I$(ROOTSYS)/include \
                 -I/usr/lpp/xlC/include
 OPT           = -g -DROOT10308
 NOOPT         =
 LD            = xlC
 LDFLAGS       = -g
 SOFLAGS       = -b
 SOGFLAGS      = -b
 DEPEND_INC    = -I$(ROOTSYS)/include -I/usr/include -I/usr/local/include \
                 -I/usr/lpp/xlC/include
 ROOTLIBS      = -L$(ROOTSYS)/lib -lRoot -lCint
 ROOTGLIBS     = -lGpad -lGX11 -lMotif -lWidgets -lX3d
 LIBS          = $(ROOTLIBS) -lm -lcurses
 GLIBS         = $(ROOTLIBS) -lXm -lXt -lX11 -lm -lPW -lld -lcurses
 SHLD          = /usr/lpp/xlC/bin/makeC++SharedLib $(LIBS) -p 0
endif

##############
#            #
#  DEC/OSF1  #
#  =======   #
##############
ifeq  "$(OS)"  "OSF1"
 PLATFORM      = osf
 CXX           = cxx
 CXXFLAGS      = -D__osf__ -D__alpha -I$(ROOTSYS)/include \
                 -I/usr/include -I/usr/include/cxx
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
endif


##### RULES #####

.SUFFIXES: .cxx

$(OBJDIR)/%.o:    %.cxx
	$(CXX) $(OPT) $(CXXFLAGS) -c $< -o $@
#.cxx.o:
#	$(CXX) $(OPT) $(CXXFLAGS) -c $<


##### MACROS and TARGETS #####

include Make-bin


##### DEPENDENCIES #####

ifneq ($(MAKECMDGOALS),clean)
 include $(OBJDIR)/Make-depend
endif
