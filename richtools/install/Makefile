# Compilers and linker
COMPILERPATH=/opt/intel/Compiler/11.1/073/

FF = $(COMPILERPATH)bin/intel64/ifort
CC = $(COMPILERPATH)bin/intel64/icc
LD = $(COMPILERPATH)bin/intel64/icpc 

# XTRA LIBRARIES
#CERN_ROOT = /afs/cern.ch/exp/ams/Offline/CERN/2006/cern/pro
CERN_ROOT = /afs/cern.ch/exp/ams/Offline/CERN/2005/
CERNLIB   = -L$(CERN_ROOT)/lib -Wl,-static -lmathlib -lpacklib -lkernlib -Wl,-dy  -lnsl -lcrypt -ldl 
FORTRANLIB   = -L$(COMPILERPATH)/lib/intel64   -lifport -lifcoremt -limf  -lipgo -lirc_s
ROOTLIB   = -L`root-config --libdir`  -lRIO -lTree -lSQL  -lNet -lNetx -lHist -lHistPainter -lGraf -lMatrix

# FLAGS
FFLAGS = -O3 -fPIC -c
CFLAGS = -O3 -fPIC -D__root_new -D__WRITEROOT__ -D__ROOTPOINTERS__ -D__BATCH__ -D__ROOTSHAREDLIBRARY__  -D_PGTRACK_ -Wno-deprecated -I../include -I../../include -I`root-config --incdir` -c
LDFLAGS = -shared $(CERNLIB) $(ROOTLIB) -lMinuit $(FORTRANLIB)

# DIRECTORIES
AMS_I_PATH = ../../include/
AMS_F_PATH = ../../F/
AMS_C_PATH = ../../CC/
RICH_F_PATH = ../F/
RICH_C_PATH = ../CC/
RICH_I_PATH = ../include/
# output
BINPATH= ../bin/
LIBPATH= ../lib/
TMPPATH= ../tmp/
EXEPATH = ../exe/

RICH_C_FILES = richdbc.C richid.C richradid.C richrec.C trrec.C
AMS_C_FILES = id.C point.C root.C amschain.C TrdHTrack.C TrdHSegment.C TrdHRecon.C VCon.C VCon_root.C TrElem.C
AMS_F_FILES= cerenkov.F richpmtlib.F
GENERAL= -L../lib -lshared -I../include -I../../include -I`root-config --incdir` `root-config --libs` $(CERNLIB) $(FORTRAN) -lMinuit ../../lib/linuxx8664icc/ntuple_slc4_PG.so

########### CLEANING 
clean: clean_dirs
	rm -f $(CPATH)rootcint.*

clean_dirs:
	rm -rf $(BINPATH) $(LIBPATH)
#Directories
dirs:
	mkdir -p $(BINPATH) $(EXEPATH) $(LIBPATH)

# Objects
$(AMS_C_FILES:%.C=$(BINPATH)%.o): $(BINPATH)%.o: $(AMS_C_PATH)%.C
	$(CC) $< $(CFLAGS) -o $@

$(RICH_C_FILES:%.C=$(BINPATH)%.o): $(BINPATH)%.o: $(RICH_C_PATH)%.C 
	$(CC) $< $(CFLAGS) -o $@

$(AMS_F_FILES:%.F=$(BINPATH)%.o): $(BINPATH)%.o: $(AMS_F_PATH)%.F 
	$(FF) $< $(FFLAGS) -o $@

########## LIBRARY
lib: methods dirs $(RICH_C_FILES:%.C=$(BINPATH)%.o) $(AMS_F_FILES:%.F=$(BINPATH)%.o) 
	@echo $(LD) $(LDFLAGS) -o $(LIBPATH)libshared.so  $(BINPATH)*.o
	$(LD) $(LDFLAGS) $(BINPATH)*.o -o $(LIBPATH)libshared.so  

# Something that could be missing
$(AMS_I_PATH)root_methods.h: ../../perl/root.perl
	cd ..;	perl ../perl/root.perl

methods: $(AMS_I_PATH)root_methods.h



# Contributions

CONTRIB = ../contrib/


#####################
# obsolete targets  #
#####################

################## NEW TARGETS

