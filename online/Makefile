#  $Id: Makefile,v 1.55 2012/04/27 17:03:13 choutko Exp $
# PZ reviewing the online  Makefile on Aug 2009
#    WIN32 ans MACOSX arch commented as not supported in GBATCH for the moment
#    LINUX arch supported is SLC4 with gcc or  icc

#    WIN32 ans MACOSX arch commented as not supported in GBATCH for the moment
#    LINUX arch supported is SLC4 with gcc or  icc


ifndef AMSWD
AMSWD := ..
endif

ARCH         := $(shell $(ROOTSYS)/bin/root-config --arch )
PLATFORM      =  Linux
PREFIX        = $(AMSWD)/bin/$(ARCH)/online/
DEPDIR        = $(PREFIX)/dep

AMSLIB        = ntuple_slc4
AMSPRG        = $(AMSWD)/exe/$(ARCH)/offmon
AMSPRGC       = $(AMSWD)/exe/$(ARCH)/offmonc
SLC4=y
ifdef PGTRACK
PREFIX        = $(AMSWD)/bin/$(ARCH)/online/PG/
AMSLIB        = ntuple_slc4_PG
AMSPRG        = $(AMSWD)/exe/$(ARCH)/offmonPG
AMSPRGC       = $(AMSWD)/exe/$(ARCH)/offmoncPG
else
endif



# display compiles only with customized ROOT, at the moment v5.21
NEW         :=_new

VER          := $(shell $(ROOTSYS)/bin/root-config --version )


ifeq ($(ARCH),linuxicc)
CXX           =  /opt/intel/Compiler/11.1/073/bin/ia32/icc 
CPPFLAGS      =   -m32  -D__root_$(NEW) -D__WRITEROOT__ -D__ROOTSHAREDLIBRARY__ -I. \
		 -I../include -I$(ROOTSYS)/include -I/usr/include/freetype2   
CXXFLAGS      = -Wno-deprecated -openmp -axssse3 $(OPT) $(CPPFLAGS)
OPT           = -O3
OPTC          = -O3
NOOPT         = -g
NOOPTC        = -g
LD            = /opt/intel/Compiler/11.1/073/bin/ia32/icpc -m32 -axssse3 -openmp

LDFLAGS       = 
LDS	= 	$(LD) -Bstatic
LDD	= 	$(LD) -Bdynamic
ifdef PGTRACK
LFORT= -lifcore
else
LFORT = -L/afs/cern.ch/ams/Offline/AMSsoft/linux_slc5_icc32/2005/lib -lgeant321 -lpacklib -lmathlib -lkernlib  -L/opt/intel/Compiler/11.1/073/lib/ia32 -lifport -lifcoremt -limf  -lipgo -lirc_s -lguide 
endif

ROOTLIBS      =  $(shell $(ROOTSYS)/bin/root-config --glibs) -lASImage  -lNetx -lHistPainter -lMinuit -lGX11 -lGX11TTF $(LFORT)
ROOTLIBSA     =   -L$(ROOTSYS)/lib -lRoot -pthread  -lAfterImage $(ROOTSYS)/lib/libXrdClient.a -lpcre -lncurses \
	          -L/usr/X11R6/lib -lXft -lfontconfig -lXrender -lfreetype  -lexpat   \
                  -lXpm -lXmu -lXt -lXext -lX11 -ljpeg -lpng -ltiff   -lXrd -lXrdNet -lXrdNetUtil -lXrdOuc -lXrdSys $(LFORT)

LIBS          =  -Bstatic   $(ROOTLIBSA)  -Bdynamic -lshift -lm    -ldl
LIBSA         =   $(ROOTLIBSA) -Wl,-whole-archive,--allow-multiple-definition -lshift -Wl,-no-whole-archive  -lm   -Bdynamic          -ldl 

endif
ifeq ($(ARCH),linuxx8664icc)
CXX           =  /opt/intel/Compiler/11.1/073/bin/intel64/icc 
CPPFLAGS      =     -D__root_$(NEW) -D__WRITEROOT__ -D__ROOTSHAREDLIBRARY__ -I. \
		 -I../include -I$(ROOTSYS)/include -I/usr/include/freetype2   
CXXFLAGS      = -Wno-deprecated -openmp -axssse3 $(OPT) $(CPPFLAGS)
OPT           = -O3
OPTC          = -O3
NOOPT         = -g
NOOPTC        = -g
LD            = /opt/intel/Compiler/11.1/073/bin/intel64/icpc  -axssse3 -openmp -static-intel
ifdef PGTRACK
LDFLAGS       = 
LFORT= -lifcore
else
LDFLAGS = 
LFORT =  -L/afs/cern.ch/exp/ams/Offline/CERN/2005/lib -lgeant321 -lminuit -lpacklib -lmathlib -lkernlib  -L/opt/intel/Compiler/11.1/073/lib/intel64 -lifport -lifcoremt -limf  -lipgo -lirc_s -lguide 
endif
LDS	= 	$(LD) -Bstatic
LDD	= 	$(LD) -Bdynamic

