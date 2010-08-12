CERN_ROOT = /afs/cern.ch/exp/ams/Offline/CERN/2006/cern/pro

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

#CERNLIB = `cernlib mathlib`
CERNLIB = -L/afs/cern.ch/exp/ams/Offline/CERN/2006/cern/pro/lib -Wl,-static -lmathlib -lpacklib -lkernlib -Wl,-dy -lm -lnsl -lcrypt -ldl 
#FORTRAN= -lg2c -lgfortran
FORTRAN=  -L/opt/intel//Compiler/11.0/069//lib/ia32     -lifport -lifcoremt -limf  -lipgo -lirc_s
FFLAGS = -m32 -fPIC -c
CFLAGS =  -m32 -fPIC  -D__root_new -D__WRITEROOT__ -D__ROOTPOINTERS__ -D__BATCH__ -D__ROOTSHAREDLIBRARY__  -Wno-deprecated -I../include -I../../include -I`root-config --incdir` -c  
#LDFLAGS =  -m32 -shared `cernlib mathlib` -L`root-config --libdir`  -lRIO -lTree -lSQL  -lNet -lNetx -lHist -lHistPainter -lGraf -lMatrix $(FORTRAN)
LDFLAGS =  -m32 -shared $(CERNLIB) -L`root-config --libdir`  -lRIO -lTree -lSQL  -lNet -lNetx -lHist -lHistPainter -lGraf -lMatrix $(FORTRAN) 

#GENERAL= -m32 -L../lib -lshared -I../include -I../../include -I`root-config --incdir` `root-config --libs` `cernlib mathlib` $(FORTRAN)
GENERAL= -m32 -L../lib -lshared -I../include -I../../include -I`root-config --incdir` `root-config --libs` $(CERNLIB) $(FORTRAN)


#FF = g77 -O3
#CC = g++ -O3
#LD = g++ -O3

FF = /opt/intel//Compiler/11.0/069//bin/ia32/ifort -O3
CC = /opt/intel//Compiler/11.0/069//bin/ia32/icc -O3
LD = /opt/intel//Compiler/11.0/069//bin/ia32/icpc -O3




################### FILES SPECIFIC TO RICHTOOLS
C_FILES =   richdbc.C richid.C richradid.C richrec.C trrec.C
COMMON_C_FILES=  id.C point.C root.C amschain.C TrdHTrack.C TrdHSegment.C TrdHRecon.C VCon.C VCon_root.C TrElem.C
COMMON_F_FILES= cerenkov.F richpmtlib.F
ROOTCINT_INCLUDES= $(AMSIPATH)root_RVS.h $(AMSIPATH)root.h $(AMSIPATH)amschain.h $(AMSIPATH)VCon_root.h 
ROOTCINT_FLAGS=



#################### PGTRACK STUF

ifdef PGTRACK
CFLAGS+= -D_PGTRACK_
ROOTCINT_FLAGS+=  -D_PGTRACK_
ROOTCINT_INCLUDES+= $(AMSIPATH)TkPlane.h $(AMSIPATH)TkSens.h $(AMSIPATH)TrCalDB.h $(AMSIPATH)patt.h $(AMSIPATH)TkCoo.h \
		    $(AMSIPATH)TkSens.h  $(AMSIPATH)TrRecon.h $(AMSIPATH)TkDBc.h  $(AMSIPATH)MagField.h $(AMSIPATH)TrLadCal.h   \
	  	    $(AMSIPATH)TkLadder.h   $(AMSIPATH)TkObject.h $(AMSIPATH)TrLadPar.h  $(AMSIPATH)TrAlignFit.h \
		    $(AMSIPATH)TrCluster.h  $(AMSIPATH)TrRecHit.h $(AMSIPATH)TrTrack.h $(AMSIPATH)TrTasCluster.h \
		    $(AMSIPATH)TrTasPar.h $(AMSIPATH)TrTasDB.h $(AMSIPATH)TrFit.h $(AMSIPATH)TrMCCluster.h $(AMSIPATH)trrec.h \
	            $(AMSIPATH)TrSim.h $(AMSIPATH)TrSimSensor.h $(AMSIPATH)TrSimCluster.h 


COMMON_C_FILES+=  rnormx.C TrFit.C TrRecon.C TrSim.C TrSimCluster.C TrSimSensor.C TrCalDB.C TrTasDB.C MagField.C \
  	          TkCoo.C TkDBc.C tkdcards.C  TkLadder.C TkObject.C TkPlane.C  TkSens.C TrCluster.C \
		  TrLadCal.C TrMCCluster.C TrRecHit.C TrTrack.C  Vertex.C  TrRawCluster.C patt.C \
	          TrLadPar.C  TrParDB.C	TrTasCluster.C TrTasPar.C TrAlignFit.C HistoMan.C amsdbc.C
