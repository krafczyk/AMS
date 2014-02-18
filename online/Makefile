#  $Id: Makefile,v 1.73 2014/02/18 15:46:01 choutko Exp $
# PZ reviewing the online  Makefile on Aug 2009
#    WIN32 ans MACOSX arch commented as not supported in GBATCH for the moment
#    LINUX arch supported is SLC4 with gcc or  icc

#    WIN32 ans MACOSX arch commented as not supported in GBATCH for the moment
#    LINUX arch supported is SLC4 with gcc or  icc
#
# List of SLC6 changes
#
ifeq ($(wildcard $(ROOTSYS)/include/Compression.h),)
    LZMAL =
else
    DEFINES  +=  -D__LZMA__
    LZMAL = -llzma
endif
ifndef SLC6system
SLC6system=
ifneq "$(wildcard /etc/redhat-release)" ""
 SLC6system := $(shell cat /etc/redhat-release | grep "Scientific Linux CERN SLC release 6")
endif
endif
#
#[ae-2012.08.17] - can't take from ../install/Makefile
#
ifndef AMSDEPLIB
  AMSDEPLIB := /afs/cern.ch/ams/local2/opt/
endif
ifdef SLC6system
AMSLIBSUFFIX=slc6
else
AMSLIBSUFFIX=slc4
endif
#
#[ae-2012.08.02] - some static libs we need take from slc5, initially we have there only those ones: 
#    libcrypt.a  libexpat.a  libfontconfig.a  libshift.a     
#
IMPORTS_SLC5_LIBS=
ifdef SLC6system
  IMPORTS_SLC5_LIBS= -L$(AMSDEPLIB)/lib64
endif
#
#[ae-2012.08.02] - change local /usr/X11R6 to afs location
ifndef 
  IMPORTS_X11R6= $(AMSDEPLIB)/X11R6
endif
#


ifndef AMSWD
AMSWD := ..
endif

XRDLIBP        := 
ARCH         := $(shell $(ROOTSYS)/bin/root-config --arch )
VERSION      := $(shell $(ROOTSYS)/bin/root-config --version | cut -b1-4)
PLATFORM      =  Linux

ifeq ($(VERSION),5.27)
VERSIONP=
else
VERSIONP=$(VERSION)
endif


PREFIX        = $(AMSWD)/bin/$(ARCH)$(VERSIONP)/online/
DEPDIR        = $(PREFIX)/dep

AMSLIB        = ntuple_$(AMSLIBSUFFIX)
AMSPRG        = $(AMSWD)/exe/$(ARCH)$(VERSIONP)/offmon
AMSPRGC       = $(AMSWD)/exe/$(ARCH)$(VERSIONP)/offmonc
SLC4=y
ifdef PGTRACK
PREFIX        = $(AMSWD)/bin/$(ARCH)$(VERSIONP)/online/PG/
AMSLIB        = ntuple_$(AMSLIBSUFFIX)_PG
AMSPRG        = $(AMSWD)/exe/$(ARCH)$(VERSIONP)/offmonPG
AMSPRGC       = $(AMSWD)/exe/$(ARCH)$(VERSIONP)/offmoncPG
else
endif
ifeq ($(VERSION),5.27)
ifndef SLC6system
  XRDLIB = -lXrdClient -lXrd -lXrdNet -lXrdOuc -lXrdSys -lXrdNetUtil 
else
  XRDLIB = -lXrdClient -lXrd -lXrdNet -lXrdOuc -lXrdSys 
endif
endif
ifeq ($(VERSION),5.34)
  XRDLIB = -lXrdClient   -lXrdUtils
endif


# display compiles only with customized ROOT, at the moment v5.21
NEW         :=_new

VER          := $(shell $(ROOTSYS)/bin/root-config --version )


ifeq ($(ARCH),linuxicc)