ROOTLIBS      =  $(shell $(ROOTSYS)/bin/root-config --glibs) -lASImage  -lNetx -lHistPainter -lMinuit -lGX11 -lGX11TTF $(LFORT)
ROOTLIBSA     =   -L$(ROOTSYS)/lib -lRoot -pthread  -lAfterImage $(ROOTSYS)/lib/libXrdClient.a -lpcre \
	          -L/usr/X11R6/lib64 -lXft -lfontconfig -lXrender -lfreetype  -lexpat  -lncurses \
                  -lXpm -lXmu -lXt -lXext -lX11 -ljpeg -lpng -ltiff   -lXrd -lXrdNet -lXrdNetUtil -lXrdOuc -lXrdSys $(LFORT)

LIBS          =  -Bstatic   $(ROOTLIBSA)  -Bdynamic -lshift -lm    -ldl 
LIBSA         =   $(ROOTLIBSA) -Wl,-whole-archive,--allow-multiple-definition -lshift -Wl,-no-whole-archive  -lm   -Bdynamic          -ldl 

endif
ifeq ($(ARCH),linux)

CXX           =  g++ -m32 
CPPFLAGS      =   -D__root_$(NEW) -D__WRITEROOT__ -D__ROOTSHAREDLIBRARY__  -I. -I../include -I$(ROOTSYS)/include -I/usr/include/freetype2 
CXXFLAGS      = -Wno-deprecated $(OPT) $(CPPFLAGS)
OPT           = -g
OPTC          = -g
NOOPT         = -g
LD            = g++ -m32
LDFLAGS       = -g 
LDS	= 	$(LD) -static
LDS	= 	$(LD) -static
LDD	= 	$(LD) -rdynamic
ifdef PGTRACK
LFORT= -lgfortran
else
LFORT =  -L/afs/cern.ch/ams/Offline/AMSsoft/linux_slc5_gcc32/2005/lib -lgeant321  -lpacklib -lmathlib -lkernlib  -lgfortran
endif

ROOTLIBS      = $(shell $(ROOTSYS)/bin/root-config --glibs) -lASImage  -lNetx -lGX11 -lHistPainter -lMinuit  -lGX11TTF $(LFORT)
ROOTLIBSA      =  -L$(ROOTSYS)/lib -lRoot -pthread  -lAfterImage $(ROOTSYS)/lib/libXrdClient.a -lpcre -lncurses \
	          -L/usr/X11R6/lib -lXft -lfontconfig -lXrender -lfreetype  -lexpat   \
                  -lXpm -lXmu -lXt -lXext -lX11 -ljpeg -lpng -ltiff   -lXrd -lXrdNet  -lXrdNetUtil -lXrdOuc -lXrdSys $(LFORT)


LIBS          =  -Bstatic   $(ROOTLIBSA)  -Bdynamic -lshift -lm    -ldl
LIBSA         =   $(ROOTLIBSA) -Wl,-whole-archive,--allow-multiple-definition -lshift -Wl,-no-whole-archive  -lm   -Bdynamic          -ldl 
endif

ifeq ($(ARCH),linuxx8664gcc)

CXX           =  g++
CPPFLAGS      =   -D__root_$(NEW) -D__WRITEROOT__ -D__ROOTSHAREDLIBRARY__  -I. -I../include -I$(ROOTSYS)/include -I/usr/include/freetype2 
CXXFLAGS      = -Wno-deprecated $(OPT) $(CPPFLAGS)
OPT           = -O2
OPTC          = -O0
NOOPT         = -g
LD            = g++ 
LDFLAGS       = -g 
ifdef PGTRACK
LFORT= -lgfortran
else
LFORT =  -L/afs/cern.ch/exp/ams/Offline/CERN/2005.gcc64/lib -lgeant321 -lminuit -lpacklib -lmathlib -lkernlib  -lgfortran
endif
LDS	= 	$(LD) -static
LDS	= 	$(LD) -static
LDD	= 	$(LD) -rdynamic

ROOTLIBS      = $(shell $(ROOTSYS)/bin/root-config --glibs) -lASImage  -lNetx -lGX11 -lHistPainter -lMinuit  -lGX11TTF $(LFORT)
ROOTLIBSA      =  -L$(ROOTSYS)/lib -lRoot -pthread  -lAfterImage $(ROOTSYS)/lib/libXrdClient.a -lpcre -lncurses \
	          -L/usr/X11R6/lib64 -lXft -lfontconfig -lXrender -lfreetype  -lexpat   \
                  -lXpm -lXmu -lXt -lXext -lX11 -ljpeg -lpng -ltiff   -lXrd -lXrdNet  -lXrdNetUtil -lXrdOuc -lXrdSys $(LFORT)


