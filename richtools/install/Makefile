FORTRAN= -lg2c -lgfortran
FFLAGS = -m32 -fPIC -c
CFLAGS =  -m32 -fPIC  -D__root_new -D__WRITEROOT__ -D__ROOTPOINTERS__ -D__BATCH__ -D__ROOTSHAREDLIBRARY__  -Wno-deprecated -I../include -I`root-config --incdir` -c 
LDFLAGS =  -m32 -shared `cernlib mathlib` -L`root-config --libdir`  -lRIO -lTree -lSQL  -lNet -lNetx -lHist -lHistPainter -lGraf -lMatrix $(FORTRAN) 

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

FF = g77 -O3
CC = g++ -O3
LD = g++ -O3

# This is the version with the debugger
#FF = g77 -ggdb
#CC = g++ -ggdb
#LD = g++ -ggdb



####################### AMS SOURCE FILES ####################


C_HEADERS = $(AMSIPATH)id.h $(AMSIPATH)linkdef.h $(AMSIPATH)ntuple.h $(AMSIPATH)point.h \
            $(AMSIPATH)richid_default_values.h $(AMSIPATH)root.h   $(AMSIPATH)root_methods.h \
            $(AMSIPATH)root_RVS.h  $(AMSIPATH)typedefs.h $(AMSIPATH)amschain.h $(AMSIPATH)AMSNtupleHelper.h


C_FORTRAN = $(AMSFPATH)cerenkov.F $(AMSFPATH)richpmtlib.F

C_C       = $(AMSCPATH)id.C   $(AMSCPATH)point.C  $(AMSCPATH)root.C $(AMSCPATH)amschain.C

SOURCE = $(CPATH)richdbc.C  $(CPATH)richid.C  $(CPATH)richradid.C  $(CPATH)richrec.C  $(CPATH)trrec.C \
	 $(C_C:$(AMSCPATH)%.C=$(CPATH)%.C) $(C_FORTRAN:$(AMSFPATH)%.F=$(FPATH)%.F)


main:
	@echo Available targets:
	@echo "lib              - Build the shared library"
	@echo "contrib          - Build all the utilities in the contrib directory"
	@echo "test             - Build the contrib/test utility"
	@echo "alignment        - Build the contrib/alignment utility (not working properly yet)"
	@echo "manual_alignment - Build the contrib/manual_alignment utility "
	@echo "tuning           - Build the contrib/parameter_tuning utility, which tunes the CIEMAT beta reconstruction single parameter"
	@echo "clean            - Clean up everything (expect the exe directory)"
# Main target

lib: $(BINPATH)rootcint.o $(SOURCE:$(CPATH)%.C=$(BINPATH)%.o) $(SOURCE:$(FPATH)%.F=$(BINPATH)%.o) $(LIBPATH)
	@echo $(LD) $(LDFLAGS) -o $(LIBPATH)libshared.so  $(BINPATH)*.o
	$(LD) $(LDFLAGS) -o $(LIBPATH)libshared.so  $(BINPATH)*.o
	ar rv $(LIBPATH)libstatic.a $(BINPATH)*.o

clean: $(C_HEADERS:$(AMSIPATH)%.h=$(IPATH)%.h) $(C_FORTRAN:$(AMSFPATH)%.F=$(FPATH)%.F) $(C_C:$(AMSCPATH)%.C=$(CPATH)%.C)
	rm $^
	rm -rf $(BINPATH) $(LIBPATH) $(TMPPATH) 



# Dependencies

$(AMSIPATH)root_methods.h: ../../perl/root.perl
	cd ..;	perl ../perl/root.perl


#Directories
$(BINPATH):
	mkdir $(BINPATH)

$(EXEPATH):
	mkdir $(EXEPATH)

$(TMPPATH):
	mkdir $(TMPPATH)

$(LIBPATH)/:
	mkdir $(LIBPATH)

# Object files

$(BINPATH)%.o: $(CPATH)%.C
	@echo Compiling $< ...
	$(CC) $< $(CFLAGS) -o $@

$(BINPATH)%.o: $(FPATH)%.F
	@echo Compiling $< ...
	$(FF) $< $(FFLAGS)  -o $@

$(IPATH)%.h: $(AMSIPATH)%.h
	cp $< $@

$(CPATH)%.C: $(AMSCPATH)%.C
	cp $< $@

$(FPATH)%.F: $(AMSFPATH)%.F
	cp $< $@


# No dependencies 
$(CPATH)richdbc.C:

$(CPATH)richid.C:  

$(CPATH)richradid.C:

$(CPATH)richrec.C:

$(CPATH)trrec.C:

$(BINPATH)rootcint.o: $(TMPPATH) $(TMPPATH)/rootcint.C $(BINPATH)
	$(CC) $(TMPPATH)rootcint.C $(CFLAGS) -I$(TMPPATH) -o $(BINPATH)rootcint.o 

$(TMPPATH)/rootcint.C: $(TMPPATH) $(C_HEADERS:$(AMSIPATH)%.h=$(IPATH)%.h) 
	rootcint -f $(TMPPATH)rootcint.C -c $(IPATH)point.h $(IPATH)root_RVS.h $(IPATH)root.h $(IPATH)amschain.h $(IPATH)linkdef.h


# Contributions

CONTRIB = ../contrib/

GENERAL= -m32 -L../lib -lshared -I../include -I`root-config --incdir` `root-config --libs` `cernlib mathlib` $(FORTRAN)

contrib: test alignment manual_alignment tuning

test: lib $(EXEPATH) $(CONTRIB)test/test.C
	$(CC) $(GENERAL) -I$(CONTRIB)test $(CONTRIB)test/test.C -o $(EXEPATH)test.exe

alignment: lib $(EXEPATH) $(CONTRIB)alignment/align.C
	$(CC) $(GENERAL) -lMinuit -I$(CONTRIB)alignment $(CONTRIB)alignment/*.C -o $(EXEPATH)alignment.exe


manual_alignment: lib $(EXEPATH) $(CONTRIB)manual_aligment/align.C
	$(CC) $(GENERAL) -lMinuit -I$(CONTRIB)manual_aligment $(CONTRIB)manual_aligment/*.C -o $(EXEPATH)manual_aligment.exe

tuning: lib $(EXEPATH) $(CONTRIB)parameter_tuning/analysis.C
	$(CC) $(GENERAL) -I$(CONTRIB)parameter_tuning $(CONTRIB)parameter_tuning/*.C -o $(EXEPATH)tuning.exe

full_calibration_tool: lib $(CONTRIB)full_calibration_tool/analysis.C
	$(CC) $(GENERAL) -I$(CONTRIB)$(@) $(CONTRIB)$(@)/*.C -o $(EXEPATH)$(@).exe
