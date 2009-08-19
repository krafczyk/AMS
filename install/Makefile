#  $Id: Makefile,v 1.76 2009/08/19 14:36:45 pzuccon Exp $

#P.Zuccon june 2005

# definition of cppflags and inc paths within Makefile
# avoid multiple listings of files, using where possible the generation
#      of objects names from the source names
#
# tentative classification of source files
# 
# renaming of some variable to avoid conficts with the gmake 
#          implicit variables (ex CPP is C pre processor)
#
# many comments added

#rl october 2003

# removed direct references to /afs within the MAKEFILE
#   created variables for these references (CERNICC AMSLIB AMSICCLIB)
#     so that it is easier to create a local version of the Mopt by
#     defining these variables once at the TOP of the OS specific Makefile
#    rather than making changes in many places
#    also makes it easier to change if the library paths change
#
# removed most rules for compiling from sources
#      created default rules which will work for ~80% of the source files
#      makes it easier to add a program as the user
#      doesn't  have to add a rule to compile
#
# removed ifdefs for Operating system and included an OS specific file 
#     instead.  This makes it easier to edit files for a specific OS
#    it also automatically provides for new a OS (if necessary)
#
#   added a variable SFF = ../F which is used in the default rules
#             for compiling .F .f files
#
# made some formatting changes to make it easier to read (I think)
#
#---------------------------
# this Makefile calls OS specific Makefiles which set specific
# flags/variables for that OS.


#rl variables that must be defined 
#  CERN_LEVEL 
#  CERNDIR     root directory of the cern tree e.g /cern/pro
#  CERNICC     root directory of cern tree for intel compiler on Linux 
#  CERNSRCDIR  root directory for CERN SOURCES 
#  ROOTSYS     root directory for ROOT (defined externally via amsvar)
#  AMSLIB AMSICCLIB  directory for where ams libraries (amsutil.a)
#  NAGDIR            directory for NAG 

SHELL=/bin/sh

#-----------------------DEFAULTS------------------------------
### defaults can be redefined in the OS specific Makefile (Makefile.[OS])
## these defaults are for  Linux at CERN 

CERN_LEVEL = 2001
CERNDIR = /cern/$(CERN_LEVEL)
CERNSRCDIR =  /afs/cern.ch/asis/share/cern/$(CERN_LEVEL)


ifndef AMSWD
AMSWD := ..
endif
#
SCC=../CC/
# rl add SFF so it can be used in the default compile rules
SFF = ../F/

ROOTLIBS      = -L$(ROOTSYS)/lib  -lRoot 

CPPFLAGS=      -D__AMS02DAQ__    -D__WRITEROOT__ -D__ROOTPOINTERS__ 
INCPATHS= -I../include  -I$(CERNDIR)/include -I$(ROOTSYS)/include

ROOTDICT_CPPFLAGS= $(INCPATHS) 

ifdef PGTRACK
CPPFLAGS+= -D_PGTRACK_ 
ROOTDICT_CPPFLAGS+= -D_PGTRACK_ 
endif



CPPFLAGS+= $(INCPATHS)

IDLFLAGS=-D__CORBA__ -I$(BIN)

#-- CALL OS SPECIFIC FILE-------------------------------------------

# Operating system dependence.
# include a file called Makefile.$(MY_OS)
#  this file has os dependent defs

MY_OS := $(shell uname)

ifeq ($(origin local_makefile), undefined)
        local_makefile = Makefile.$(MY_OS)
endif
include $(shell echo ${local_makefile})



CFLAGSB+= $(CPPFLAGS) 
CFLAGSB+= -D__BATCH__
#FFLAGS+= $(INCPATHS)



#------------------------------------------------------------------------------

# Setup rules to compile source from $SFF or $SCC and put objects in $BIN
# default rules for object files 
# object files that have special flags are still defined below 
#
#    rules to compile o from F,f,C,c using the AMS directories
#                  $<  =  the prerequsite (the source code)
#                  $@  =  the target (the object file)

$(BIN)%.o : $(SFF)%.F
	@echo Compiling  $< ...
	@if ! [ -d $(BIN) ] ; then mkdir -p $(BIN); fi
	$(FF) -c $(FFLAGS) -I$(CERNDIR)/include $< -o $@

$(BIN)%.o : $(SFF)%.f
	@echo Compiling  $< ...
	@if ! [ -d $(BIN) ] ; then mkdir -p $(BIN); fi
	@$(FF) -c $(FFLAGS) -I$(CERNDIR)/include $< -o $@

$(BIN)%.o : $(SCC)%.C
	@echo Compiling  $< ...
	@if ! [ -d $(BIN) ] ; then mkdir -p $(BIN); fi
	$(CXX) -c $(CFLAGS) $< -o $@

$(BIN)%.o : $(SCC)%.c
	@echo Compiling  $< ...
	@if ! [ -d $(BIN) ] ; then mkdir -p $(BIN); fi
	@$(CXX) -c $(CFLAGS) $< -o $@

