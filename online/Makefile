#  $Id: Makefile,v 1.36 2009/08/17 12:59:36 pzuccon Exp $
# PZ reviewing the online  Makefile on Aug 2009
#    WIN32 ans MACOSX arch commented as not supported in GBATCH for the moment
#    LINUX arch supported is SLC4 with gcc or  icc

ARCH         := $(shell $(ROOTSYS)/bin/root-config --arch)
PLATFORM      =  Linux
VER         := $(shell $(ROOTSYS)/bin/root-config --version )
PREFIX        = $(AMSWD)/online/
DEPDIR        = $(PREFIX)/dep

# display compiles only with customized ROOT, at the moment v5.21
NEW             :=_new



ifeq ($(ARCH),linuxicc)
CXX           =  /opt/intel/Compiler/11.0/069/bin/ia32/icc 
CPPFLAGS      =    -D__root_$(NEW) -D__WRITEROOT__ -D__ROOTSHAREDLIBRARY__ -I. \
		 -I../include -I$(ROOTSYS)/include -I/usr/include/freetype2   
CXXFLAGS      = -Wno-deprecated -openmp -axssse3 $(OPT) $(CPPFLAGS)
OPT           = -O3
OPTC          = -O3
NOOPT         = -g
NOOPTC        = -g
LD            = /opt/intel/Compiler/11.0/069/bin/ia32/icpc -axssse3 -openmp
LDFLAGS       = 
LDS	      =	$(LD) -Bstatic
LDD	      =	$(LD) -Bdynamic

ROOTLIBS      =  $(shell $(ROOTSYS)/bin/root-config --glibs) -lASImage -lRFIO -lNetx -lHistPainter -lMinuit -lGX11 -lGX11TTF
ROOTLIBSA     =   -L$(ROOTSYS)/lib -lRoot -pthread  -lAfterImage $(ROOTSYS)/lib/libXrdClient.a -lpcre \
	          -L/usr/X11R6/lib -lXft -lfontconfig -lXrender -lfreetype  -lexpat   \
                  -lXpm -lXmu -lXt -lXext -lX11 -ljpeg -lpng -ltiff   -lXrd -lXrdNet -lXrdOuc -lXrdSys 

LIBS          =  -L$(AMSWD)/lib/linux/ -lntuple_slc4_icc $(ROOTLIBS)  -lshift -lm    -ldl
LIBSA         =  -L$(AMSWD)/lib/linux/ -lntuple_slc4_icc $(ROOTLIBSA) -lshift -lm   -Bdynamic          -ldl 

else

CXX           =  g++ -m32 
CPPFLAGS      =   -D__root_$(NEW) -D__WRITEROOT__ -D__ROOTSHAREDLIBRARY__  -I../include -I$(ROOTSYS)/include -I/usr/include/freetype2 
CXXFLAGS      = -Wno-deprecated $(OPT) $(CPPFLAGS)
OPT           = -O2
OPTC          = -O0
NOOPT         = -g
LD            = g++ -m32
LDFLAGS       = -g 
LDS	      =	$(LD) -static
LDD	      =	$(LD) -rdynamic

ROOTLIBS      = $(shell $(ROOTSYS)/bin/root-config --glibs) -lASImage -lRFIO -lNetx -lGX11 -lHistPainter -lMinuit  -lGX11TTF
ROOTLIBSA     =  -L$(ROOTSYS)/lib -lRoot -pthread  -lAfterImage $(ROOTSYS)/lib/libXrdClient.a -lpcre \
	          -L/usr/X11R6/lib -lXft -lfontconfig -lXrender -lfreetype  -lexpat   \
                  -lXpm -lXmu -lXt -lXext -lX11 -ljpeg -lpng -ltiff   -lXrd -lXrdNet -lXrdOuc -lXrdSys 


LIBSA          =  -L$(AMSWD)/lib/linux/ -lntuple_slc4 $(ROOTLIBSA) -lshift -lm -ldl
LIBS          =   -L$(AMSWD)/lib/linux/ -lntuple_slc4  $(ROOTLIBS) -lshift -lm -ldl
endif