LIBS          =  -Bstatic   $(ROOTLIBSA)  -Bdynamic -lshift -lm    -ldl
LIBSA         =   $(ROOTLIBSA) -Wl,-whole-archive,--allow-multiple-definition -lshift -Wl,-no-whole-archive  -lm   -Bdynamic          -ldl 
endif


#ifeq ($(ARCH),macosx)
#CXX           =  g++  -Wno-deprecated
#CXXFLAGS      =   -D__WRITEROOT__ -D__ROOTSHAREDLIBRARY__  -I../include -I$(ROOTSYS)/include -I/usr/include/freetype2 
#OPTC          = -O0
#NOOPT         = -g
#LD            = g++
#ROOTLIBS      =  -L$(ROOTSYS)/lib -lRoot -lfreetype -lcrypto -L/usr/X11R6/lib  -lXpm -lXmu -lXt -lXext -lX11  

#LIBS          =  $(ROOTLIBS) $(RH9add) -lm -ldl
#LIBSA          =  $(ROOTLIBS) $(RH9adda) -lm -ldl
#endif
#ifeq ($(ARCH),win32gcc)
#PREFIX        = $(AMSWD)/online/cygwin/
#CXX           =  g++ -Wno-deprecated
#CXXFLAGS      =   -D__WRITEROOT__ -D__ROOTSHAREDLIBRARY__  -I../include -I$(ROOTSYS)/include -I/usr/include/freetype2 
#OPT           = -O2
#OPTC          = -O0
#NOOPT         = -g
#NOOPTC         = -g
#LD            = g++
#LDFLAGS       = -g 
#ROOTLIBS      =  -L$(ROOTSYS)/lib -lRoot -lfreetype -lcrypt -lcrypto -L/usr/X11R6/lib  -lXpm.dll -lX11.dll  -L/usr/lib -lshift
#LIBS          =  $(ROOTLIBS) $(RH9add) -lm 
#LIBSA          =  $(ROOTLIBS) $(RH9adda) -lm
#endif
#ifeq ($(ARCH),win32)
#PREFIX   = $(AMSWD)/online/wingdk/
#CXX        =  $(ROOTSYS)/build/win/cxx.sh
#CXXFLAGS =   -D__WRITEROOT__ -D__ROOTSHAREDLIBRARY__ -O2 -Z7 -G5 -GR -GX -MD -DWIN32 -DGDK_WIN32  -D_WINDOWS -DWINVER=0x0400 -nologo -DCRTAPI1=_cdecl -DCRTAPI2=_cdecl -FIw32pragma.h -D_X86_=1 -D_DLL -DVISUAL_CPLUSPLUS -I../include  -I$(ROOTSYS)/include  /TP
#OPT           = 
#OPTC          = 
#NOOPT         = -g
#NOOPTC         = -g
#LD = $(ROOTSYS)/build/win/ld.sh
#LDFLAGS       =  -nologo    -nodefaultlib:libc.lib -nodefaultlib:dfor.lib 
#LIBS          =  $(ROOTSYS)/lib/libCore.lib $(ROOTSYS)/lib/libCint.lib $(ROOTSYS)/lib/libHist.lib $(ROOTSYS)/lib/libHistPainter.lib $(ROOTSYS)/lib/libGui.lib  $(ROOTSYS)/lib/libMatrix.lib $(ROOTSYS)/lib/libGeom.lib $(ROOTSYS)/lib/libPhysics.lib $(ROOTSYS)/lib/libGpad.lib $(ROOTSYS)/lib/libGraf.lib $(ROOTSYS)/lib/libGraf3d.lib $(ROOTSYS)/lib/libTree.lib  $(ROOTSYS)/lib/libRint.lib advapi32.lib
#ALIBS          = lib/libroot.a lib/libCore.lib lib/libfreetype.lib advapi32.lib
#endif

ifdef CASTOSTATIC
CPPFLAGS+= -DCASTORSTATIC
ROOTLIBS+=-lRFIO
endif
ifdef PGTRACK
PG := PG
CPPFLAGS += -D_PGTRACK_
else
PG :=
endif

##### MACROS #####



SRCS          = AMSDisplay.cxx     AMSHist.cxx       AMSAntiHist.cxx \
                AMSTrackerHist.cxx AMSRICHHist.cxx   AMSECALHist.cxx \
                AMSTRDHist.cxx     AMSTOFHist.cxx    AMSLVL1Hist.cxx \
                AMSLVL3Hist.cxx    AMSAxAMSHist.cxx  AMSGenHist.cxx \
                AMSNtuple.cxx      ControlFrame.cxx  AMSEverything$(PG).cxx