#------------------------------------------------------------------------------
# gbatch source files


INCS= $(wildcard ../include/*.h)    


# Base AMS Classes
AMS_BASE=  $(SCC)apool.C   $(SCC)amsdbc.C   $(SCC)amsgobj.C  \
                $(SCC)astring.C $(SCC)commonsi.C $(SCC)cont.C    $(SCC)id.C \
                $(SCC)link.C    $(SCC)node.C     $(SCC)point.C   $(SCC)snode.C \
                $(SCC)upool.C $(SCC)uzstat.C


AMS_BASEO=$(AMS_BASE:$(SCC)%.C=$(BIN)%.o)
COMMON_FILES+= $(AMS_BASE)

#Geant Interface
COMMON_FILES +=	$(SCC)commons.C    $(SCC)gvolume.C $(SCC)amsgeom.C $(SFF)Geant3_utils.F  \
		$(SCC)gmat.C       $(SCC)mceventg.C                $(SCC)mccluster.C     \
		$(SFF)egzav.F    \
		$(SFF)hadevv.F        



#Utils
COMMON_FILES += $(SCC)cern.C       $(SFF)sgif.F    $(SCC)rnormx.C  $(SFF)geocoor.F     \
		$(SFF)grfile.F   $(SCC)status.C $(SCC)user.C    $(SCC)timelx.c $(SFF)e04ccf.f

ifndef PGTRACK

COMMON_FILES +=		$(SFF)mtx2.F     $(SFF)fint_my.F 

endif


#Tracker
ifdef PGTRACK
TRACK_BASE= 	$(SCC)TkCoo.C $(SCC)TkDBc.C	\
        $(SCC)tkdcards.C $(SCC)TkLadder.C $(SCC)TkObject.C \
	$(SCC)TkPlane.C $(SCC)TkSens.C \
	$(SCC)TrCalDB.C $(SCC)TrCluster.C \
	$(SCC)TrLadCal.C  $(SCC)TrMCCluster.C \
	$(SCC)TrRawCluster.C $(SCC)patt.C \
	 $(SCC)TrLadPar.C $(SCC)TrParDB.C\
	$(SCC)VCon.C  $(SCC)TrAlignFit.C\
	$(SCC)TrPatt.C  $(SCC)TrRecon.C $(SCC)trrec.C $(SCC)TrFit.C $(SCC)MagField.C  $(SCC)TrElem.C

TRACK_ADD= $(SCC)tkgeom.C  $(SCC)trdaq.C $(SCC)VCon_gbatch.C \
	$(SCC)TrRecHit.C $(SCC)TrTrack.C $(SCC)gufld.C $(SCC)Vertex.C

else

#Tracker
TRACK_BASE += $(SCC)trid.C       $(SCC)trmccluster.C $(SCC)trrec.C   $(SCC)trcalib.C $(SCC)tralig.C  \
	 	$(SFF)tkfit.F      $(SCC)tkdbc.C   $(SFF)trafit.F  $(SFF)tcircf.F  \
		$(SFF)tszfit.F     $(SFF)txyfit.F  $(SCC)trrawcluster.C	         \
		$(SFF)tptcir.F     $(SCC)gsen.C     \
		$(SFF)rkms.F       $(SFF)tkmccl.F $(SCC)tkgeom.C $(SCC)vtx.C
TRACK_ADD= $(SFF)mfield.F $(SCC)bcorr.C
endif
COMMON_FILES +=$(TRACK_ADD)
COMMON_FILES +=$(TRACK_BASE)

TRACK_BASEO=$(TRACK_BASE:$(SCC)%.C=$(BIN)%.o)


#TRD 
COMMON_FILES += $(SCC)trdid.C      $(SCC)trdsim.C  $(SCC)trdrec.C  $(SCC)trdhrec.C  $(SCC)trddbc.C $(SCC)trdcalib.C 


#TOF + ACC
COMMON_FILES += $(SCC)tofrec02.C               $(SCC)tofuser02.C \
		$(SCC)tofsim02.C   $(SCC)tofdbc02.C   $(SCC)tofcalib02.C \
		$(SCC)tofid.C      $(SCC)antidbc02.C  $(SCC)antirec02.C \
		$(SCC)anticalib02.C 


#ECAL
COMMON_FILES += $(SCC)ecaldbc.C    $(SCC)ecid.C       $(SCC)ecalrec.C\
		$(SCC)ecalcalib.C

#RICH
COMMON_FILES += $(SCC)richdbc.C           $(SCC)richrec.C        $(SCC)richid.C      $(SCC)richgeom.C  \
		$(SFF)richpatt_lip.F   $(SFF)richrec_lip.F \
		$(SFF)richpmtlib.F        $(SFF)gtckov.F         $(SFF)graylc.F      $(SFF)cerenkov.F \
		$(SCC)richradid.C         $(SFF)ggckov.F         $(SCC)richcal.C     $(SCC)richdaq.C \
		$(SFF)richbetalike_lip.F \
		$(SFF)richbetastand_lip.F $(SFF)richchg_lip.F \
		$(SFF)richutil_lip.F

# High level reconstruction
COMMON_FILES +=        $(SCC)beta.C       $(SCC)charge.C   $(SCC)particle.C   \
		$(SCC)astro.C    $(SCC)io.C 

#Trigger
COMMON_FILES += $(SCC)trigger102.C $(SCC)trigger302.C  


#DAQ Interface
COMMON_FILES +=  $(SCC)daqs2block.C $(SCC)daqecblock.C 

#HBOOK interface
#COMMON_FILES += $(SCC)ntuple.C

#Root Interface
COMMON_FILES += $(SCC)root.C   $(SCC)rootdict.C   
#COMMON_FILES += $(SCC)root.C  

ifdef PGTRACK
ROOTDICTFILES=  ../include/TkPlane.h    ../include/TkSens.h     \
		../include/TrCalDB.h     \
		../include/patt.h       ../include/TkCoo.h  \
	        ../include/TrRecon.h 	../include/TkDBc.h      \
		../include/TrLadCal.h   ../include/VCon.h  \
		../include/TkLadder.h   ../include/TkObject.h ../include/TrLadPar.h  ../include/TrAlignFit.h \
		../include/trrec.h \
		../include/point.h  ../include/root_RV.h ../include/root.h  ../include/linkdef.h
else
ROOTDICTFILES=  ../include/point.h ../include/root_RV.h ../include/root.h ../include/amschain.h ../include/linkdef.h
endif

#------------------------------------------------------------------------------
# File list for standard gbatch compilation
#------------------------------------------------------------------------------



FILES= 	$(COMMON_FILES) \
	$(SCC)gbatch.C  $(SCC)geant.C  	$(SCC)daqevt.C $(SCC)geant3.C      \
	$(SCC)job.C     $(SCC)event.C   $(SFF)sbcomp.F $(SFF)gskpho.F  \
	$(SFF)flinit.F  $(SFF)gbirk.F   $(SFF)rndm.F   $(SCC)timeid.C $(SCC)ntuple.C \
	$(SFF)richnodisp_lip.F

ifndef PGTRACK
FILES +=$(SFF)TrackFit_utils.F
endif

MYOBJS1:=$(FILES:$(SFF)%.F=$(BIN)%.o) 
MYOBJS2:=$(MYOBJS1:$(SCC)%.C=$(BIN)%.o) 
MYOBJS:=$(MYOBJS2:$(SCC)%.c=$(BIN)%.o) 
OBJECTS:=$(MYOBJS:$(SFF)%.f=$(BIN)%.o) 

OBJECTS+= $(BIN)mtx.o

#------------------------------------------------------------------------------
# File list for interactive gbatch compilation

FILESI= $(COMMON_FILES) $(SCC)gxint.C $(SFF)gi.F $(SCC)gi2.C   $(SCC)job.C $(SCC)event.C $(SCC)timeid.C  $(SCC)ntuple.C  $(SFF)TrackFit_utils.F $(SCC)daqevt.C \
	$(SFF)richdisp_lip.F

MYOBJS1:=$(FILESI:$(SFF)%.F=$(BIN)%.o) 
MYOBJS2:=$(MYOBJS1:$(SCC)%.C=$(BIN)%.o) 
MYOBJS:=$(MYOBJS2:$(SCC)%.c=$(BIN)%.o) 
OBJECTSI:=$(MYOBJS:$(SFF)%.f=$(BIN)%.o) 

OBJECTSI+=  $(BIN)mtx.o     $(BIN)geanti.o 	$(BIN)geant3i.o  

#------------------------------------------------------------------------------
# File list for client(production) gbatch compilation

FILESC= $(COMMON_FILES)     $(SCC)producer.C $(SCC)client.C \
		$(SCC)ins_dummy.C $(SFF)sbcomp.F   $(SFF)gskpho.F   $(SFF)flinit.F $(SFF)gbirk.F $(SFF)rndm.F $(SFF)richnodisp_lip.F

MYOBJS1:=$(FILESC:$(SFF)%.F=$(BIN)%.o) 
MYOBJS2:=$(MYOBJS1:$(SCC)%.C=$(BIN)%.o) 
MYOBJS:=$(MYOBJS2:$(SCC)%.c=$(BIN)%.o) 
OBJECTSC:=$(MYOBJS:$(SFF)%.f=$(BIN)%.o) 


FILESR= $(COMMON_FILES)     $(SCC)readdaq.C
MYOBJS1:=$(FILESR:$(SFF)%.F=$(BIN)%.o) 
MYOBJS:=$(MYOBJS1:$(SCC)%.C=$(BIN)%.o) 
OBJECTR:=$(MYOBJS:$(SFF)%.f=$(BIN)%.o) 

OBJECTSC+= 	$(BIN)gbatch_c.o  $(BIN)geant_c.o  $(BIN)geant3_c.o $(BIN)job_c.o \
		$(BIN)event_c.o   $(BIN)timeid_c.o $(BIN)ntuple_c.o  $(BIN)daqevt_c.o

ifndef PGTRACK
OBJECTSC += $(BIN)TrackFit_utils_c.o
endif
OBJECTR+= 	$(BIN)geant.o  $(BIN)geant3.o $(BIN)job.o \
		$(BIN)event.o   $(BIN)timeid.o $(BIN)ntuple.o


#------------------------------------------------------------------------------
#----   Files list for the ORBiT/CORBA client/server 
#------------------------------------------------------------------------------

SRC=$(BIN)
INCLUDE=../include/

INCSCPP= $(SRC)server-cpp-stubs.h $(SRC)server-cpp-skels.h $(SRC)server-cpp-common.h

SRCS= $(SRC)server-stubs.c $(SRC)server-skels.c $(SRC)server-common.c
OBJECTCO=$(BIN)server-stubs.o $(BIN)server-skels.o $(BIN)server-common.o


SRCSCPP= $(SRC)server-cpp-stubs.cc $(SRC)server-cpp-skels.cc $(SRC)server-cpp-common.cc
OBJECTCL= $(BIN)server-cpp-stubs.o                           $(BIN)server-cpp-common.o
OBJECTSE=  $(BIN)server-cpp-skels.o   $(OBJECTCL)

OBJECTSEAUX=    $(BIN)id.o         $(BIN)node.o     $(BIN)astring.o \
		$(BIN)client.o     $(BIN)amsdbc.o   $(BIN)timeid_s.o \
		$(BIN)apool.o      $(BIN)snode.o    $(BIN)dbserver.o  

#--------------------------------------------------------------------------------------------------

SRCSMT= $(SRCMT)server-stubs.c $(SRCMT)server-skels.c $(SRCMT)server-common.c
OBJECTCOMT=$(BINMT)server-stubs.o $(BINMT)server-skels.o $(BINMT)server-common.o

SRCSCPPMT= $(SRCMT)server-cpp-stubs.cc $(SRCMT)server-cpp-skels.cc $(SRCMT)server-cpp-common.cc
OBJECTCLMT= $(BINMT)server-cpp-stubs.o                             $(BINMT)server-cpp-common.o
OBJECTSEMT=  $(BINMT)server-cpp-skels.o   $(OBJECTCLMT)

OBJECTSEAUXMT=	$(BIN)id.o 	   $(BIN)node.o      $(BIN)astring.o  \
		$(BIN)client_mt.o  $(BIN)amsdbc.o    $(BIN)timeid.o    \
		$(BIN)apool.o      $(BIN)snode.o     $(BIN)dbserver_mt.o



#------------------------------------------------------------------------------
#----   TARGETS
#------------------------------------------------------------------------------


#------------------------------------------------------------------------------
#  Gbatch standalone batch processing

gbatch: $(EXE)gbatch.exe 

$(EXE)gbatch.exe:	$(OBJECTS) $(BIN)ctype.o
	@if ! [ -d $(EXE) ] ; then mkdir -p $(EXE); fi; echo "Creating the $(EXE) directory ...."
	$(LD) $(FLAGSSTATIC) -v -o  $(EXE)gbatch.exe $(OBJECTS)\
         $(TIMEL) $(LDFLAGSSTATIC)

readdaq: $(EXE)readdaq

$(EXE)readdaq:  $(OBJECTR) $(BIN)ctype.o
	$(LD) $(FLAGSSTATIC) -v -o  $(EXE)readdaq $(OBJECTR)\
         $(TIMEL) $(LDFLAGSSTATIC)


#------------------------------------------------------------------------------
#  Gbatch standalone interactive session

gxint :	$(EXE)gxint.exe 


$(EXE)gxint.exe:	$(OBJECTSI) $(BIN)ctype.o
	@if ! [ -d $(EXE) ] ; then mkdir -p $(EXE); fi; echo "Creating the $(EXE) directory ...."
	$(LD) -v -o $(EXE)/gxint.exe  $(OBJECTSI) $(LDFLAGSI)


$(BIN)geanti.o:      $(SCC)geant.C
	$(CXX) -c  $(CPPFLAGS) $(SCC)geant.C -o $(BIN)/geanti.o

$(BIN)geant3i.o:      $(SCC)geant3.C
	$(CXX) -c  $(CPPFLAGS) $(SCC)geant3.C -o $(BIN)/geant3i.o

#------------------------------------------------------------------------------
#   Libraries to read the ROOT output

lib: shared static 
ifdef PGTRACK
shared :     $(AMS_BASEO) $(TRACK_BASEO) $(BIN)amschain_rs.o $(BIN)Vertex_s.o $(BIN)TrRecHit_s.o $(BIN)TrTrack_s.o $(BIN)rnormx_s.o $(BIN)VCon_root.o $(BIN)root_rs.o $(BIN)amschain_rs.o $(BIN)rootdict_s.o 
	@if ! [ -d $(LIB) ] ; then mkdir -p $(LIB); fi; echo "Creating the $(LIB) directory ...."
	$(LD) -O  $(SOFLAGS) $^  $(SHLDFLAGS) -o  $(SHLD)

static : $(AMS_BASEO) $(TRACK_BASEO)  $(BIN)Vertex_s.o $(BIN)TrRecHit_s.o $(BIN)TrTrack_s.o $(BIN)rnormx_s.o $(BIN)VCon_root.o $(BIN)root_rs.o $(BIN)amschain_rs.o $(BIN)rootdict_s.o 
	@if ! [ -d $(LIB) ] ; then mkdir -p $(LIB); fi; echo "Creating the $(LIB) directory ...."	
	$(AR) rv $(NTLD) $^
else

shared :     $(BIN)point.o $(BIN)amschain_rs.o	$(BIN)root_rs.o ../include/root_RV.h ../include/root.h $(BIN)rootdict_s.o $(BIN)id.o 
	@if ! [ -d $(LIB) ] ; then mkdir -p $(LIB); fi; echo "Creating the $(LIB) directory ...."
	$(AR) rv   $(NTLD) $(BIN)point.o $(BIN)/root_rs.o $(BIN)/rootdict_s.o  $(BIN)id.o
	$(LD) -O  $(SOFLAGS) $(BIN)point.o $(BIN)/root_rs.o $(BIN)/rootdict_s.o  $(BIN)amschain_rs.o  $(BIN)id.o $(SHLDFLAGS) -o  $(SHLD)

static :	$(BIN)point.o $(BIN)root_rs.o $(BIN)rootdict_s.o  $(BIN)amschain_rs.o  $(BIN)id.o 
	@if ! [ -d $(LIB) ] ; then mkdir -p $(LIB); fi; echo "Creating the $(LIB) directory ...."
	$(AR) rv   $(NTLD) $^ 

endif
#-------------------------------------------------------------------------------
# these files do not use the default rules

$(BIN)ctype.o: $(SCC)ctype.c
ifeq  "$(shell uname)"  "Linux" 
	$(CC) -c $< -o$@
else 
	cp $(SCC)ctype.c $(BIN)ctype.o
endif
$(BIN)timelx.o: $(SCC)timelx.c
	$(CC) -c $(CFLAGS) $< -o$@
../include/root_methods.h:      ../perl/root.perl
	perl ../perl/root.perl

ifdef PGTRACK
$(BIN)rootdict.C:  $(ROOTDICTFILES)
	@echo "Generating dictionary"
	$(ROOTSYS)/bin/rootcint    -f $(BIN)rootdict.C -c $(ROOTDICT_CPPFLAGS)$^ 
	@echo "OK"

$(BIN)rootdict_s.C:   ../include/VCon_root.h ../include/root_RVS.h ../include/amschain.h $(ROOTDICTFILES)
	@echo "Generating dictionary"
	$(ROOTSYS)/bin/rootcint  -f $(BIN)rootdict_s.C -c $(ROOTDICT_CPPFLAGS) $^ 
	@echo "OK"
else
$(BIN)rootdict.C:    ../include/point.h   ../include/root_RV.h ../include/root.h   ../include/linkdef.h
	@echo "Generating dictionary"
	$(ROOTSYS)/bin/rootcint    -f $(BIN)rootdict.C -c $^ 
	@echo "OK"

$(BIN)rootdict_s.C: ../include/point.h   ../include/root_RVS.h ../include/root.h ../include/amschain.h ../include/linkdef.h
	@echo "Generating dictionary"
	$(ROOTSYS)/bin/rootcint  -f $(BIN)rootdict_s.C -c $^ 
	@echo "OK"

endif


$(BIN)rootdict.o:         $(BIN)rootdict.C
	$(CXX) -c $(CFLAGS) $(BIN)rootdict.C -o $(BIN)rootdict.o 

$(BIN)daqevt.o:         $(SCC)daqevt.C
	$(CXX) -c $(CFLAGSDAQ) $(SCC)daqevt.C -o $(BIN)daqevt.o 

$(BIN)daqevt_c.o:         $(SCC)daqevt.C
	$(CXX_ORBIT) -c $(CFLAGSDAQ) $(IDLFLAGS) $(SCC)daqevt.C -o $(BIN)daqevt_c.o 

$(BIN)rootdict_s.o:         $(BIN)rootdict_s.C
	$(CXX) -c $(CFLAGS)  $(BIN)rootdict_s.C -o $(BIN)rootdict_s.o 

$(BIN)rnormx_s.o:         $(SCC)rnormx.C
	$(CXX) -c $(CFLAGS) -D__ROOTSHAREDLIBRARY__ $^ -o $@

$(BIN)Vertex_s.o:         $(SCC)Vertex.C
	$(CXX) -c $(CFLAGS) -D__ROOTSHAREDLIBRARY__ $^ -o $@

$(BIN)TrTrack_s.o:         $(SCC)TrTrack.C
	$(CXX) -c $(CFLAGS) -D__ROOTSHAREDLIBRARY__ $^ -o $@

$(BIN)TrRecHit_s.o:         $(SCC)TrRecHit.C
	$(CXX) -c $(CFLAGS) -D__ROOTSHAREDLIBRARY__ $^ -o $@

$(BIN)root_rs.o:    	 $(SCC)root.C ../include/root_methods.h
	$(CXX) -c $(CFLAGS)  -D__ROOTSHAREDLIBRARY__ $(SCC)root.C -o $(BIN)root_rs.o

$(BIN)amschain_rs.o:    	 $(SCC)amschain.C 
	$(CXX) -c $(CFLAGS)  -D__ROOTSHAREDLIBRARY__ $(SCC)amschain.C -o $(BIN)amschain_rs.o

$(BIN)ntuple.o:    	 $(SCC)ntuple.C

ifdef AMSMEMMONITOR
	$(CXX) -c -D__MEMMONITOR__ $(CFLAGS) $(SCC)ntuple.C -o $(BIN)ntuple.o
else
	$(CXX) -c $(CFLAGS) $(SCC)ntuple.C -o $(BIN)ntuple.o
endif

$(BIN)ntuple_c.o:   $(SCC)ntuple.C
	$(CXX_ORBIT) -c $(CFLAGS) $(IDLFLAGS) $(SCC)ntuple.C -o $(BIN)ntuple_c.o                                                                                  
$(BIN)TrackFit_utils_c.o:   $(SFF)TrackFit_utils.F
	$(FF) -c $(FFLAGS) $(IDLFLAGS) $(SFF)TrackFit_utils.F -o $(BIN)TrackFit_utils_c.o                                                                                
ifeq  "$(shell uname)"  "SunOS" 

$(BIN)mtx.o:                   $(SFF)mtx77.F
	f77 -c $(FFLAGS)      $(SFF)mtx77.F -o $(BIN)mtx.o
endif
ifeq  "$(shell uname)"  "HP-UX" 

$(BIN)mtx.o:                   $(SFF)mtx77.F
	f77 -c $(FFLAGS)      $(SFF)mtx77.F -o $(BIN)mtx.o
endif
ifeq  "$(shell uname)"  "Linux" 

$(BIN)mtx.o:                   $(SFF)mtx77.F
	$(FF) -c $(FFLAGS)      $(SFF)mtx77.F -o $(BIN)mtx.o
endif
ifeq  "$(shell uname)"  "AIX" 

$(BIN)mtx.o:                   $(SFF)mtx.F
	xlf90 -c $(FFLAGS)      $(SFF)mtx.F -o $(BIN)mtx.o
endif
ifeq  "$(shell uname)"  "AIXold" 

$(BIN)mtx.o:                   $(SFF)mtx.F
	xlf90 -c $(FFLAGS)      $(SFF)mtx.F -o $(BIN)mtx.o
endif

ifeq  "$(shell uname)"  "OSF1" 

$(BIN)mtx.o:                   $(SFF)mtx.F
	f90 -c -free $(FFLAGS)      $(SFF)mtx.F -o $(BIN)mtx.o
endif

#===============================================================================
#  Rules to customize  and compile some  GEANT3 files
#===============================================================================

$(SFF)flinit.F:      $(CERNSRCDIR)/src/geant321/fiface/flinit.F
	sed  "s/      OPEN(UNIT=LUNERR,FILE='flukaerr.dat',STATUS='UNKNOWN')/#if defined __IFC || defined __INTEL_COMPILER\n       OPEN(UNIT=LUNERR,FILE='\/dev\/null',shared,STATUS='UNKNOWN')\n#else\n       OPEN(UNIT=LUNERR,FILE='\/dev\/null',STATUS='UNKNOWN')\n#endif/g"  $(CERNSRCDIR)/src/geant321/fiface/flinit.F > $(SFF)flinit.F


$(SFF)gbirk.F:      $(CERNSRCDIR)/src/geant321/gphys/gbirk.F
	sed  "s/  99  RETURN/             IF(ABS(CHARGE).GE.2..AND.MODEL.EQ.1)C=0\n      IF(ABS(CHARGE).GE.2)THEN\n       GAMASS=GETOT+AMASS\n       BET2=GEKIN*GAMASS\/(GETOT*GETOT)\n        BET=SQRT(BET2)\n       W1=1.034-0.1777*EXP(-0.08114*CHARGE)\n       W2=BET\/ABS(CHARGE)**(2.\/3.)\n       W3=121.4139*W2+0.0378*SIN(190.7165*W2)\n       CHARG1=CHARGE*(1.-W1*EXP(-W3))\n       IF(CHARG1.LT.0)CHARG1=1\n       CHARG2=CHARG1**2\n       DEDXCM=DEDXCM*CHARG2\n      ENDIF\n       IF(MODEL.EQ.0)THEN\n        C=C*DENS**2     \n        RKB=RKB*DENS \n       EDEP=DESTEP\/(1.+C*ATAN(RKB\/C*DEDXCM))\n      ELSE\n       EDEP=DESTEP\/(1.+RKB*DEDXCM+C*DEDXCM**2)\n      ENDIF\n  99  RETURN/g"  $(CERNSRCDIR)/src/geant321/gphys/gbirk.F > $(SFF)gbirk.F


$(SFF)grfile.F:      $(CERNSRCDIR)/src/geant321/giopa/grfile.F
	sed  "s/NQUOT=1000/NQUOT=32000/g"  /$(CERNSRCDIR)/src/geant321/giopa/grfile.F > $(SFF)grfile.F

$(BIN)grfile.o:      $(SFF)grfile.F
	$(FF) -c $(FFLAGS) -I$(CERNDIR)/include $(SFF)grfile.F -o $(BIN)grfile.o

$(SFF)gskpho.F:      $(CERNSRCDIR)/src/geant321/gtrak/gskpho.F
	sed  "s/N2-N1/NGPHOT\*10/g;s/MISSNG.GT.0/MISSNG.GT.NGPHOT\*10/g;s/MSSPOL.GT.0/MSSPOL.GT.NGPHOT\*10/g"  $(CERNSRCDIR)/src/geant321/gtrak/gskpho.F > $(SFF)gskpho.F

$(BIN)gskpho.o:      $(SFF)gskpho.F
	$(FF) -c $(FFLAGS) -I$(CERNDIR)/include $(SFF)gskpho.F -o $(BIN)gskpho.o

$(SFF)hadevv.F:      $(CERNSRCDIR)/src/geant321/fluka/hadevv.F
	sed  "0,/AMCH2 .LT. AMB8/ s/AMCH2 .LT. AMB8/AMCH2 .LT. AMB8 .AND. AMB8 .LT. UMO*0.5/"  $(CERNSRCDIR)/src/geant321/fluka/hadevv.F > $(SFF)hadevv.F.tmp
	sed  "s/ACH1.LE./ACH1.LT./;s/ACH2.LE./ACH2.LT./"  $(SFF)hadevv.F.tmp > $(SFF)hadevv.F
	rm -rf $(SFF)hadevv.F.tmp 

$(BIN)hadevv.o:      $(SFF)hadevv.F
	$(FFOLD) -c $(FFLAGS) -DCERNLIB_DOUBLE -I$(CERNDIR)/include $(SFF)hadevv.F -o $(BIN)hadevv.o

$(SFF)sbcomp.F:      $(CERNSRCDIR)/src/geant321/peanut/sbcomp.F
	sed  "s/STOP 'BIMPCT'/write(*,*)'****SBCOMP BIMPCT STOP ****',R0TRAJ,BIMPCT,R1TRAJ/g"  $(CERNSRCDIR)/src/geant321/peanut/sbcomp.F > $(SFF)sbcomp.F

$(BIN)sbcomp.o:      $(SFF)sbcomp.F
	$(FFOLD) -c $(FFLAGS) -I$(CERNDIR)/include -DCERNLIB_DOUBLE $(SFF)sbcomp.F -o $(BIN)sbcomp.o


$(BIN)flinit.o:      $(SFF)flinit.F
	$(FFOLD) -c $(FFLAGS) -DCERNLIB_UNIX -I$(CERNDIR)/include $(SFF)flinit.F -o $(BIN)flinit.o

$(BIN)gbirk.o:      $(SFF)gbirk.F
	$(FFOLD) -c $(FFLAGS) -DCERNLIB_UNIX -I$(CERNDIR)/include $(SFF)gbirk.F -o $(BIN)gbirk.o

$(BIN)richpmtlibo.o:	$(SFF)richpmtlib.F
	$(FF) -c $(FFLAGS) -DCERNLIB_UNIX -I$(CERNDIR)/include $(SFF)richpmtlib.F -o $(BIN)richpmtlib.o

#---------------------------------------------------------------------------------
#  Server TARGET (server)

SERVER=$(EXE)amsprodserver.exe

server:	$(SERVER)

$(SERVER):          $(OBJECTCO) $(OBJECTSE) $(OBJECTSEAUX) $(BIN)server.o $(BIN)ctype.o
	@if ! [ -d $(EXE) ] ; then mkdir -p $(EXE); fi; echo "Creating the $(EXE) directory ...."
	$(LD) -v -o $(SERVER) $(BIN)server.o  $(OBJECTSE) $(OBJECTSEAUX) $(OBJECTCO)  $(LDFLAGSSTATICIDL) 



#---------------------------------------------------------------------------------
# Client TARGET (producer)

CLIENT=$(EXE)gbatch-orbit.exe

producer:	$(CLIENT)


$(CLIENT):          $(OBJECTCO) $(OBJECTCL) $(OBJECTSC) $(BIN)ctype.o
	@if ! [ -d $(EXE) ] ; then mkdir -p $(EXE); fi; echo "Creating the $(EXE) directory ...."
	$(LD) $(FLAGSSTATIC) -v -o $(CLIENT) $(OBJECTSC)  $(OBJECTCL) $(OBJECTCO) $(LDFLAGSSTATICIDL) $(TIMEL) $(LDFLAGSSTATIC)

#---------------------------------------------------------------------------------
# Additional Rules for client/server 

$(OBJECTSE):         $(SRCSCPP)
	$(CXX_ORBIT)  -c  $(SRCSCPP) 
	mv *.o $(BIN)

$(OBJECTCO):         $(SRCS)
	$(CC_ORBIT)  -c  $(SRCS) 
	mv *.o $(BIN)


$(SRCS):    	 $(INCLUDE)server.idl
	$(IDL) $(INCLUDE)server.idl
	mv *.h $(SRC)
	mv *.c $(SRC)



$(SRCSCPP):   $(SRCS) 	 $(INCLUDE)server.idl
	$(IDLCPP) $(INCLUDE)server.idl
	mv *.h $(SRC)
	mv *.cc $(SRC)


$(BIN)gbatch_c.o:         $(SCC)gbatch.C
	$(CXX_ORBIT) -c $(CFLAGS) -D__ROOTA__ $(SCC)gbatch.C $(IDLFLAGS)  -o $(BIN)gbatch_c.o

$(BIN)geant_c.o: 	 $(SCC)geant.C
	$(CXX_ORBIT) -c $(CFLAGSB) $(IDLFLAGS) $(SCC)geant.C -o $(BIN)geant_c.o

$(BIN)job_c.o: 	 $(SCC)job.C
	$(CXX_ORBIT) -c $(CFLAGS) $(IDLFLAGS) $(SCC)job.C -o $(BIN)job_c.o

$(BIN)geant3_c.o: 	 $(SCC)geant3.C
	$(CXX_ORBIT) -c $(CFLAGS) $(IDLFLAGS) $(SCC)geant3.C -o $(BIN)geant3_c.o

$(BIN)event_c.o: 	 $(SCC)event.C
	$(CXX_ORBIT) -c $(CFLAGS) $(IDLFLAGS) $(SCC)event.C -o $(BIN)event_c.o

$(BIN)timeid_c.o:        $(SCC)timeid.C
	$(CXX_ORBIT) -c $(CFLAGS) $(IDLFLAGS) $(SCC)timeid.C -o $(BIN)timeid_c.o

$(BIN)timeid_s.o:        $(SCC)timeid.C
	$(CXX_ORBIT) -c $(CFLAGS) -D__CORBASERVER__ $(SCC)timeid.C -o $(BIN)timeid_s.o

$(BIN)producer.o: 	 $(SCC)producer.C
	$(CXX_ORBIT) -c -w $(CFLAGSB) $(IDLFLAGS) $(SCC)producer.C -o $(BIN)producer.o

$(BIN)client.o: 	 $(SCC)client.C
	$(CXX_ORBIT) -fpermissive -c -w $(CFLAGS) $(IDLFLAGS) $(SCC)client.C -o $(BIN)client.o

$(BIN)server.o: 	 $(SCC)server.C
	$(CXX_ORBIT)  -c -w $(CFLAGS) $(IDLFLAGS) $(SCC)server.C -o $(BIN)server.o

$(BIN)dbserver.o: 	 $(SCC)dbserver.C
	$(CXX_ORBIT)  -c -w $(CFLAGS) $(IDLFLAGS) $(SCC)dbserver.C -o $(BIN)dbserver.o



depend:         $(BIN)Make-depend

touch_depend:
		touch $(BIN)Make-depend

$(BIN)Make-depend:    $(INCS)
	@if ! [ -d $(BIN) ] ; then mkdir -p $(BIN); fi
ifneq "$(shell du -a  $(BIN)Make-depend | grep -c  ake)" "1"
		touch $(BIN)Make-depend
endif
		makedepend -f $(BIN)Make-depend  -Y../include  $(FILES) -p$(BIN)
		cat  $(BIN)Make-depend | sed -e 'sx/../CC/x/x1' > $(BIN)Make-depend.new
		rm -f $(BIN)Make-depend
		mv $(BIN)Make-depend.new $(BIN)Make-depend

clean : 
	rm -f $(BIN)*.o 
	rm -f $(BIN)Make-depend 
	rm -f $(BIN)*.C
	rm -f $(BIN)*.c
	rm -f $(BIN)*.cc
	rm -f $(BIN)*.h
	rm -f $(EXE)*.exe

cleanlibs:
	rm -f $(BIN)/root*.o
	rm -f $(LIB)/*.so
	rm -f $(LIB)/*.a
	cd ../ruby; gmake clean
	cd ../python; gmake clean
	cd ../install

##### DEPENDENCIES #####
ifneq ($(MAKECMDGOALS),touch_depend)
 include $(BIN)Make-depend
endif