# Alignment of rich using AGL 
alignment: lib $(EXEPATH) $(CONTRIB)alignment/analysis.C $(CONTRIB)alignment/analysis.h
	$(CC) $(CONTRIB)$(@)/*.C $(GENERAL) -I$(CONTRIB)$(@) -o $(EXEPATH)$(@).exe

# Calibration of AGL indexes
index_calibration: lib $(EXEPATH) $(CONTRIB)index_calibration/analysis.C $(CONTRIB)index_calibration/analysis.h
	$(CC) $(CONTRIB)$(@)/*.C $(GENERAL) -I$(CONTRIB)$(@) -o $(EXEPATH)$(@).exe

# Tuning of the window parameter 
parameter_tuning: lib $(EXEPATH) $(CONTRIB)parameter_tuning/analysis.C $(CONTRIB)parameter_tuning/analysis.h
	$(CC) $(CONTRIB)$(@)/*.C $(GENERAL) -I$(CONTRIB)$(@) -o $(EXEPATH)$(@).exe

# Several test histograms
test: lib $(EXEPATH) $(CONTRIB)test/test.C $(CONTRIB)test/test.h
	$(CC) $(CONTRIB)$(@)/*.C $(GENERAL) -I$(CONTRIB)$(@) -o $(EXEPATH)$(@).exe

# Fit expansion height using NaF events
fit_naf_height: lib $(EXEPATH) $(CONTRIB)fit_naf_height/analysis.C $(CONTRIB)fit_naf_height/analysis.h
	$(CC) $(CONTRIB)$(@)/*.C $(GENERAL) -I$(CONTRIB)$(@) -o $(EXEPATH)$(@).exe

# Align the mirror using AGL events
alignment_mirror: lib $(EXEPATH) $(CONTRIB)alignment_mirror/analysis.C $(CONTRIB)alignment_mirror/analysis.h
	$(CC) $(CONTRIB)$(@)/*.C $(GENERAL) -I$(CONTRIB)$(@) -o $(EXEPATH)$(@).exe

# Preslect events to be use in the analysis chain
preselect: lib $(EXEPATH) $(CONTRIB)preselect/analysis.C $(CONTRIB)preselect/analysis.h
	$(CC) $(CONTRIB)$(@)/*.C $(GENERAL) -I$(CONTRIB)$(@) -o $(EXEPATH)$(@).exe

# Preslect events to be use in the analysis chain
tile_analysis: lib $(EXEPATH) $(CONTRIB)tile_analysis/analysis.C $(CONTRIB)tile_analysis/analysis.h
	$(CC) $(CONTRIB)$(@)/*.C $(GENERAL) -I$(CONTRIB)$(@) -o $(EXEPATH)$(@).exe

Forest: 
	cd $(CONTRIB)$(@); rootcint -f libForest.cxx -c clasifier.h LinkDef.h
	$(CC) -I$(CONTRIB)$(@) $(CONTRIB)$(@)/clasifier.cc $(CFLAGS) -o ../bin/clasifier.o
	$(CC) -I$(CONTRIB)$(@) $(CONTRIB)$(@)/libForest.cxx $(CFLAGS) -o ../bin/forest.o
	$(LD) $(LDFLAGS) -o $(LIBPATH)libForest.so  $(BINPATH)clasifier.o $(BINPATH)forest.o
	rm $(BINPATH)clasifier.o $(BINPATH)forest.o

$(LIBPATH)/libForest.so: Forest


####################### TESTS

parametric_rec_build: $(LIBPATH)/libForest.so
#	cd $(CONTRIB)$(@); rootcint -f libForest.cxx -c clasifier.h LinkDef.h
#	@echo $(LD) $(LDFLAGS) -o $(LIBPATH)libshared.so  $(BINPATH)*.o
#	$(CC) $(GENERAL) -I$(CONTRIB)$(@) `root-config --cflags --libs` $(CONTRIB)$(@)/libForest.cxx $(CONTRIB)$(@)/clasifier.cc $(CONTRIB)$(@)/*.C -o $(EXEPATH)$(@).exe
	$(CC) $(CONTRIB)$(@)/*.C -I$(CONTRIB)$(@) `root-config --cflags --libs` -lForest $(CONTRIB)$(@)/*.C -o $(EXEPATH)$(@).exe

# Preslect events to be use in the analysis chain
check_rec: lib $(EXEPATH) 
	$(CC) $(CONTRIB)$(@)/*.C $(GENERAL) -I$(CONTRIB)$(@) -o $(EXEPATH)$(@).exe


# Preslect events to be use in the analysis chain
tile_maps: lib $(EXEPATH) 
	$(CC) $(CONTRIB)$(@)/*.C $(GENERAL) -I$(CONTRIB)$(@) -o $(EXEPATH)$(@).exe

beta_map: $(LIBPATH)/libForest.so
	$(CC) $(CONTRIB)$(@)/*.C $(GENERAL) -I$(CONTRIB)$(@) `root-config --cflags --libs` -lForest -o $(EXEPATH)$(@).exe


####################### OTHER

# Creates RunClassifier files from raw files
rich_cnf_hkd: dirs $(CONTRIB)rich_cnf_hkd/rich_cnf_hkd.c $(CONTRIB)rich_cnf_hkd/rich_cnf_hkd.h
	$(CC) $(CONTRIB)$(@)/*.c -I$(CONTRIB)$(@) -o $(EXEPATH)$(@) -lm -static

