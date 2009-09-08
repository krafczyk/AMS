FFLAGS = -m32 -fPIC
CFLAGS = -m32 -fPIC  -D__root_new -D__WRITEROOT__ -D__ROOTPOINTERS__ -D__BATCH__ -D__ROOTSHAREDLIBRARY__ -Wno-deprecated -I../include -I`root-config --incdir`

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


####################### AMS SOURCE FILES ####################


C_HEADERS = $(AMSIPATH)id.h $(AMSIPATH)linkdef.h $(AMSIPATH)ntuple.h $(AMSIPATH)point.h \
            $(AMSIPATH)richid_default_values.h $(AMSIPATH)root.h   $(AMSIPATH)root_methods.h \
            $(AMSIPATH)root_RVS.h  $(AMSIPATH)typedefs.h $(AMSIPATH)amschain.h $(AMSIPATH)AMSNtupleHelper.h


C_FORTRAN = $(AMSFPATH)cerenkov.F $(AMSFPATH)richpmtlib.F

C_C       = $(AMSCPATH)id.C   $(AMSCPATH)point.C  $(AMSCPATH)root.C 

$(AMSIPATH)root_methods.h: ../../perl/root.perl
	cd ..;	perl ../perl/root.perl

copy_headers: $(C_HEADERS)
	cp --target-directory=$(IPATH) $^
copy_fortran:  $(C_FORTRAN)
	cp --target-directory=$(FPATH) $^

copy_c:  $(C_C)
	cp --target-directory=$(CPATH) $^

delete_headers: $(C_HEADERS:$(AMSIPATH)%.h=$(IPATH)%.h)
	rm $^

delete_fortran: $(C_FORTRAN:$(AMSFPATH)%.F=$(FPATH)%.F)
	rm $^

delete_c: $(C_C:$(AMSCPATH)%.C=$(CPATH)%.C)
	rm $^


clean: delete_headers delete_fortran delete_c
	rm -rf $(BINPATH) $(LIBPATH) $(TMPPATH)

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
	$(CC) $< $(CFLAGS) -c  -o $@

$(BINPATH)%.o: $(FPATH)%.F
	@echo Compiling $< ...
	$(FF) $< $(FFLAGS) -c  -o $@



###################### Source files #####################

SOURCE = $(CPATH)richdbc.C  $(CPATH)richid.C  $(CPATH)richradid.C  $(CPATH)richrec.C  $(CPATH)trrec.C


$(CPATH)%.C : $(IPATH)%.h


dictionary: $(TMPPATH) $(BINPATH) copy_headers copy_c 
	rootcint -f $(TMPPATH)rootcint.C -c $(IPATH)point.h $(IPATH)root_RVS.h $(IPATH)root.h $(IPATH)amschain.h $(IPATH)linkdef.h
	$(CC) $(TMPPATH)rootcint.C $(CFLAGS) -I$(TMPPATH) -c -o $(BINPATH)rootcint.o 

lib: $(LIBPATH) $(BINPATH) dictionary copy_headers copy_fortran copy_c $(SOURCE:$(CPATH)%.C=$(BINPATH)%.o) $(SOURCE:$(FPATH)%.F=$(BINPATH)%.o)  
	echo $(CC) -shared -lg2c `cernlib mathlib` -I`root-config --incdir` -lRIO -lTree -lSQL  -lNet -lNetx -lHist \
-lHistPainter -lGraf -lMatrix -o $(LIBPATH)libshared.so  $(BINPATH)*.o

