FFLAGS = -m32 -fPIC -c
CFLAGS =  -m32 -fPIC  -D__root_new -D__WRITEROOT__ -D__ROOTPOINTERS__ -D__BATCH__ -D__ROOTSHAREDLIBRARY__  -Wno-deprecated -I../include -I`root-config --incdir` -c 
LDFLAGS =  -m32 -shared -lg2c `cernlib mathlib` -L`root-config --libdir`  -lRIO -lTree -lSQL  -lNet -lNetx -lHist -lHistPainter -lGraf -lMatrix 

FPATH = ../F/
CPATH = ../CC/
IPATH = ../include/
BINPATH= ../bin/
LIBPATH= ../lib/
TMPPATH= ../tmp/

AMSIPATH = ../../include/
AMSFPATH = ../../F/
AMSCPATH = ../../CC/

FF = g77
CC = g++
LD = g++ 

####################### AMS SOURCE FILES ####################


C_HEADERS = $(AMSIPATH)id.h $(AMSIPATH)linkdef.h $(AMSIPATH)ntuple.h $(AMSIPATH)point.h \
            $(AMSIPATH)richid_default_values.h $(AMSIPATH)root.h   $(AMSIPATH)root_methods.h \
            $(AMSIPATH)root_RVS.h  $(AMSIPATH)typedefs.h $(AMSIPATH)amschain.h $(AMSIPATH)AMSNtupleHelper.h


C_FORTRAN = $(AMSFPATH)cerenkov.F $(AMSFPATH)richpmtlib.F

C_C       = $(AMSCPATH)id.C   $(AMSCPATH)point.C  $(AMSCPATH)root.C $(AMSCPATH)amschain.C

SOURCE = $(CPATH)richdbc.C  $(CPATH)richid.C  $(CPATH)richradid.C  $(CPATH)richrec.C  $(CPATH)trrec.C \
	 $(C_C:$(AMSCPATH)%.C=$(CPATH)%.C) $(C_FORTRAN:$(AMSFPATH)%.F=$(FPATH)%.F)


# Main target

lib: $(BINPATH)rootcint.o $(SOURCE:$(CPATH)%.C=$(BINPATH)%.o) $(SOURCE:$(FPATH)%.F=$(BINPATH)%.o) $(LIBPATH)
	$(LD) $(LDFLAGS) -o $(LIBPATH)libshared.so  $(BINPATH)*.o

clean: $(C_HEADERS:$(AMSIPATH)%.h=$(IPATH)%.h) $(C_FORTRAN:$(AMSFPATH)%.F=$(FPATH)%.F) $(C_C:$(AMSCPATH)%.C=$(CPATH)%.C)
	rm $^
	rm -rf $(BINPATH) $(LIBPATH) $(TMPPATH)



# Dependencies

$(AMSIPATH)root_methods.h: ../../perl/root.perl
	cd ..;	perl ../perl/root.perl


#Directories
$(BINPATH):
	mkdir $(BINPATH)

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






