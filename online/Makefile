#  $Id: Makefile,v 1.5 2003/06/18 16:54:42 choutko Exp $

PLATFORM      =  Linux

CXX           =  g++ -Wno-deprecated
CXXFLAGS      =   -D__WRITEROOT__ -D__ROOTSHAREDLIBRARY__  -I../include -I$(ROOTSYS)/include -I/usr/include/freetype2 
OPT           = -O2
NOOPT         = -g
LD            = g++
LDFLAGS       = -g 

ROOTLIBS      =  -L$(ROOTSYS)/lib -lRoot -lfreetype -L/usr/X11R6/lib  -lXpm -lXmu -lXt -lXext -lX11 

LIBS          =  $(ROOTLIBS) -lm -ldl
GLIBS         = -L$(ROOTSYS)/lib -lGX11TTF  -lCore -lCint -lHist -lGraf -lGraf3d -lGui -lGpad -lGX11  -lTree -lRint -lPostscript -lMatrix -lPhysics  -lHistPainter -lMinuit -L/usr/X11R6/lib  -lXmu -lXt -lXext -lX11 -lXpm -lm -ldl 

##### MACROS #####

ROOTH         = TTree.h TFile.h TH1.h

SRCS          = AMSDisplay.cxx \
                AMSHist.cxx \
                AMSAntiHist.cxx\
                AMSTrackerHist.cxx\
                AMSRICHHist.cxx\
                AMSECALHist.cxx\
                AMSTRDHist.cxx\
                AMSTOFHist.cxx\
                AMSLVL1Hist.cxx\
                AMSLVL3Hist.cxx\
                AMSAxAMSHist.cxx\
                AMSGenHist.cxx\
                AMSNtuple.cxx  ControlFrame.cxx\

HDRS          =   \
                ../include/root_RVS.h\
		AMSDisplay.h \
		AMSHist.h \
		AMSAntiHist.h \
		AMSTrackerHist.h \
                AMSRICHHist.h\
                AMSECALHist.h\
                AMSTRDHist.h\
                AMSTOFHist.h\
                AMSLVL1Hist.h\
                AMSLVL3Hist.h ControlFrame.h\
                AMSAxAMSHist.h\
                AMSGenHist.h\
                AMSNtuple.h\
                LinkDef.h
PREFIX        = $(AMSWD)/online/
DICT          = AMSCint.cxx
DICTH         = $(DICT:.cxx=.h)
DICTO         = $(DICT:%.cxx=$(PREFIX)%.o)


MAIN          = main.cxx
MAINH         = $(MAIN:.cxx=.h)
MAINO         = $(MAIN:%.cxx=$(PREFIX)%.o)

MAINC          = mainc.cxx
MAINOC         = $(MAINC:.cxx=.o)

OBJS          = $(SRCS:%.cxx=$(PREFIX)%.o) 
OBJSR          = $(PREFIX)root.o


ALLDICT       = $(DICT) $(DICTH) 
ALLSRCS       = $(SRCS)  $(HDRS)  $(ALLDICT) $(MAIN) $(MAINH) $(MAINC)
ALLOBJS       = $(OBJS) $(MAINO) 


AMSLIB       = libAMS.a

AMSSHAREDLIB = libAMS.so


AMSPRG       = $(PREFIX)offmon
AMSPRGC       = $(PREFIX)offmonc



##### TARGETS #####
interprete:  $(AMSSHAREDLIB)

standalone:  $(AMSPRG)

osdepend:  $(AMSPRGC)



$(AMSPRG):      $(ALLOBJS) $(DICTO) $(MAINH) $(OBJSR)
		$(LD)  -static  $(LDFLAGS) $(ALLOBJS) $(OBJSR)  $(DICTO)  $(LIBS)  \
		-o $(@) 
		@echo "$(AMSPRG) done"

$(AMSPRGC):	$(ALLOBJS) $(DICTO) $(OBJSR)
		$(LD) -rdynamic $(LDFLAGS) $(ALLOBJS) $(DICTO) $(OBJSR)   $(LIBS) -o $(AMSPRGC) 


$(DICT):        $(HDRS)
		@echo "Generating dictionary ..."
		rootcint -f $(DICT) -c $(ROOTH) $(HDRS)




depend:		$(PREFIX)Make-depend 
touch_depend:
		touch $(PREFIX)Make-depend
$(PREFIX)Make-depend:	
ifneq "$(shell du -a  $(PREFIX)Make-depend | grep -c  ake)" "1"
		touch $(PREFIX)Make-depend
endif
	makedepend -f$(PREFIX)Make-depend -- $(CXXFLAGS) -- $(SRCS) $(SRCSG) -p$(PREFIX) \
		$(MAIN)
		cat  $(PREFIX)Make-depend | sed -e 'sx/../CC/x/x1' > $(PREFIX)Make-depend.new
		rm -f $(PREFIX)Make-depend
		mv $(PREFIX)Make-depend.new $(PREFIX)Make-depend

dist:
		rm -f $(AMSPRG).tar.gz ; \
		tar cvf $(AMSPRG).tar $(ALLSRCS) 00index.txt Make* ; \
		gzip $(AMSPRG).tar

clean:
		@rm -f $(ALLOBJS) $(ALLDICT)


##### RULES #####

$(ALLOBJS): $(PREFIX)%.o : %.cxx 
	$(CXX) $(NOOPT) $(CXXFLAGS) -c $(<)  -o $(@)

$(OBJSR): $(PREFIX)%.o : ../CC/%.C 
	$(CXX) $(NOOPT) $(CXXFLAGS) -c $(<)  -o $(@)

$(DICTO): $(PREFIX)%.o : %.cxx 
	$(CXX) $(NOOPT) $(CXXFLAGS)  -c $(<)  -o $(@)


##### DEPENDENCIES #####

ifneq ($(MAKECMDGOALS),touch_depend)
include $(PREFIX)Make-depend
endif