CXX           =  $(AMSDEPLIB)/intel/Compiler/11.1/073/bin/ia32/icc 
CPPFLAGS      =   -m32  -D__root_$(NEW) -D__WRITEROOT__ -D__ROOTSHAREDLIBRARY__ -I. \
		 -I../include -I$(ROOTSYS)/include -I/usr/include/freetype2   
CXXFLAGS      = -Wno-deprecated -openmp -axssse3 $(OPT) $(CPPFLAGS)
OPT           = -O3
OPTC          = -O3
NOOPT         = -g
NOOPTC        = -g
LD            = $(AMSDEPLIB)/intel/Compiler/11.1/073/bin/ia32/icpc -m32 -axssse3 -openmp

LDFLAGS       = 
LDS	= 	$(LD) -Bstatic
LDD	= 	$(LD) -Bdynamic
ifdef PGTRACK
LFORT= -lifcore
else
LFORT = -L/afs/cern.ch/ams/Offline/AMSsoft/linux_slc5_icc32/2005/lib -lgeant321 -lpacklib -lmathlib -lkernlib  -L$(AMSDEPLIB)/intel/Compiler/11.1/073/lib/ia32 -lifport -lifcoremt -limf  -lipgo -lirc_s -lguide 
endif

ROOTLIBS      =  $(shell $(ROOTSYS)/bin/root-config --glibs) -lASImage  -lNetx -lHistPainter -lMinuit -lGX11 -lGX11TTF $(LFORT)
ROOTLIBSA     =  -L$(ROOTSYS)/lib -lRoot -pthread  -lAfterImage $(ROOTSYS)/lib/libXrdClient.a -lpcre -lncurses \
	             -L$(IMPORTS_X11R6)/lib -lXft -lfontconfig -lXrender -lfreetype  -lexpat   \
                 -lXpm -lXmu -lXt -lXext -lX11 -ljpeg -lpng -ltiff   -lXrd -lXrdNet -lXrdNetUtil -lXrdOuc -lXrdSys $(LFORT)

LIBS          =  -Bstatic   $(ROOTLIBSA)  -Bdynamic -lshift -lm    -ldl
LIBSA         =  $(ROOTLIBSA) -Wl,-whole-archive,--allow-multiple-definition -lshift -Wl,-no-whole-archive  -lm -Bdynamic -ldl 
ifdef AMS_ACQT_INTERFACE
LIBS+= $(QTDIR_STATIC_32)/lib/libQtCore.a
LIBSA+= $(QTDIR_STATIC_32)/lib/libQtCore.a
endif

endif
ifeq ($(ARCH),linuxx8664icc)


ifeq ($(VERSION),5.34)
ifndef SLC6system
  XRDLIBP += -Bdynamic -L$(AMSDEPLIB)/xrootd-icc64-11/lib64
else
  XRDLIBP += -Bdynamic -L$(AMSDEPLIB)/xrootd-icc64-12/lib64
endif
endif

ifndef SLC6system
  INTELVER=/opt/intel/Compiler/11.1/073/
   CFLAGS=-axssse4.1,ssse3
else
  INTELVER=$(AMSDEPLIB)/intel/composer_xe_2011_sp1.9.293/
  INTELVER=$(AMSDEPLIB)/intel/composer_xe_2013_sp1.1.106/
  CFLAGS=-axAVX -msse3
endif
CXX           =  $(INTELVER)/bin/intel64/icc
CPPFLAGS      =  -D__root_$(NEW) -D__WRITEROOT__ -D__ROOTSHAREDLIBRARY__ -I. -I../include -I$(ROOTSYS)/include -I/usr/include/freetype2
CXXFLAGS      = -Wno-deprecated -openmp $(CFLAGS) $(OPT) $(CPPFLAGS)
ifdef SLC6system
  CPPFLAGS += -D__LINUXNEW__
endif
OPT           = -O3
OPTC          = -O3
NOOPT         = -g
NOOPTC        = -g
LD            = $(INTELVER)/bin/intel64/icpc  $(CFLAGS) -openmp -static-intel -static-libgcc
ifdef PGTRACK
  LDFLAGS       = 
  LFORT= -lifcore