#ifeq ($(ARCH),macosx)
#CXX           =  g++ -Wno-deprecated
#CXXFLAGS      =   -D__WRITEROOT__ -D__ROOTSHAREDLIBRARY__  -I../include -I$(ROOTSYS)/include -I/usr/include/freetype2 
#OPT           = -O2
#OPTC          = -O0
#NOOPT         = -g
#LD            = g++
#LDFLAGS       = -g 
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
#LIBS          =  $(ROOTSYS)/lib/libCore.lib $(ROOTSYS)/lib/libCint.lib $(ROOTSYS)/lib/libHist.lib  $(ROOTSYS)/lib/libHistPainter.lib $(ROOTSYS)/lib/libGui.lib  $(ROOTSYS)/lib/libMatrix.lib $(ROOTSYS)/lib/libGeom.lib $(ROOTSYS)/lib/libPhysics.lib $(ROOTSYS)/lib/libGpad.lib $(ROOTSYS)/lib/libGraf.lib $(ROOTSYS)/lib/libGraf3d.lib $(ROOTSYS)/lib/libTree.lib  $(ROOTSYS)/lib/libRint.lib advapi32.lib
#ALIBS          = lib/libroot.a lib/libCore.lib lib/libfreetype.lib advapi32.lib
#endif


ifdef PGTRACK
CPPFLAGS += -D_PGTRACK_
endif

##### MACROS #####



SRCS          = AMSDisplay.cxx     AMSHist.cxx       AMSAntiHist.cxx \
                AMSTrackerHist.cxx AMSRICHHist.cxx   AMSECALHist.cxx \
                AMSTRDHist.cxx     AMSTOFHist.cxx    AMSLVL1Hist.cxx \
                AMSLVL3Hist.cxx    AMSAxAMSHist.cxx  AMSGenHist.cxx \
                AMSNtuple.cxx      ControlFrame.cxx  AMSEverything.cxx

HDRS          = AMSDisplay.h       AMSHist.h         AMSAntiHist.h \
		AMSTrackerHist.h   AMSRICHHist.h     AMSECALHist.h \
                AMSTRDHist.h       AMSTOFHist.h      AMSLVL1Hist.h \
                AMSLVL3Hist.h      AMSAxAMSHist.h    AMSGenHist.h  \
                AMSNtuple.h        ControlFrame.h

DEPS=  $(SRCS:%.cxx=$(DEPDIR)/%.d)

DICT          = AMSCint_$(ARCH)_$(SLC4)$(NEW).cxx
DICTH         = $(DICT:.cxx=.h)
DICTO         = $(DICT:%.cxx=$(PREFIX)%.o)


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


AMSLIB       = libAMS.a

AMSSHAREDLIB = libAMS.so


AMSPRG       = $(PREFIX)offmon
AMSPRGC       = $(PREFIX)offmonc
SLC4=y

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
standalone:  $(AMSPRG)

osdepend:  $(AMSPRGC)



$(AMSPRG):       $(OBJS)  $(DICTO) $(MAINO)
		$(LDS)    $(LDFLAGS) $^   $(LIBSA) -o $(@) 
		@echo "$(AMSPRG) done"

$(AMSPRGC):	$(ALLOBJS) $(DICTO) $(OBJSR) $(OBJSC)
		$(LDD)  $(LDFLAGS) $(ALLOBJS) $(DICTO) $(OBJSR) $(OBJSC)   $(LIBS) -o $(AMSPRGC) 


$(DICT):        $(HDRS) LinkDef.h
		@echo "Generating dictionary ..."
		$(ROOTSYS)/bin/rootcint -f $(DICT) -c $(CPPFLAGS) $(ROOTH) $(HDRS) LinkDef.h





../include/root_methods.h:      ../perl/root.perl
	perl ../perl/root.perl

AMSEverything.cxx:      ../perl/online.perl ../include/root_methods.h
	perl ../perl/online.perl

dist:
		rm -f $(AMSPRG).tar.gz ; \
		tar cvf $(AMSPRG).tar $(ALLSRCS) 00index.txt Make* ; \
		gzip $(AMSPRG).tar

clean:
		@rm -vf $(ALLOBJS) $(ALLDICT) $(DEPS) $(AMSPRG) $(AMSPRGC) AMSEverything.cxx



##### RULES #####


$(DEPDIR)/%.d : %.cxx
	@echo "Generating $@ ..."
	@if ! [ -d $(DEPDIR) ] ; then mkdir -p $(DEPDIR); fi
	@$(CXX) $(CPPFLAGS) -MM $< -MT $(OBJ)/$*.o -MF $@



$(PREFIX)%.o : %.cxx 
	$(CXX) $(OPT) $(CXXFLAGS) -c $(<)  -o $(@)




##### DEPENDENCIES #####

ifneq ($(MAKECMDGOALS),clean)
include $(DEPS)
endif
