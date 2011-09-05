#  $Id: Makefile,v 1.93 2011/09/05 14:25:49 mduranti Exp $
# PZ reviewing the display Makefile on Aug 2009
#    WIN32 ans MACOSX arch commented as not supported in GBATCH for the moment
#    LINUX arch supported is SLC4 with gcc or  icc

ifndef AMSWD
AMSWD := ..
endif


ARCH         := $(shell $(ROOTSYS)/bin/root-config --arch )
PLATFORM      =  Linux
PREFIX        = $(AMSWD)/bin/$(ARCH)/display/
DEPDIR        = $(PREFIX)/dep

AMSLIB        = ntuple_slc4
AMSLIBSO        = ntuple_slc4.so
AMSPRG        = $(AMSWD)/exe/$(ARCH)/amsed
AMSPRGC       = $(AMSWD)/exe/$(ARCH)/amsedc
SLC4=y

GCC_VER_MAJOR=$(shell gcc --version | grep gcc | cut -b11 )
GCC_VER_MIDDLE=$(shell gcc --version | grep gcc |cut -b13 )
GCC_VER_MINOR=$(shell gcc --version | grep gcc |cut -b15 )

ifeq ($(GCC_VER_MAJOR),4) 
 VERGCC:=1

ifeq ($(GCC_VER_MIDDLE),4) 
VERGCCH      := 1
endif
endif 


ifdef PGTRACK
PREFIX        = $(AMSWD)/bin/$(ARCH)/display/PG/
AMSLIB        = ntuple_slc4_PG
AMSPRG        = $(AMSWD)/exe/$(ARCH)/amsedPG
AMSPRGC       = $(AMSWD)/exe/$(ARCH)/amsedcPG
AMSPRGCC       = $(AMSWD)/exe/$(ARCH)/amseddPG
endif


CASTORLIB= -lRCastor
ifdef NOCASTOR
CPPFLAGS+= -D__NOCASTOR__
CASTORLIB=
endif

# display compiles only with customized ROOT, at the moment v5.21
NEW         :=_new

VER          := $(shell $(ROOTSYS)/bin/root-config --version ) 

ifeq ($(ARCH),linuxicc)
CXX           =  /opt/intel/Compiler/11.1/073/bin/ia32/icc 
CPPFLAGS      =    -m32 -D__LINUXGNU__ -D__root_$(NEW) -D__WRITEROOT__ -D__ROOTSHAREDLIBRARY__ -I. \
		 -I../include -I$(ROOTSYS)/include -I/usr/include/freetype2   
CXXFLAGS      = -Wno-deprecated -openmp -axssse3 $(OPT) $(CPPFLAGS)
OPT           = -O3
OPTC          = -O3
NOOPT         = -g
NOOPTC        = -g
LD            = /opt/intel/Compiler/11.1/073/bin/ia32/icpc -axsse4.2,ssse3 -openmp -static-intel
LDFLAGS       = 
LDS	= 	$(LD) -static 
LDD	= 	$(LD) -rdynamic 

ROOTLIBS      =  $(shell $(ROOTSYS)/bin/root-config --glibs) -lMinuit -lASImage -lRFIO -lTMVA -lNetx -lGX11 -lGX11TTF $(CASTORLIB)
ROOTLIBSA     =   -L$(ROOTSYS)/lib -lRoot -pthread  -lAfterImage $(ROOTSYS)/lib/libXrdClient.a -lpcre \
	          -L/usr/X11R6/lib -lXft -lfontconfig -lXrender -lfreetype  -lexpat   \
                  -lXpm -lXmu -lXt -lXext -lX11 -ljpeg -lpng -ltiff   -lXrd -lXrdNet -lXrdOuc -lXrdSys 