GENERAL += -lMinuit

GENERAL+= -D_PGTRACK_

endif



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
	rootcint -f $(CPATH)rootcint.C -c $(ROOTCINT_FLAGS) $(ROOTCINT_INCLUDES) $(AMSIPATH)linkdef.h


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
	rm $(CPATH)rootcint.*

clean_dirs:
	rm -rf $(BINPATH) $(LIBPATH)

########## LIBRARY

lib: objects $(LIBPATH) | $(LIBPATH)
	@echo $(LD) $(LDFLAGS) -o $(LIBPATH)libshared.so  $(BINPATH)*.o
	$(LD) $(LDFLAGS) -o $(LIBPATH)libshared.so  $(BINPATH)*.o
	ar rv $(LIBPATH)libstatic.a $(BINPATH)*.o




# Contributions

CONTRIB = ../contrib/




#####################
# obsolete targets  #
#####################

manual_alignment: lib $(EXEPATH) $(CONTRIB)manual_aligment/align.C
	$(CC) $(GENERAL) -lMinuit -I$(CONTRIB)manual_aligment $(CONTRIB)manual_aligment/*.C -o $(EXEPATH)manual_aligment.exe



full_calibration_tool: lib $(CONTRIB)full_calibration_tool/analysis.C
	$(CC) $(GENERAL) -I$(CONTRIB)$(@) $(CONTRIB)$(@)/*.C -o $(EXEPATH)$(@).exe

lg_analysis: lib $(CONTRIB)lg_analysis/analysis.C
	$(CC) $(GENERAL) -I$(CONTRIB)$(@) $(CONTRIB)$(@)/*.C -o $(EXEPATH)$(@).exe


amoeba: lib $(CONTRIB)amoeba/analysis.C $(CONTRIB)amoeba/analysis.h
	$(CC) $(GENERAL) -I$(CONTRIB)$(@) $(CONTRIB)$(@)/*.C -o $(EXEPATH)$(@).exe



################## NEW TARGETS

# Alignment of rich using AGL 
alignment: lib $(EXEPATH) $(CONTRIB)alignment/analysis.C $(CONTRIB)alignment/analysis.h
	$(CC) $(GENERAL) -I$(CONTRIB)$(@) $(CONTRIB)$(@)/*.C -o $(EXEPATH)$(@).exe

# Calibration of AGL indexes
index_calibration: lib $(EXEPATH) $(CONTRIB)index_calibration/analysis.C $(CONTRIB)index_calibration/analysis.h
	$(CC) $(GENERAL) -I$(CONTRIB)$(@) $(CONTRIB)$(@)/*.C -o $(EXEPATH)$(@).exe

# Tuning of the window parameter 
parameter_tuning: lib $(EXEPATH) $(CONTRIB)parameter_tuning/analysis.C $(CONTRIB)parameter_tuning/analysis.h
	$(CC) $(GENERAL) -I$(CONTRIB)$(@) $(CONTRIB)$(@)/*.C -o $(EXEPATH)$(@).exe

# Several test histograms
test: lib $(EXEPATH) $(CONTRIB)test/test.C $(CONTRIB)test/test.h
	$(CC) $(GENERAL) -I$(CONTRIB)$(@) $(CONTRIB)$(@)/*.C -o $(EXEPATH)$(@).exe

# Fit expansion height using NaF events
fit_naf_height: lib $(EXEPATH) $(CONTRIB)fit_naf_height/analysis.C $(CONTRIB)fit_naf_height/analysis.h
	$(CC) $(GENERAL) -I$(CONTRIB)$(@) $(CONTRIB)$(@)/*.C -o $(EXEPATH)$(@).exe

# Align the mirror using AGL events
alignment_mirror: lib $(EXEPATH) $(CONTRIB)alignment_mirror/analysis.C $(CONTRIB)alignment_mirror/analysis.h
	$(CC) $(GENERAL) -I$(CONTRIB)$(@) $(CONTRIB)$(@)/*.C -o $(EXEPATH)$(@).exe

# Preslect events to be use in the analysis chain
preselect: lib $(EXEPATH) $(CONTRIB)preselect/analysis.C $(CONTRIB)preselect/analysis.h
	$(CC) $(GENERAL) -I$(CONTRIB)$(@) $(CONTRIB)$(@)/*.C -o $(EXEPATH)$(@).exe

# Preslect events to be use in the analysis chain
tile_analysis: lib $(EXEPATH) $(CONTRIB)tile_analysis/analysis.C $(CONTRIB)tile_analysis/analysis.h
	$(CC) $(GENERAL) -I$(CONTRIB)$(@) $(CONTRIB)$(@)/*.C -o $(EXEPATH)$(@).exe