HDRS          = AMSDisplay.h       AMSHist.h         AMSAntiHist.h \
		AMSTrackerHist.h   AMSRICHHist.h     AMSECALHist.h \
                AMSTRDHist.h       AMSTOFHist.h      AMSLVL1Hist.h \
                AMSLVL3Hist.h      AMSAxAMSHist.h    AMSGenHist.h  \
                AMSNtuple.h        ControlFrame.h

DEPS=  $(SRCS:%.cxx=$(DEPDIR)/%.d)

DICT          = $(PREFIX)/AMSCint_$(ARCH)_$(SLC4)$(NEW).cxx
DICTH         = $(DICT:.cxx=.h)
DICTO         = $(DICT:$(PREFIX)/%.cxx=$(PREFIX)/%.o)


MAIN          = main.cxx
MAINH         = $(MAIN:.cxx=.h)
MAINO         = $(MAIN:%.cxx=$(PREFIX)%.o)
DEPS+=  $(MAIN:%.cxx=$(DEPDIR)/%.d)

MAINC          = mainc.cxx
MAINOC         = $(MAINC:.cxx=.o)
#DEPS+=  $(MAINC:%.cxx=$(DEPDIR)/%.d)

OBJS          = $(SRCS:%.cxx=$(PREFIX)%.o) 


ALLDICT       = $(DICT) $(DICTH) 
ALLSRCS       = $(SRCS  $(ALLDICT) $(MAIN) $(MAINH) $(MAINC)
ALLOBJS       = $(OBJS) $(MAINO) 





#ifeq ($(ARCH),win32)
#AMSPRG       = $(PREFIX)offmon.exe
#endif
LDS	= 	$(LD) -static
LDD	= 	$(LD) -rdynamic
#ifeq ($(ARCH),macosx)
#AMSPRG       = nothing
#LDS =  @echo "  Static Linking not supported in macosx!!! Use 'make osdepend' instead"
#LDD	= 	$(LD) -dynamic
#endif



##### TARGETS #####
standalone:  amslib $(AMSPRG) 
osdepend:  amslib $(AMSPRGC)

amslib: $(AMSWD)/lib/$(ARCH)/lib$(AMSLIB).a
$(AMSWD)/lib/$(ARCH)/lib$(AMSLIB).a:
	$(MAKE) -C ../install lib

  



$(AMSPRG):        $(OBJS)  $(DICTO) $(MAINO) $(AMSWD)/lib/$(ARCH)/lib$(AMSLIB).a
		$(LDS)   $(LDFLAGS) $^  $(LIBSA) $(AMSWD)/lib/$(ARCH)/lib$(AMSLIB).a -o $(@) 
		@echo "$(AMSPRG) done"

$(AMSPRGC):	  $(ALLOBJS) $(DICTO) $(OBJSR) $(OBJSC)
		$(LDD)  $(LDFLAGS) $(ALLOBJS) $(DICTO) $(OBJSR) $(OBJSC)   $(LIBS) $(AMSWD)/lib/$(ARCH)/lib$(AMSLIB).a  $(LIBS) -o $(AMSPRGC) 


$(DICT):        $(HDRS) LinkDef.h
		@echo "Generating dictionary ..."
		$(ROOTSYS)/bin/rootcint -f $(DICT) -c $(CPPFLAGS) $(ROOTH) $(HDRS) LinkDef.h





../include/root_methods$(PG).h:      ../perl/root.perl
	perl ../perl/root.perl

AMSEverything$(PG).cxx:      ../perl/online.perl ../include/root_methods$(PG).h
	perl ../perl/online.perl

dist:
		rm -f $(AMSPRG).tar.gz ; \
		tar cvf $(AMSPRG).tar $(ALLSRCS) 00index.txt Make* ; \
		gzip $(AMSPRG).tar

clean:
		@rm -vf $(ALLOBJS) $(ALLDICT) $(DEPS) $(AMSPRG) $(AMSPRGC) AMSEverything$(PG).cxx



##### RULES #####


$(DEPDIR)/%.d : %.cxx
	@echo "Generating Dep file $@ ..."
	@if ! [ -d $(DEPDIR) ] ; then mkdir -p $(DEPDIR); fi
	@$(CXX) $(CPPFLAGS) -MM $< -MT $(OBJ)/$*.o -MF $@


$(PREFIX)%.o : $(PREFIX)%.cxx 
	$(CXX) $(OPT) $(CXXFLAGS) -c $(<)  -o $(@)

$(PREFIX)%.o : %.cxx 
	$(CXX) $(OPT) $(CXXFLAGS) -c $(<)  -o $(@)




##### DEPENDENCIES #####

ifneq ($(MAKECMDGOALS),clean)
-include $(DEPS)
endif

