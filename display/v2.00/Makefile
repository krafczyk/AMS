
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
 FF            = xlf
 FFLAGS        = -qextname
 OPT           = -g -DROOT10308
 NOOPT         =
 LD            = xlC
 LDFLAGS       = 
 SOFLAGS       = -b
 SOGFLAGS      = -b
 FORLIBS       = -lxlf90 -lxlf -L/cern/pro/lib -lmathlib -lpacklib -lkernlib
 ADDLIBS       =
 ROOTLIBS      = -L$(ROOTSYS)/lib -lRoot -lCint
 ROOTGLIBS     = -lGpad -lGX11 -lMotif -lWidgets -lX3d
 LIBS          = $(ROOTLIBS) -lm -lcurses $(FORLIBS)
 GLIBS         = $(ROOTLIBS) -lXm -lXt -lX11 -lm -lPW -lld -lcurses $(FORLIBS)
 SHLD          = /usr/lpp/xlC/bin/makeC++SharedLib $(LIBS) -p 0
 ARLD          = ar vr
endif

##############
#            #
#  DEC/OSF1  #
#  =======   #
##############
ifeq  "$(OS)"  "OSF1"
 PLATFORM      = osf
 CXX           = cxx
 CXXFLAGS      = -D__osf__ -D__alpha -D_ALPHA_ -I$(ROOTSYS)/include \
                 -I/usr/include -I/usr/include/cxx
 OPT           = -g -DROOT10308
 NOOPT         =
 FF            = f77
 FFLAGS        =
 LD            = cxx
 LDFLAGS       = -g
 SOFLAGS       =
 SOGFLAGS      =
# SOFLAGS       = -Wl,-expect_unresolved,* -call_shared
# SOGFLAGS      = -Wl,-expect_unresolved,* -call_shared

 FORLIBS       = -L/cern/pro/lib -lmathlib -lpacklib -lkernlib \
                 -lUfor -lfor -lFutil
 ADDLIBS       = $(FORLIBS)
 ROOTLIBS      = -L$(ROOTSYS)/lib -lNew -lBase -lCint -lClib -lCont -lFunc \
                 -lGraf -lGraf3d -lHist -lHtml -lMatrix -lMeta -lMinuit -lNet \
                 -lPostscript -lProof -lTree -lUnix -lZip
 ROOTGLIBS     = -lGpad -lGui -lGX11 -lX3d
 LIBS          = $(ROOTLIBS) -lm $(FORLIBS)
 GLIBS         = $(ROOTLIBS) $(ROOTGLIBS) -lXpm -lX11 -lm -lPW $(FORLIBS)
 SHLD          = ld -L/usr/lib/cmplrs/cxx -rpath /usr/lib/cmplrs/cxx \
                 -expect_unresolved -g -shared /usr/lib/cmplrs/cc/crt0.o \
                 /usr/lib/cmplrs/cxx/_main.o -lcxxstd -lcxx -lexc -lots -lc
 ARLD          = ar vr
endif


##### RULES #####

.SUFFIXES:
.SUFFIXES: .cxx .f

$(OBJDIR)/%.o:    %.cxx
	$(CXX) $(OPT) $(CXXFLAGS) -c $< -o $@

$(OBJDIR)/%.o:    %.f
	$(FF)  $(OPT) $(FFLAGS) -c $< -o $@
#.cxx.o:
#	$(CXX) $(OPT) $(CXXFLAGS) -c $<


##### MACROS and TARGETS #####

include Make-bin


##### DEPENDENCIES #####

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(MAKECMDGOALS),touch_depend)
 include $(OBJDIR)/Make-depend
endif
endif