else
  LDFLAGS = 
  CERNDIRLIB = /afs/cern.ch/exp/ams/Offline/CERN/2005/lib
  LFORT =  -L$(CERNDIRLIB) -lgeant321  -lminuit -lpacklib -lmathlib -lkernlib  -L$(INTELVER)/lib/intel64 -lifport -lifcoremt -limf  -lipgo -lirc_s
  ifndef SLC6system
    LFORT += -lguide
  endif
endif
LDS	= 	$(LD) -Bstatic
LDD	= 	$(LD) -Bdynamic

ROOTLIBS      = $(shell $(ROOTSYS)/bin/root-config --glibs) -lASImage  -lNetx -lHistPainter -lMinuit -lGX11 -lGX11TTF $(LFORT)
ROOTLIBSA     = -L$(ROOTSYS)/lib -lRoot $(LZMAL) -pthread  -lAfterImage  -lpcre -lcrypto \
	            $(IMPORTS_SLC5_LIBS) -L$(IMPORTS_X11R6)/lib64 -lXft -lfontconfig -lXrender -lfreetype  -lexpat  -lncurses \
                -lXpm -lXmu -lXt -lXext -lX11 -ljpeg -lpng -ltiff   
ROOTLIBSA +=  $(LFORT)

LIBS          =  -Bstatic   $(ROOTLIBSA)  $(XRDLIBP) $(XRDLIB) -Bdynamic -lshift -lm    -ldl 
LIBSA         =   $(ROOTLIBSA) -Wl,-whole-archive,--allow-multiple-definition -lshift -Wl,-no-whole-archive  $(XRDLIBP) $(XRDLIB) -lm -Bstatic -ldl 

endif
ifeq ($(ARCH),linux)

CXX           =  g++ -m32 
CPPFLAGS      =  -D__root_$(NEW) -D__WRITEROOT__ -D__ROOTSHAREDLIBRARY__  -I. -I../include -I$(ROOTSYS)/include -I/usr/include/freetype2 
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
ROOTLIBSA     =  -L$(ROOTSYS)/lib -lRoot -pthread  -lAfterImage $(ROOTSYS)/lib/libXrdClient.a -lpcre -lncurses \
	             -L$(IMPORTS_X11R6)/lib -lXft -lfontconfig -lXrender -lfreetype  -lexpat   \
                 -lXpm -lXmu -lXt -lXext -lX11 -ljpeg -lpng -ltiff   -lXrd -lXrdNet  -lXrdNetUtil -lXrdOuc -lXrdSys $(LFORT)


LIBS          =  -Bstatic   $(ROOTLIBSA)  -Bdynamic -lshift -lm    -ldl
LIBSA         =   $(ROOTLIBSA) -Wl,-whole-archive,--allow-multiple-definition -lshift -Wl,-no-whole-archive  -lm   -Bdynamic          -ldl 
ifdef AMS_ACQT_INTERFACE
LIBS+= $(QTDIR_STATIC_32)/lib/libQtCore.a
LIBSA+= $(QTDIR_STATIC_32)/lib/libQtCore.a
endif

endif
ifeq ($(ARCH),linuxx8664gcc)
ifeq ($(VERSION),5.34)
ifndef SLC6system
  XRDLIBP += -Bdynamic -L$(AMSDEPLIB)/xrootd-gcc64-41/lib64
else
  XRDLIBP += -Bdynamic -L$(AMSDEPLIB)/xrootd-gcc64-44/lib64
endif
endif

CXX           =  g++
CPPFLAGS      =  -D__root_$(NEW) -D__WRITEROOT__ -D__ROOTSHAREDLIBRARY__  -I. -I../include -I$(ROOTSYS)/include -I/usr/include/freetype2 
ifdef SLC6system
  CPPFLAGS += -D__LINUXNEW__
