### Directories
SRCDIR         := CC
INCDIR         := include
OBJDIR         := obj
BINDIR         := bin

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
DEFINES        := -I$(AMSWD)/include -I$(INCDIR) -I$(OBJDIR)
ARCH           := $(shell $(ROOTCFG) --arch)
LIBS           := $(AMSWD)/lib/$(ARCH)/ntuple_slc4_PG.so
LIBS           += $(shell $(ROOTCFG) --glibs)

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

### Everything
all: $(BINDIR)/tkonline $(BINDIR)/tkhadd $(BINDIR)/tkprinter

### Linking 
$(BINDIR)/tkonline: $(OBJDIR)/tkonline.o $(COMMONOBJS)
	@echo Linking $@ ... 
	$(CXX) $(CXXFLAGS) $(DEFINES) $^ $(LIBS) -o $@

$(BINDIR)/tkhadd: $(OBJDIR)/tkhadd.o 
	@echo Linking $@ ... 
	$(CXX) $(CXXFLAGS) $(DEFINES) $^ $(LIBS) -o $@

$(BINDIR)/tkprinter: $(OBJDIR)/tkprinter.o $(COMMONOBJS) 
	@echo Linking $@ ... 
	$(CXX) $(CXXFLAGS) $(DEFINES) $^ $(LIBS) -o $@

### Rules for symbolic linking for few needed classes from GBATCH
$(SRCDIR)/%.C : $(AMSWD)/CC/%.C 
	@if [ ! -f $@ ] ; then echo Creating symbolic link  $< ...; ln -s $< $@; fi

### Rules for compilation  
$(OBJDIR)/%.o: $(SRCDIR)/%.cxx
	@echo Compiling $^ ...
	$(CXX) $(CXXFLAGS) $(DEFINES) -c $^ -o $@

### ROOT dictionary
$(OBJDIR)/%.o: $(OBJDIR)/%.cxx
	@echo Compiling $^ ROOT dictionary ...
	$(CXX) $(CXXFLAGS) $(DEFINES) -c $^ -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.C
	@echo Compiling $^ ...
	$(CXX) $(CXXFLAGS) $(DEFINES) -c $^ -o $@

### ROOT Dictionary
$(DICTSOURCE):
	@echo Creating $@ ROOT dictionary ...
	$(CINT) -f $@ -c $(DEFINES) $(DICTINC)

### Clean
clean:
	@echo Clean up ...
	@rm -rf $(BINDIR)/tkonline $(OBJDIR)/*