LIBS          =  -Bstatic -L$(AMSWD)/lib/linuxicc/ -l$(AMSLIB) -Bstatic $(ROOTLIBSA) -Wl,-whole-archive,--allow-multiple-definition -lshift -Wl,-no-whole-archive  -lm   -Bdynamic -ldl 
LIBSA         =  -L$(AMSWD)/lib/linuxicc/ -l$(AMSLIB) $(ROOTLIBSA) -Wl,-whole-archive,--allow-multiple-definition -lshift -Wl,-no-whole-archive -lm             -ldl 
LIBSD         =  -L$(AMSWD)/lib/linuxicc/ -l$(AMSLIB) $(ROOTLIBS) 

endif
ifeq ($(ARCH),linuxx8664icc)
CXX           =  /opt/intel/Compiler/11.1/073/bin/intel64/icc 
CPPFLAGS      =    -D__LINUXGNU__ -D__root_$(NEW) -D__WRITEROOT__ -D__ROOTSHAREDLIBRARY__ -I. \
		 -I../include -I$(ROOTSYS)/include -I/usr/include/freetype2   
CXXFLAGS      = -Wno-deprecated -openmp -axsse4.2,ssse3 $(OPT) $(CPPFLAGS)
OPT           = -O3 
OPTC          = -O3
NOOPT         = -g
NOOPTC        = -g
LD            = /opt/intel/Compiler/11.1/073/bin/intel64/icpc -axsse4.2,ssse3 -openmp -static-intel
LDFLAGS       = 
LDS	= 	$(LD) -static 
LDD	= 	$(LD) -rdynamic 

ROOTLIBS      =  $(shell $(ROOTSYS)/bin/root-config --glibs) -lMinuit -lASImage -lRFIO -lTMVA -lNetx -lGX11 -lGX11TTF $(CASTORLIB)
ROOTLIBSA     =   -L$(ROOTSYS)/lib -lRoot -pthread  -lAfterImage $(ROOTSYS)/lib/libXrdClient.a -lpcre \
	          -L/usr/X11R6/lib64 -lXft -lfontconfig -lXrender -lfreetype  -lexpat   \
                  -lXpm -lXmu -lXt -lXext -lX11 -ljpeg -lpng -ltiff   -lXrd -lXrdNet -lXrdOuc -lXrdSys -lXrdNetUtil -lncurses

LIBS          =   -Bstatic $(ROOTLIBSA)  -Wl,-whole-archive,--allow-multiple-definition -lshift -Wl,-no-whole-archive -lm   -Bdynamic -ldl 
LIBSA         =   $(ROOTLIBSA) -Wl,-whole-archive,--allow-multiple-definition -lshift -Wl,-no-whole-archive -lm             -ldl 
LIBSD         =  -L$(AMSWD)/lib/linuxx8664icc/ -l$(AMSLIB) $(ROOTLIBS) 

endif
ifeq ($(ARCH),macosx64)


CXX           =  g++  
CPPFLAGS      =  -D__APPLE__ -D__WRITEROOT__ -D__ROOTSHAREDLIBRARY__  -I. -I../include -I$(ROOTSYS)/include  -I/usr/X11R6/include/ -I/usr/X11R6/include/freetype2
CXXFLAGS      = -Wno-deprecated $(OPT) $(CPPFLAGS)
OPT           = -O3
NOOPT         = -g

LD            = g++ 
LDFLAGS       = -g 
LDS	= 	$(LD) 
LDS	= 	$(LD) 
LDD	= 	$(LD) 


ROOTLIBSA       = $(shell $(ROOTSYS)/bin/root-config --glibs) -lMinuit -lNetx -lRFIO -lTMVA
ROOTLIBS       = $(shell $(ROOTSYS)/bin/root-config --glibs) -lMinuit -lNetx -lRFIO -lTMVA
LIBS          =  -L$(AMSWD)/lib/macosx64/  -l$(AMSLIB) $(ROOTLIBS) 
LIBSA         =  -L$(AMSWD)/lib/macosx64/ -l$(AMSLIB) $(ROOTLIBSA)
LIBSD         =  -L$(AMSWD)/lib/macosx64/ -l$(AMSLIB) $(ROOTLIBS)
endif

