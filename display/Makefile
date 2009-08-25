#  $Id: Makefile,v 1.44 2009/08/25 09:28:28 pzuccon Exp $
# PZ reviewing the display Makefile on Aug 2009
#    WIN32 ans MACOSX arch commented as not supported in GBATCH for the moment
#    LINUX arch supported is SLC4 with gcc or  icc



ARCH         := $(shell $(ROOTSYS)/bin/root-config --arch )
PLATFORM      =  Linux
PREFIX        = $(AMSWD)/bin/$(ARCH)/display/
DEPDIR        = $(PREFIX)/dep

AMSLIB        = ntuple_slc4
AMSPRG        = $(AMSWD)/exe/$(ARCH)/amsed
AMSPRGC       = $(AMSWD)/exe/$(ARCH)/amsedc
SLC4=y

ifdef PGTRACK
PREFIX        = $(AMSWD)/bin/$(ARCH)/display/PG/
AMSLIB        = ntuple_slc4_PG
AMSPRG        = $(AMSWD)/exe/$(ARCH)/amsedPG
AMSPRGC       = $(AMSWD)/exe/$(ARCH)/amsedcPG
endif



# display compiles only with customized ROOT, at the moment v5.21
NEW         :=_new

VER          := $(shell $(ROOTSYS)/bin/root-config --version )

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
LDS	= 	$(LD) -Bstatic
LDD	= 	$(LD) -Bdynamic

ROOTLIBS      =  $(shell $(ROOTSYS)/bin/root-config --glibs) -lASImage -lRFIO -lNetx -lGX11 -lGX11TTF
ROOTLIBSA     =   -L$(ROOTSYS)/lib -lRoot -pthread  -lAfterImage $(ROOTSYS)/lib/libXrdClient.a -lpcre \
	          -L/usr/X11R6/lib -lXft -lfontconfig -lXrender -lfreetype  -lexpat   \
                  -lXpm -lXmu -lXt -lXext -lX11 -ljpeg -lpng -ltiff   -lXrd -lXrdNet -lXrdOuc -lXrdSys 

LIBS          =  -L$(AMSWD)/lib/linuxicc/ -lntuple_slc4 $(ROOTLIBS)  -lshift -lm    -ldl
LIBSA         =  -L$(AMSWD)/lib/linuxicc/ -lntuple_slc4 $(ROOTLIBSA) -lshift -lm   -Bdynamic          -ldl 


else
CXX           =  g++ -m32 
CPPFLAGS      =   -D__WRITEROOT__ -D__ROOTSHAREDLIBRARY__  -I. -I../include -I$(ROOTSYS)/include -I/usr/include/freetype2 
CXXFLAGS      = -Wno-deprecated $(OPT) $(CPPFLAGS)
OPT           = -O2
NOOPT         = -g

LD            = g++ -m32
LDFLAGS       = -g 
LDS	= 	$(LD) -static
LDD	= 	$(LD) -rdynamic


ROOTLIBS       = $(shell $(ROOTSYS)/bin/root-config --glibs) -lASImage -lRFIO -lNetx -lGX11 -lGX11TTF
ROOTLIBSA      =  -L$(ROOTSYS)/lib -lRoot -pthread  -lAfterImage $(ROOTSYS)/lib/libXrdClient.a -lpcre \
	          -L/usr/X11R6/lib -lXft -lfontconfig -lXrender -lfreetype  -lexpat   \
                  -lXpm -lXmu -lXt -lXext -lX11 -ljpeg -lpng -ltiff   -lXrd -lXrdNet -lXrdOuc -lXrdSys 


LIBSA         =   -L$(AMSWD)/lib/linux/ -l$(AMSLIB)  $(ROOTLIBSA)  -lshift -lm -ldl
LIBS          =   -L$(AMSWD)/lib/linux/ -l$(AMSLIB)  $(ROOTLIBS) -lshift -lm -ldl
endif


ifdef PGTRACK
CPPFLAGS+= -D_PGTRACK_
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
                 AMSNtupleV.cxx       Help.cxx

SRCSN          = AMSR_GeometrySetter.cxx  AMSDisplay$(NEW).cxx \
                 TMarker3DCl$(NEW).cxx    AMSR_Axis.cxx 

HDRS          = TGRunEventDialog.h    ControlFrame.h \
		AMSNtupleV.h                         \
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

standalone:  $(AMSPRG)
osdepend:  $(AMSPRGC)




$(AMSPRG):    $(OBJSN)  $(OBJS) $(DICTO)  
		$(LDS)   $(LDFLAGS) $^  $(LIBSA)  -o $(@) 
		@echo "$(AMSPRG) done"

$(AMSPRGC):	$(OBJSN)  $(OBJS) $(DICTO)  
		$(LDD)   $(LDFLAGS) $^  $(LIBS) -o $(@) 
		@echo "$(AMSPRGC) done"



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