endif
CXXFLAGS      = -Wno-deprecated $(OPT) $(CPPFLAGS)
OPT           = -O2
OPTC          = -O0
NOOPT         = -g
LD            = g++ 
LDFLAGS       = -g 
ifdef PGTRACK
LFORT= -lgfortran
else
  ifndef SLC6system
    CERNDIRLIB = /afs/cern.ch/exp/ams/Offline/CERN/2005.gcc64/lib
  else  
    CERNDIRLIB = /afs/cern.ch/exp/ams/Offline/CERN/2005.gcc64.44/lib
  endif  
  LFORT = -L$(CERNDIRLIB) -lgeant321 -lpacklib -lmathlib -lkernlib  -lgfortran
endif
LDS	= 	$(LD) -static
LDS	= 	$(LD) -static
LDD	= 	$(LD) -rdynamic

ROOTLIBS      = $(shell $(ROOTSYS)/bin/root-config --glibs) -lASImage  -lNetx -lGX11 -lHistPainter -lMinuit  -lGX11TTF $(LFORT)
ROOTLIBSA     = -L$(ROOTSYS)/lib -lRoot $(LZMAL) -pthread  -lAfterImage  -lpcre -lcrypto  -lncurses \
	            $(IMPORTS_SLC5_LIBS) -L$(IMPORTS_X11R6)/lib64 -lXft -lfontconfig -lXrender -lfreetype  -lexpat   \
                -lXpm -lXmu -lXt -lXext -lX11 -ljpeg -lpng -ltiff   $(XRDLIB) $(XRDLIBP) $(LFORT)

LIBS          = -Bstatic   $(ROOTLIBSA)   $(XRDLIBP) $(XRDLIB) -Bdynamic -lshift -lm    -ldl
LIBSA         = $(ROOTLIBSA) -Wl,-whole-archive,--allow-multiple-definition -lshift -Wl,-no-whole-archive  -lm   $(XRDLIBP) $(XRDLIB)  -Bdynamic          -ldl 
ifdef AMS_ACQT_INTERFACE
LIBS+= $(QTDIR_STATIC)/lib/libQtCore.a
LIBSA+= $(QTDIR_STATIC)/lib/libQtCore.a
LIBSD+= -L$(QTDIR)/lib -lQtCore
endif

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


ifdef AMS_ACQT_INTERFACE
LIBSD+= -L$(QTDIR)/lib -lQtCore
endif

ifdef AMS_ACQT_INTERFACE
LIBS+=-lrt
LIBSA+=-lrt
endif

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


ALLDICT       = $(DICT) $(DICTH) 8
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

amslib: $(AMSWD)/lib/$(ARCH)$(VERSIONP)/lib$(AMSLIB).a
$(AMSWD)/lib/$(ARCH)$(VERSIONP)/lib$(AMSLIB).a:
	$(MAKE) -C ../install lib

  



$(AMSPRG):        $(OBJS)  $(DICTO) $(MAINO) $(AMSWD)/lib/$(ARCH)$(VERSIONP)/lib$(AMSLIB).a
		$(LDS)   $(LDFLAGS) $^  $(LIBSA) $(AMSWD)/lib/$(ARCH)$(VERSIONP)/lib$(AMSLIB).a -o $(@) 
		@echo "$(AMSPRG) done"

$(AMSPRGC):	  $(ALLOBJS) $(DICTO) $(OBJSR) $(OBJSC)
		$(LDD)  $(LDFLAGS) $(ALLOBJS) $(DICTO) $(OBJSR) $(OBJSC)   $(LIBS) $(AMSWD)/lib/$(ARCH)$(VERSIONP)/lib$(AMSLIB).a  $(LIBS) -o $(AMSPRGC) 


$(DICT):        $(HDRS) LinkDef.h
		@echo "Generating dictionary ..."
		$(ROOTSYS)/bin/rootcint -f $(DICT) -c $(CPPFLAGS) $(ROOTH) $(HDRS) LinkDef.h





../include/root_methods$(P$(IMPORTS_SLC5_LIBS) G).h:      ../perl/root.perl
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