ifeq ($(ARCH),linux)

CXX           =  g++ -m32 
CPPFLAGS      =  -D__LINUXGNU__  -D__WRITEROOT__ -D__ROOTSHAREDLIBRARY__  -I. -I../include -I$(ROOTSYS)/include -I/usr/include/freetype2 
CXXFLAGS      = -Wno-deprecated $(OPT) $(CPPFLAGS)
OPT           = -g
NOOPT         = -g

LD            = g++ -m32
LDFLAGS       = -g 
LDS	= 	$(LD) -static
LD            = g++ -m32
LDFLAGS       = -g 
LDS	= 	$(LD) -static
LDD	= 	$(LD) -rdynamic


ROOTLIBS       = $(shell $(ROOTSYS)/bin/root-config --glibs) -lMinuit -lASImage -lNetx -lRFIO -lTMVA -lGX11 -lGX11TTF $(CASTORLIB)
ROOTLIBSA      =  -L$(ROOTSYS)/lib -lRoot -pthread  -lAfterImage $(ROOTSYS)/lib/libXrdClient.a -lpcre \
	          -L/usr/X11R6/lib -lXft -lfontconfig -lXrender -lfreetype  /usr/lib/libexpat.a   \
                  -lXpm -lXmu -lXt -lXext -lX11 -ljpeg -lpng -ltiff   -lXrd -lXrdNet -lXrdOuc -lXrdSys 
LIBS          =   -Bstatic $(ROOTLIBSA)   -Wl,-whole-archive,--allow-multiple-definition -lshift -Wl,-no-whole-archive -lm -Bdynamic -ldl
LIBSA         =   $(ROOTLIBSA) -Wl,-whole-archive,--allow-multiple-definition -lshift -Wl,-no-whole-archive -lm    -ldl 
LIBSD          =   -L$(AMSWD)/lib/linux/ -l$(AMSLIB) $(ROOTLIBS)  

ifdef VERGCCH
CPPFLAGS+= -D__LINUXNEW__
endif
endif

ifeq ($(ARCH),linuxx8664gcc)

CXX           =  g++ 
CPPFLAGS      =  -D__LINUXGNU__  -D__WRITEROOT__ -D__ROOTSHAREDLIBRARY__  -I. -I../include -I$(ROOTSYS)/include -I/usr/include/freetype2 
CXXFLAGS      = -Wno-deprecated $(OPT) $(CPPFLAGS)
OPT           = -g
NOOPT         = -g

LD            = g++ 
LDFLAGS       = -g 
LDS	= 	$(LD) -static
LD            = g++ 
LDFLAGS       = -g 
LDS	= 	$(LD) -static
LDD	= 	$(LD) -rdynamic


ROOTLIBS       = $(shell $(ROOTSYS)/bin/root-config --glibs) -lMinuit -lASImage -lRFIO -lTMVA -lNetx -lGX11 -lGX11TTF $(CASTORLIB)
ROOTLIBSA      =  -L$(ROOTSYS)/lib -lRoot -pthread  -lAfterImage $(ROOTSYS)/lib/libXrdClient.a -lpcre -lncurses \
	          -L/usr/X11R6/lib64 -lXft -lfontconfig -lXrender -lfreetype  /usr/lib64/libexpat.a   \
                  -lXpm -lXmu -lXt -lXext -lX11 -ljpeg -lpng -ltiff   -lXrd -lXrdNet -lXrdOuc -lXrdSys -lXrdNetUtil 
LIBS          =    -Bstatic $(ROOTLIBSA)  -lm -Wl,-whole-archive,--allow-multiple-definition -lshift -Wl,-no-whole-archive -ldl

