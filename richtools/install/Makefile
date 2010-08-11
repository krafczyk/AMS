FPATH = ../F/
CPATH = ../CC/
IPATH = ../include/
BINPATH= ../bin/
LIBPATH= ../lib/
TMPPATH= ../tmp/
EXEPATH = ../exe/

AMSIPATH = ../../include/
AMSFPATH = ../../F/
AMSCPATH = ../../CC/

FORTRAN= -lg2c -lgfortran
FFLAGS = -m32 -fPIC -c
CFLAGS =  -m32 -fPIC  -D__root_new -D__WRITEROOT__ -D__ROOTPOINTERS__ -D__BATCH__ -D__ROOTSHAREDLIBRARY__  -Wno-deprecated -I../include -I../../include -I`root-config --incdir` -c  
LDFLAGS =  -m32 -shared `cernlib mathlib` -L`root-config --libdir`  -lRIO -lTree -lSQL  -lNet -lNetx -lHist -lHistPainter -lGraf -lMatrix $(FORTRAN) 


FF = g77 -O3
CC = g++ -O3
LD = g++ -O3

################### FILES SPECIFIC TO RICHTOOLS
C_FILES =   richdbc.C richid.C richradid.C richrec.C trrec.C
COMMON_C_FILES=  id.C point.C root.C amschain.C TrdHTrack.C TrdHSegment.C TrdHRecon.C VCon.C VCon_root.C TrElem.C
COMMON_F_FILES= cerenkov.F richpmtlib.F



################### FEW GENERAL RULES
#Directories
$(BINPATH):
	mkdir $(BINPATH)

$(EXEPATH):
	mkdir $(EXEPATH)

$(TMPPATH):
	mkdir $(TMPPATH)

$(LIBPATH)/:
	mkdir $(LIBPATH)

$(COMMON_C_FILES:%.C=$(BINPATH)%.o): $(BINPATH)%.o: $(AMSCPATH)%.C
	$(CC) $< $(CFLAGS) -o $@

$(C_FILES:%.C=$(BINPATH)%.o): $(BINPATH)%.o: $(CPATH)%.C
	$(CC) $< $(CFLAGS) -o $@

$(COMMON_F_FILES:%.F=$(BINPATH)%.o): $(BINPATH)%.o: $(AMSFPATH)%.F
	$(FF) $< $(FFLAGS) -o $@


$(CPATH)/rootcint.C: 
	rootcint -f $(CPATH)rootcint.C -c $(AMSIPATH)point.h $(AMSIPATH)root_RVS.h $(AMSIPATH)root.h $(AMSIPATH)amschain.h $(AMSIPATH)VCon_root.h $(AMSIPATH)linkdef.h


$(BINPATH)rootcint.o: $(CPATH)/rootcint.C $(BINPATH)
	$(CC) $(CPATH)rootcint.C  -I$(TMPPATH) $(CFLAGS) -o $(BINPATH)rootcint.o 


rootcint: $(BINPATH)rootcint.o


objects: methods $(BINPATH) $(COMMON_C_FILES:%.C=$(BINPATH)%.o)  $(C_FILES:%.C=$(BINPATH)%.o) $(COMMON_F_FILES:%.F=$(BINPATH)%.o) $(BINPATH)rootcint.o | $(BINPATH)

# Something that could be missing
$(AMSIPATH)root_methods.h: ../../perl/root.perl
	cd ..;	perl ../perl/root.perl

methods: $(AMSIPATH)root_methods.h

########### CLEANING 
clean: clean_dirs


clean_dirs:
	rm -rf $(BINPATH) $(LIBPATH)

########## LIBRARY

lib: objects $(LIBPATH) | $(LIBPATH)
	@echo $(LD) $(LDFLAGS) -o $(LIBPATH)libshared.so  $(BINPATH)*.o
	$(LD) $(LDFLAGS) -o $(LIBPATH)libshared.so  $(BINPATH)*.o
	ar rv $(LIBPATH)libstatic.a $(BINPATH)*.o




# Contributions

CONTRIB = ../contrib/

GENERAL= -m32 -L../lib -lshared -I../include -I../../include -I`root-config --incdir` `root-config --libs` `cernlib mathlib` $(FORTRAN)

contrib: test alignment manual_alignment tuning

test: lib $(EXEPATH) $(CONTRIB)test/test.C
	$(CC) $(GENERAL) -I$(CONTRIB)test $(CONTRIB)test/test.C -o $(EXEPATH)test.exe

alignment: lib $(EXEPATH) $(CONTRIB)alignment/analysis.C $(CONTRIB)alignment/analysis.h
	$(CC) $(GENERAL) -I$(CONTRIB)$(@) $(CONTRIB)$(@)/*.C -o $(EXEPATH)$(@).exe


manual_alignment: lib $(EXEPATH) $(CONTRIB)manual_aligment/align.C
	$(CC) $(GENERAL) -lMinuit -I$(CONTRIB)manual_aligment $(CONTRIB)manual_aligment/*.C -o $(EXEPATH)manual_aligment.exe

tuning: lib $(EXEPATH) $(CONTRIB)parameter_tuning/analysis.C
	$(CC) $(GENERAL) -I$(CONTRIB)parameter_tuning $(CONTRIB)parameter_tuning/*.C -o $(EXEPATH)tuning.exe

full_calibration_tool: lib $(CONTRIB)full_calibration_tool/analysis.C
	$(CC) $(GENERAL) -I$(CONTRIB)$(@) $(CONTRIB)$(@)/*.C -o $(EXEPATH)$(@).exe

lg_analysis: lib $(CONTRIB)lg_analysis/analysis.C
	$(CC) $(GENERAL) -I$(CONTRIB)$(@) $(CONTRIB)$(@)/*.C -o $(EXEPATH)$(@).exe


amoeba: lib $(CONTRIB)amoeba/analysis.C $(CONTRIB)amoeba/analysis.h
	$(CC) $(GENERAL) -I$(CONTRIB)$(@) $(CONTRIB)$(@)/*.C -o $(EXEPATH)$(@).exe

