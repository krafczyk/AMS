### Directories
SRCDIR         := CC
INCDIR         := include
OBJDIR         := obj
BINDIR         := bin
CXX            :=$(shell root-config --cxx)

ifndef AMSWD
AMSWD := ..
endif

VERSION      := $(shell $(ROOTSYS)/bin/root-config --version | cut -b1-4)
ifeq ($(VERSION),5.27)
VERSIONP=
else
VERSIONP = $(VERSION)
endif

### Options
CINT           := $(ROOTSYS)/bin/rootcint
ROOTCFG        := $(ROOTSYS)/bin/root-config
ARCH           := $(shell $(ROOTCFG) --arch)
CXXFLAGS       := -g -Wno-deprecated -Wno-write-strings -D_PGTRACK_ -D_STANDALONE_ -D__ROOTSHAREDLIBRARY__ 
ifeq ($(ARCH),macosx64)
  CXXFLAGS     += -D__DARWIN__ 
else
  CXXFLAGS     += -D__LINUXGNU__
endif
CXXFLAGS       += $(shell $(ROOTCFG) --cflags) 
DEFINES        := -I../include -I$(INCDIR) -I$(OBJDIR)
ARCH           := $(shell $(ROOTCFG) --arch)
LIBS           := -L$(AMSWD)/lib/$(ARCH)$(VERSIONP)/ -lntuple_slc4_PG
LIBS           += $(shell $(ROOTCFG) --glibs) -lRIO -lTree -lThread -lGpad -lNet -lNetx -lHist -lMinuit -lHistPainter -lGraf -lMatrix -lTMVA
ifneq ($(ARCH),macosx64)
LIBS           += -lRFIO
endif

ifeq ($(ARCH),macosx64)

#OSXVER := $(shell sw_vers -productVersion | sed 's/\.//g')
OSXVER := $(shell if [ `sw_vers -productVersion | sed 's/\.//g'` == "109" ]; then echo 1090; else echo `sw_vers -productVersion | sed 's/\.//g'`; fi)
DEFINES+=-D__OSXVER__=$(OSXVER)

endif

### Additional Classes Dictionary  
CLASSES        := CalPars SliderI CalSlider MonSlider GenericSlider ToggleButton ButtonPanel InteractionPanel SearchPanel TrackerPanel TimerPanel MonitorUI CalList
CLASSES        += Predict
DICTINCNAMES   := $(CLASSES) LinkDef
DICTINC        := $(addprefix $(PWD)/$(INCDIR)/, $(addsuffix .h, $(DICTINCNAMES)))
DICTNAME       := tkonlinedict
DICTSOURCE     := $(addprefix $(OBJDIR)/, $(addsuffix .cxx, $(DICTNAME)))

### Additional needed GBATCH files (%.C) 
ADDGBATCH      := timeid commonsi

### Additional Objects
COMMONOBJNAMES := $(CLASSES) $(DICTNAME) $(ADDGBATCH) 
COMMONOBJS     := $(addprefix $(OBJDIR)/, $(addsuffix .o, $(COMMONOBJNAMES)))  

### General Rules
.PHONY: all clean  
.SECONDARY: $(COMMONOBJS)

### Development?
DEV:=

### Everything
all: $(BINDIR)/tkonline$(DEV) $(BINDIR)/tkhadd$(DEV) $(BINDIR)/tkprinter$(DEV)

### Linking 
$(BINDIR)/tkonline$(DEV): $(OBJDIR)/tkonline.o $(COMMONOBJS)
	@echo Linking $@ ... 
	@mkdir -p bin
	$(CXX) $(CXXFLAGS) $(DEFINES) $^ $(LIBS) -o $@

$(BINDIR)/tkhadd$(DEV): $(OBJDIR)/tkhadd.o 
	@echo Linking $@ ... 
	@mkdir -p bin
	$(CXX) $(CXXFLAGS) $(DEFINES) $^ $(LIBS) -o $@

$(BINDIR)/tkprinter$(DEV): $(OBJDIR)/tkprinter.o $(COMMONOBJS) 
	@echo Linking $@ ... 
	@mkdir -p bin
	$(CXX) $(CXXFLAGS) $(DEFINES) $^ $(LIBS) -o $@

### Rules for symbolic linking for few needed classes from GBATCH
$(SRCDIR)/%.C : $(AMSSRC)/CC/%.C 
	if [ ! -f $@ ] ; then echo Creating symbolic link  $< ...; ln -s $< $@; fi

### Rules for compilation  
$(OBJDIR)/%.o: $(SRCDIR)/%.cxx
	@echo Compiling $^ ...
	@mkdir -p obj
	$(CXX) $(CXXFLAGS) $(DEFINES) -c $^ -o $@

### ROOT dictionary
$(OBJDIR)/%.o: $(OBJDIR)/%.cxx
	@echo Compiling $^ ROOT dictionary ...
	@mkdir -p obj
	$(CXX) $(CXXFLAGS) $(DEFINES) -c $^ -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.C
	@echo Compiling $^ ...
	@mkdir -p obj
	$(CXX) $(CXXFLAGS) $(DEFINES) -c $^ -o $@

### ROOT Dictionary
$(DICTSOURCE):
	@echo Creating $@ ROOT dictionary ...
	$(CINT) -f $@ -c -p $(DEFINES) $(DICTINC)

### Clean
clean:
	@echo Clean up ...
	@rm -rf $(BINDIR) $(OBJDIR)