LIBSA         =   $(ROOTLIBSA) -Wl,-whole-archive,--allow-multiple-definition -lshift -Wl,-no-whole-archive -lm    -ldl 
LIBSD          =   -L$(AMSWD)/lib/linuxx8664gcc/ -l$(AMSLIB) $(ROOTLIBS) 

ifdef VERGCCH
CPPFLAGS+= -D__LINUXNEW__
endif
endif




ifdef PGTRACK
CPPFLAGS+= -D_PGTRACK_
endif

ifdef NOCASTOR
CPPFLAGS+= -D__NOCASTOR__
endif

# Other ARCH are not supported for the moment !!!!!!!!!!

#ifeq ($(ARCH),macosx)
#CXX           =  g++  -Wno-deprecated
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
#PREFIX        = $(AMSWD)/display/cygwin/
#CXX           =  g++ -Wno-deprecated
#CXXFLAGS      =   -D__WRITEROOT__ -D__ROOTSHAREDLIBRARY__  -I../include -I$(ROOTSYS)/include -I/usr/include/freetype2 
#OPT           = -O2
#OPTC          = -O0
#NOOPT         = -g
#NOOPTC         = -g
#LD            = g++
#LDFLAGS       = -g 
#ROOTLIBS      =  -L$(ROOTSYS)/lib -lRoot -lfreetype -lcrypt -L/usr/X11R6/lib  -lXpm.dll -lX11.dll $(RH9add) -L/usr/lib -lshift
#ROOTLIBSA      =  -L$(ROOTSYS)/lib -lRoot -lfreetype -lcrypt -L/usr/X11R6/lib  -lXpm.dll -lX11.dll $(RH9adda) -L/usr/lib -lshift
#LIBS          =  $(ROOTLIBS) -lm 
#LIBSA          =  $(ROOTLIBSA) -lm 
#endif
#ifeq ($(ARCH),win32)
#PREFIX   = $(AMSWD)/display/wingdk/
#CXX        =  $(ROOTSYS)/build/win/cxx.sh
#CXXFLAGS =   -D__WRITEROOT__ -D__ROOTSHAREDLIBRARY__ -O2 -Z7 -G5 -GR -GX -MD -DWIN32 -DGDK_WIN32  -D_WINDOWS -DWINVER=0x0400 -nologo -DCRTAPI1=_cdecl -DCRTAPI2=_cdecl -FIw32pragma.h -D_X86_=1 -D_DLL -DVISUAL_CPLUSPLUS -I../include  -I$(ROOTSYS)/include  /TP
#OPT           = 
#OPTC          = 
#NOOPT         = -g
#NOOPTC         = -g
#LD = $(ROOTSYS)/build/win/ld.sh
#LDFLAGS       =  -nologo -incremental:yes   -nodefaultlib:libc.lib -nodefaultlib:dfor.lib 
#LIBS          =  $(ROOTSYS)/lib/libCore.lib $(ROOTSYS)/lib/libCint.lib $(ROOTSYS)/lib/libHist.lib $(ROOTSYS)/lib/libHistPainter.lib $(ROOTSYS)/lib/libGui.lib  $(ROOTSYS)/lib/libMatrix.lib $(ROOTSYS)/lib/libGeom.lib $(ROOTSYS)/lib/libPhysics.lib $(ROOTSYS)/lib/libGpad.lib $(ROOTSYS)/lib/libGraf.lib $(ROOTSYS)/lib/libGraf3d.lib $(ROOTSYS)/lib/libTree.lib  $(ROOTSYS)/lib/libRint.lib advapi32.lib
#ALIBS          = lib/libroot.a lib/libCore.lib lib/libfreetype.lib advapi32.lib
#endif




##### MACROS #####


SRCS           = main.cxx TGRunEventDialog.cxx ControlFrame.cxx \
                 AMSNtupleV.cxx   TView3DAMS.cxx    Help.cxx

SRCSN          = AMSR_GeometrySetter.cxx  AMSDisplay$(NEW).cxx \
                 TMarker3DCl$(NEW).cxx    AMSR_Axis.cxx 

HDRS          = TGRunEventDialog.h    ControlFrame.h \
		AMSNtupleV.h         TView3DAMS.h     \
                AMSR_GeometrySetter.h AMSDisplay.h   \
                TMarker3DCl.h          AMSR_Axis.h 

DEPS=  $(SRCS:%.cxx=$(DEPDIR)/%.d)
DEPS+= $(SRCSN:%.cxx=$(DEPDIR)/%.d)


DICT          = $(PREFIX)/AMSCint_$(ARCH)_$(SLC4)$(NEW).cxx
DICTH         = $(DICT:.cxx=.h)
DICTO         = $(DICT:$(PREFIX)/%.cxx=$(PREFIX)/%.o)




OBJS          = $(SRCS:%.cxx=$(PREFIX)%.o) 
OBJSN         = $(SRCSN:%.cxx=$(PREFIX)%.o) 

ALLDICT       = $(DICT) $(DICTH) 
ALLSRCS       = $(SRCS) $(SRCSN) $(DICT)  
ALLOBJS       = $(OBJS) $(OBJSN) $(DICTO)





#ifeq ($(ARCH),win32)
#AMSPRG       = $(PREFIX)amsed.exe
#endif
#ifeq ($(ARCH),macosx)
#AMSPRG       = nothing
#LDS =  @echo "  Static Linking not supported in macosx!!! Use 'make osdepend' instead"
#LDD	= 	$(LD) -dynamic
#endif



##### TARGETS #####
standalone:  amslib $(AMSPRG) 
osdepend:  $(AMSPRGC)
dynamic:   amslib $(AMSPRGCC)

amslib: $(AMSWD)/lib/$(ARCH)/lib$(AMSLIB).a
$(AMSWD)/lib/$(ARCH)/lib$(AMSLIB).a:
	$(MAKE) -C ../install lib

  



$(AMSPRG):    $(OBJSN)  $(OBJS) $(DICTO) $(AMSWD)/lib/$(ARCH)/lib$(AMSLIB).a 
		@if ! [ -d $(AMSWD)/exe/$(ARCH) ] ; then mkdir -p $(AMSWD)/exe/$(ARCH); fi
		$(LDS)   $(LDFLAGS) $^  $(LIBSA)  -o $(@) 
		@echo "$(AMSPRG) done"

$(AMSPRGC):	$(OBJSN)  $(OBJS) $(DICTO)  $(AMSWD)/lib/$(ARCH)/lib$(AMSLIB).a 
		@if ! [ -d $(AMSWD)/exe/$(ARCH) ] ; then mkdir -p $(AMSWD)/exe/$(ARCH); fi
		$(LDD)   $(LDFLAGS) $^  $(LIBS) -o $(@) 
		@echo "$(AMSPRGC) done"

$(AMSPRGCC):	$(OBJSN)  $(OBJS) $(DICTO)  
		@if ! [ -d $(AMSWD)/exe/$(ARCH) ] ; then mkdir -p $(AMSWD)/exe/$(ARCH); fi
		$(LDD)   $(LDFLAGS) $^  $(LIBSD) -o $(@) 
		@echo "$(AMSPRGCC) done"



$(DICT):        $(HDRS) LinkDef.h
		@echo "Generating dictionary ..."
		$(ROOTSYS)/bin/rootcint -f $@ -c $(CPPFLAGS)  $(HDRS) LinkDef.h


dep:
	@echo  "Creating deps ..."



dist:
		rm -f $(AMSPRG).tar.gz ; \
		tar cvf $(AMSPRG).tar $(ALLSRCS) 00index.txt Make* ; \
		gzip $(AMSPRG).tar

clean:
		@rm -vf $(ALLOBJS) $(ALLDICT) $(DEPS) $(AMSPRG) $(AMSPRGC)


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

