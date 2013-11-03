#  $Id: Makefile,v 1.273 2013/11/03 03:00:10 pzuccon Exp $

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

# List of SLC6 changes
#
SLC6system=
ifneq "$(wildcard /etc/redhat-release)" ""
 SLC6system := $(shell cat /etc/redhat-release | grep "Scientific Linux CERN SLC release 6")
endif
#

SHELL=/bin/sh

#-----------------------DEFAULTS------------------------------
### defaults can be redefined in the OS specific Makefile (Makefile.[OS])
## these defaults are for  Linux at CERN 
ifeq ($(wildcard $(ROOTSYS)/include/Compression.h),) 
    LZMAL =
else 
    DEFINES  +=  -D__LZMA__ 
    LZMAL = -llzma  
endif


ifndef AMSWD
AMSWD := ..
endif
#
ifndef AMSDEPLIB
AMSDEPLIB := /afs/cern.ch/ams/local2/opt/
endif
ifndef VISLIBS
VISLIBS:=
endif
ifndef UILIBS
UILIBS:=
endif
G4LIBS=
G4LIBSI=
G4=
ifdef G4AMS
G4VIS_USE:= 1
G4:=G4
ifdef G4VIS_USE
include $(G4INSTALL)/config/G4UI_USE.gmk
include $(G4INSTALL)/config/G4VIS_USE.gmk
endif
include $(G4INSTALL)/config/architecture.gmk
DEFINES  += -D__NAMESPACE__ -D__G4AMS__ 
INCDIRS  += -I$(CLHEP_BASE_DIR)/include -I$(G4INSTALL)/include/geant4/ 
ifdef CLHEP_BASE_DIR
G4LIBS= -L$(CLHEP_BASE_DIR)/lib -L$(G4INSTALL)/lib/geant4/$(G4SYSTEM) -lG4error_propagation -lG4readout -lG4physicslists -lG4run -lG4event -lG4tracking -lG4parmodels -lG4processes -lG4digits_hits -lG4track -lG4particles -lG4geometry -lG4materials -lG4graphics_reps -lG4intercoms -lG4global -lCLHEP
else
G4LIBS=  -L$(G4INSTALL)/lib/geant4/$(G4SYSTEM) -lG4error_propagation -lG4readout -lG4physicslists -lG4run -lG4event -lG4tracking -lG4parmodels -lG4processes -lG4digits_hits -lG4track -lG4particles -lG4geometry -lG4materials -lG4graphics_reps -lG4intercoms -lG4global 
endif
G4LIBSI= -L$(G4INSTALL)/lib/geant4/$(G4SYSTEM) $(VISLIBS) $(G4LIBS) -lG4interfaces -lGL -lGLU
AR=ar
endif

############## PRE PROCESSOR FLAGS (non ARCH dependent) 
ifdef ECALBDT
 DEFINES  += -D_ECALBDT_
endif
ifdef ECALBDTCHI2
 DEFINES  += -D_ECALBDTCHI2_
endif
ifdef CASTORSTATIC 
DEFINES += -DCASTORSTATIC
endif

DEFINES  +=    -D__WRITEROOT__    
INCDIRS  += -I../include  -I$(CERNDIR)/include_dummy -I$(ROOTSYS)/include  


CPPFLAGS += $(DEFINES)  $(INCDIRS)

ROOTDICT_CPPFLAGS= $(CPPFLAGS) 


################  OUTPUT DIRS  
# C++      sources dir 
SCC = ../CC/
# Fortran  sources dir 
SFF = ../F/
ifdef SLC6system
AMSLIBSUFFIX=slc6
else
AMSLIBSUFFIX=slc4
endif

ifdef PGTRACK
PG:=PG
DEFINES+= -D_PGTRACK_ 
ROOTDICT_CPPFLAGS+= -D_PGTRACK_ 

# Uncomment here if you have lib/libMinuit2.so and its objs in lib/libRoot.a
# MINUIT2=1

ifdef MINUIT2
DEFINES+= -D_MINUIT2_
ROOTDICT_CPPFLAGS+= -D_MINUIT2_
endif

BINMT=$(AMSWD)/bin/$(MARCH)$(VERSIONP)/mt_PG$(G4)/
BIN=$(AMSWD)/bin/$(MARCH)$(VERSIONP)/PG$(G4)/
EXE=$(AMSWD)/exe/$(MARCH)$(VERSIONP)/
LIB=$(AMSWD)/lib/$(MARCH)$(VERSIONP)/
SHLD=$(LIB)/ntuple_$(AMSLIBSUFFIX)_PG.so
NTLD=$(LIB)/libntuple_$(AMSLIBSUFFIX)_PG.a
else
PG=
BINMT=$(AMSWD)/bin/$(MARCH)$(VERSIONP)/mt/$(G4)/
BIN=$(AMSWD)/bin/$(MARCH)$(VERSIONP)/$(G4)/
EXE=$(AMSWD)/exe/$(MARCH)$(VERSIONP)/
LIB=$(AMSWD)/lib/$(MARCH)$(VERSIONP)/
SHLD=$(LIB)/ntuple_$(AMSLIBSUFFIX).so
NTLD=$(LIB)/libntuple_$(AMSLIBSUFFIX).a
endif
#----Qi Yan
ifdef G4AMS
DPMDIR=../DPMJET/
DPMBIN=$(BIN)/DPMJET/
DPMSCC=$(DPMDIR)/src/
DPMFF= $(DPMDIR)/dpmjet2_5/

DEFINES +=-DG4_USE_DPMJET \
        -DG4DPMJET25GDSHDEBU
INCDIRS +=-I$(DPMDIR)/include
endif
#-----

IDLFLAGS=-D__CORBA__ -I$(BIN)

#-- CALL OS SPECIFIC FILE-------------------------------------------

# Operating system dependence.
# include a file called Makefile.$(ARCH)
# this file has os dependent defs
# ARCH comes from the ROOT arch 


ifeq ($(origin MARCH), undefined) 
  MARCH   := $(shell $(ROOTSYS)/bin/root-config --arch)
endif
VERSION      := $(shell $(ROOTSYS)/bin/root-config --version | cut -b1-4)
ifeq ($(VERSION),5.27)
VERSIONP=
else
VERSIONP = $(VERSION)
endif
local_makefile = Makefile.$(MARCH)
include $(local_makefile)

# combine compiler flags with pre-processor flags
CPPFLAGSI := $(CPPFLAGS)
CFLAGSI   := $(CFLAGSB) $(CPPFLAGSI) 

CFLAGSB+= $(CPPFLAGS) 


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


$(BIN)%_s.o : $(SFF)%.F
	@echo Compiling  $< ...
	@if ! [ -d $(BIN) ] ; then mkdir -p $(BIN); fi
	$(FF) -c $(FFLAGS) -D__ROOTSHAREDLIBRARY__  -I$(CERNDIR)/include $< -o $@


$(BIN)%.o : $(SFF)%.f
	@echo Compiling  $< ...
	@if ! [ -d $(BIN) ] ; then mkdir -p $(BIN); fi
	$(FF) -c $(FFLAGS) -I$(CERNDIR)/include $< -o $@


$(BIN)%.o : $(SCC)%.C
	@echo Compiling  $< ...
	@if ! [ -d $(BIN) ] ; then mkdir -p $(BIN); fi
	$(CXX) -c $(CFLAGS) $< -o $@


$(BIN)%_s.o: $(SCC)%.C ../include/root_methods$(PG).h
	@echo Compiling  $< ...
	@if ! [ -d $(BIN) ] ; then mkdir -p $(BIN); fi
	$(CXX) -c $(CFLAGS) -D__ROOTSHAREDLIBRARY__   $<  -o $@


$(BIN)%.o : $(SCC)%.c
	@echo Compiling  $< ...
	@if ! [ -d $(BIN) ] ; then mkdir -p $(BIN); fi
	$(CC) -c $(CFLAGS) $< -o $@

#------------------------------------------------------------------------------
# gbatch source files


INCS= $(wildcard ../include/*.h)    


# Base AMS Classes
AMS_BASE=     $(SCC)amsdbc.C  $(SCC)astring.C     $(SCC)id.C  $(SCC)point.C  $(SCC)edep.C 

AMS_BASEO=$(AMS_BASE:$(SCC)%.C=$(BIN)%.o)
COMMON_FILES+= $(AMS_BASE) $(BIN)predict.o

# Base GBATCH Classes
GBATCH_BASE = $(SCC)amsgobj.C $(SCC)link.C $(SCC)commonsi.C   $(SCC)node.C   $(SCC)snode.C \
	      $(SCC)cont.C  $(SCC)upool.C $(SCC)uzstat.C $(SCC)apool.C $(SCC)ams2tgeo.C  

COMMON_FILES+=$(GBATCH_BASE)


ifdef G4AMS
#Geant 4 interface

COMMON_FILES +=   $(SCC)TRD_SimUtil.C  $(SCC)g4physics_ion.C \
	$(SCC)TRD_GammaXTRadiator.C $(SCC)TRD_VXTenergyLoss.C $(SCC)geant4.C \
	$(SCC)g4physics.C $(SCC)g4util.C $(SCC)g4xray.C  $(SCC)AMSPhysicsList_HadronIon.C $(SCC)g4strangelet.C	

COMMON_FILES += $(SCC)g4rich.C $(SCC)g4tof.C $(SCC)TofSimUtil.C

else
#Geant3/AMS custom  files
COMMON_FILES += $(SFF)sbcomp.F  $(SFF)gskpho.F  $(SFF)flinit.F  $(SFF)gbirk.F $(SFF)hadevv.F $(SFF)grfile.F  $(SFF)gstord.F
endif

#Geant3 Interface
COMMON_FILES +=	$(SCC)gvolume.C $(SCC)amsgeom.C   $(SFF)Geant3_utils.F  $(SFF)trd.F \
		$(SCC)gmat.C    $(SCC)mceventg.C  $(SCC)mccluster.C     \
		$(SFF)egzav.F    $(SFF)rzerowrapper.F 


#Utils
COMMON_FILES +=  $(SCC)commons.C  $(SCC)cern.C    $(SFF)sgif.F   $(SCC)rnormx.C  $(SFF)geocoor.F     \
		 $(SCC)status.C   $(SCC)user.C    $(SCC)timelx.c $(SFF)e04ccf.f	 
#

ifndef PGTRACK
COMMON_FILES += $(SFF)mtx2.F     $(SFF)fint_my.F 
endif


#Tracker
ifdef PGTRACK
TRACK_BASE= $(SCC)TkCoo.C 	\
        $(SCC)tkdcards.C     $(SCC)TkLadder.C $(SCC)TkObject.C \
	$(SCC)TkPlane.C      $(SCC)TkSens.C \
	$(SCC)TrCluster.C \
	$(SCC)TrLadCal.C     $(SCC)TrMCCluster.C \
	$(SCC)TrRecHit.C     $(SCC)Vertex.C \
	$(SCC)TrRawCluster.C $(SCC)tkpatt.C  $(SCC)patt.C \
        $(SCC)TrLadPar.C     $(SCC)TrParDB.C \
	$(SCC)TrTasCluster.C $(SCC)TrTasPar.C \
	$(SCC)TrAlignFit.C \
	$(SCC)TrHistoMan.C   $(SCC)HistoMan.C $(SCC)TrTrackSelection.C \
	$(SCC)TrkLH.C \
        $(SCC)TrPdf.C        $(SCC)TrPdfDB.C  $(SCC)TrCharge.C $(SCC)TrInnerDzDB.C

TRACK_ADD= $(SCC)tkgeom.C   $(SCC)trdaq.C   $(SCC)TkDBc.C $(SCC)TrTrack.C  \
	   $(SCC)trrec.C    $(SCC)MagField.C $(SCC)TrFit.C $(SCC)TrRecon.C $(SCC)TrSim.C $(SCC)TrSimSensor.C $(SCC)TrSimCluster.C \
	   $(SCC)bcorr.C    $(SFF)fint_my.F $(SCC)TrCalDB.C $(SCC)TrTasDB.C $(SCC)TrGainDB.C $(SCC)TrChargeLossDB.C $(SCC)TrOccDB.C \
	   $(SCC)TrEDepDB.C $(SCC)TrMipDB.C $(SCC)TrLinearDB.C $(SCC)TrLikeDB.C \
           $(SCC)gufld.C $(SFF)rkms.F $(SCC)TrExtAlignDB.C \
	   $(SCC)DynAlignment.C 

TRACK_ADD+=$(SCC)OrbGen.C $(SCC)pfunction.C $(SCC)cgmmap.C
else

#Tracker
TRACK_BASE +=   $(SCC)trid.C       $(SCC)trmccluster.C $(SCC)trrec.C  $(SCC)trcalib.C $(SCC)tralig.C  \
	 	$(SFF)tkfit.F      $(SCC)tkdbc.C   $(SFF)trafit.F  $(SFF)tcircf.F  \
		$(SFF)tszfit.F     $(SCC)trfit.C $(SFF)txyfit.F  $(SCC)trrawcluster.C	         \
		$(SFF)tptcir.F     $(SCC)gsen.C    $(SCC)TrPdf.C \
		$(SFF)rkms.F       $(SFF)tkmccl.F $(SCC)tkgeom.C $(SCC)vtx.C
TRACK_ADD=      $(SFF)mfield.F     $(SCC)bcorr.C  $(SCC)DynAlignment.C
endif
COMMON_FILES +=$(TRACK_ADD) 
COMMON_FILES +=$(TRACK_BASE)

TRACK_BASEO=$(TRACK_BASE:$(SCC)%.C=$(BIN)%.o)

COMMON_FILES += $(SCC)VCon_gbatch.C $(SCC)TrElem.C $(SCC)VCon.C 

#TRD 
COMMON_FILES += $(SCC)TrdMcDatacards.C $(SCC)trdid.C $(SCC)trdsim.C $(SCC)trdrec.C $(SCC)TrdHRecon.C $(SCC)TrdHCalib.C $(SCC)TrdHCharge.C $(SCC)TrdTFit.C $(SCC)TrdHSegment.C $(SCC)TrdHTrack.C $(SCC)trdhrec.C  $(SCC)trddbc.C $(SCC)trdcalib.C  

#TRD + AC Calibration + TrdKLikelihood Methods + TrdMTrack
ifdef PGTRACK
TRD_ADD= $(SCC)TrdSCalib.C $(SCC)TrdKCalib.C $(SCC)TrdKHit.C $(SCC)TrdMTrack.C $(SCC)TrdKPDF.C $(SCC)TrdKCluster.C $(SCC)TrdKToyMC.C  
TOF_ADD= $(SCC)TofGeometry.C $(SCC)TofRecon.C $(SCC)TofTrack.C
endif
COMMON_FILES +=$(TRD_ADD) 
COMMON_FILES +=$(TOF_ADD)

#ACsoft
ifdef AMS_ACQT_INTERFACE
DEFINES += -DAMS_ACQT_INTERFACE -DDISABLE_ACQT_CONTROL_PLOTS -DAMS_ACQT_SILENCE_COMMON_WARNINGS \
           -DACPASSTAG=5.8.0 -DACROOTVERSION=58 -DACPRODUCTIONSHA=0a9936b833771fb57f727286dc0a8d9b7479af40

ACSOFT_INCDIRS := -I$(QTDIR_STATIC)/Headers -I$(QTDIR_STATIC)/include -I$(QTDIR_STATIC)/include/QtCore -I../ACsoft \
                  -I../ACsoft/acroot -I../ACsoft/cuts -I../ACsoft/platform -I../ACsoft/utilities -I../ACsoft/analysis \
                  -I../ACsoft/producer -I../ACsoft/interface -I../ACsoft/io -I../ACsoft/detector -I../ACsoft/trdcalibration
ROOTDICT_CPPFLAGS += -DAMS_ACQT_INTERFACE $(ACSOFT_INCDIRS)

ROOTDICTFILES += ../ACsoft/acroot/AMSGeometry.h \
                 ../ACsoft/acroot/Database.h \
                 ../ACsoft/acroot/MagnetTemperatures.h \
                 ../ACsoft/acroot/Settings.h \
                 ../ACsoft/acroot/TrackerExternalAlignment.h \
                 ../ACsoft/acroot/TRDCalibration.h \
                 ../ACsoft/acroot/TRDSensorData.h \
                 ../ACsoft/acroot/TRDTubeCalibration.h \
                 ../ACsoft/acroot/TRDModuleCalibration.h \
                 ../ACsoft/analysis/TrdPdfHistogram.hh \
                 ../ACsoft/analysis/BadRunManager.hh \
                 ../ACsoft/cuts/BadRunCuts.hh \
                 ../ACsoft/cuts/Cut.hh \
                 ../ACsoft/cuts/TwoSidedCut.hh \
                 ../ACsoft/cuts/ManualCut.hh \
                 ../ACsoft/cuts/ManualNMinusOneCut.hh \
                 ../ACsoft/cuts/NMinusOneCut.hh \
                 ../ACsoft/cuts/PreselectionCuts.hh \
                 ../ACsoft/cuts/ECALCuts.hh \
                 ../ACsoft/cuts/GroupACuts.hh \
                 ../ACsoft/cuts/RICHCuts.hh \
                 ../ACsoft/cuts/RTICuts.hh \
                 ../ACsoft/cuts/TOFCuts.hh \
                 ../ACsoft/cuts/TriggerCuts.hh \
                 ../ACsoft/cuts/TrackerCuts.hh \
                 ../ACsoft/cuts/TRDCuts.hh \
                 ../ACsoft/cuts/ParticleSelectionCuts.hh \
                 ../ACsoft/cuts/Selector.hh \
                 ../ACsoft/io/ReducedFileHeader.hh \
                 ../ACsoft/utilities/Palettes.hh \
                 ../ACsoft/utilities/TrackfindingHistogram.hh

ACSOFT_FILES = ../ACsoft/acroot/ACC.C \
               ../ACsoft/acroot/ACCCluster.C \
               ../ACsoft/acroot/ACCCluster-Streamer.C \
               ../ACsoft/acroot/ACCRawSide.C \
               ../ACsoft/acroot/ACCRawSide-Streamer.C \
               ../ACsoft/acroot/ACC-Streamer.C \
               ../ACsoft/acroot/AMSGeometry.C \
               ../ACsoft/acroot/DAQ.C \
               ../ACsoft/acroot/DAQ-Streamer.C \
               ../ACsoft/acroot/ECAL.C \
               ../ACsoft/acroot/ECALShower.C \
               ../ACsoft/acroot/ECALShower-Streamer.C \
               ../ACsoft/acroot/ECAL-Streamer.C \
               ../ACsoft/acroot/Event.C \
               ../ACsoft/acroot/EventHeader.C \
               ../ACsoft/acroot/EventHeader-Streamer.C \
               ../ACsoft/acroot/Event-Streamer.C \
               ../ACsoft/acroot/LookupFileValidation.C \
               ../ACsoft/acroot/MC.C \
               ../ACsoft/acroot/MCEventGenerator.C \
               ../ACsoft/acroot/MCEventGenerator-Streamer.C \
               ../ACsoft/acroot/MC-Streamer.C \
               ../ACsoft/acroot/MCTrack.C \
               ../ACsoft/acroot/MCTrack-Streamer.C \
               ../ACsoft/acroot/MCTRDCluster.C \
               ../ACsoft/acroot/MCTRDCluster-Streamer.C \
               ../ACsoft/acroot/MCTrackerCluster.C \
               ../ACsoft/acroot/MCTrackerCluster-Streamer.C \
               ../ACsoft/acroot/Particle.C \
               ../ACsoft/acroot/Particle-Streamer.C \
               ../ACsoft/acroot/RICH.C \
               ../ACsoft/acroot/RICHRing.C \
               ../ACsoft/acroot/RICHRing-Streamer.C \
               ../ACsoft/acroot/RICH-Streamer.C \
               ../ACsoft/acroot/RunHeader.C \
               ../ACsoft/acroot/RunHeader-Streamer.C \
               ../ACsoft/acroot/Settings.C \
               ../ACsoft/acroot/TOFBeta.C \
               ../ACsoft/acroot/TOFBeta-Streamer.C \
               ../ACsoft/acroot/TOF.C \
               ../ACsoft/acroot/TOFCluster.C \
               ../ACsoft/acroot/TOFCluster-Streamer.C \
               ../ACsoft/acroot/TOF-Streamer.C \
               ../ACsoft/acroot/Tracker.C \
               ../ACsoft/acroot/TrackerExternalAlignment.C \
               ../ACsoft/acroot/TrackerReconstructedHit.C \
               ../ACsoft/acroot/TrackerReconstructedHit-Streamer.C \
               ../ACsoft/acroot/Tracker-Streamer.C \
               ../ACsoft/acroot/TrackerTrack.C \
               ../ACsoft/acroot/TrackerTrackFit.C \
               ../ACsoft/acroot/TrackerTrackFit-Streamer.C \
               ../ACsoft/acroot/TrackerTrack-Streamer.C \
               ../ACsoft/acroot/TRD.C \
               ../ACsoft/acroot/TRDHSegment.C \
               ../ACsoft/acroot/TRDHSegment-Streamer.C \
               ../ACsoft/acroot/TRDHTrack.C \
               ../ACsoft/acroot/TRDHTrack-Streamer.C \
               ../ACsoft/acroot/TRDRawHit.C \
               ../ACsoft/acroot/TRDRawHit-Streamer.C \
               ../ACsoft/acroot/TRD-Streamer.C \
               ../ACsoft/acroot/TRDVTrack.C \
               ../ACsoft/acroot/TRDVTrack-Streamer.C \
               ../ACsoft/acroot/Trigger.C \
               ../ACsoft/acroot/Trigger-Streamer.C \
               ../ACsoft/acroot/VectorCapacityTracker.C \
               ../ACsoft/acroot/Vertex.C \
               ../ACsoft/acroot/Vertex-Streamer.C \
               ../ACsoft/analysis/AnalysisParticle.C \
               ../ACsoft/analysis/BadRunManager.C \
               ../ACsoft/analysis/DatabaseReader.C \
               ../ACsoft/analysis/ParticleFactory.C \
               ../ACsoft/analysis/pathlength_functions.C \
               ../ACsoft/analysis/RejectionCalculator.C \
               ../ACsoft/analysis/SplineTrack.C \
               ../ACsoft/analysis/StraightLineTrack.C \
               ../ACsoft/analysis/TrackFactory.C \
               ../ACsoft/analysis/TrdCandidateMatching.C \
               ../ACsoft/analysis/TrdLikelihoodCalculator.C \
               ../ACsoft/analysis/TrdMissingHit.C \
               ../ACsoft/analysis/TrdHit.C \
               ../ACsoft/analysis/TrdHitFactory.C \
               ../ACsoft/analysis/TrdPdfHistogram.C \
               ../ACsoft/analysis/TrdPreselection.C \
               ../ACsoft/analysis/TrdSegment.C \
               ../ACsoft/analysis/TrdTrack.C \
               ../ACsoft/analysis/TrdTrackFitFunction.C \
               ../ACsoft/analysis/TrdTracking.C \
               ../ACsoft/cuts/Cut.C \
               ../ACsoft/cuts/CutFactory.C \
               ../ACsoft/cuts/SelectionParser.C \
               ../ACsoft/cuts/Selector.C \
               ../ACsoft/cuts/TwoSidedCut.C \
               ../ACsoft/cuts/ManualCut.C \
               ../ACsoft/cuts/ManualNMinusOneCut.C \
               ../ACsoft/cuts/NMinusOneCut.C \
               ../ACsoft/cuts/ParticleSelectionCuts.C \
               ../ACsoft/cuts/TrackerCharges.C \
               ../ACsoft/detector/DetectorManager.C \
               ../ACsoft/detector/TrdDetector.C \
               ../ACsoft/detector/TrdLayer.C \
               ../ACsoft/detector/TrdModule.C \
               ../ACsoft/detector/TrdStraw.C \
               ../ACsoft/detector/TrdSublayer.C \
               ../ACsoft/interface/TrdQt.C \
               ../ACsoft/io/FileManagerACQt.C \
               ../ACsoft/io/FileManagerACROOT.C \
               ../ACsoft/io/FileManager.C \
               ../ACsoft/io/MultiFileManager.C \
               ../ACsoft/io/ReducedEvent.C \
               ../ACsoft/io/ReducedEventWriter.C \
               ../ACsoft/io/ReducedFileHeader.C \
               ../ACsoft/platform/FastMalloc.cpp \
               ../ACsoft/platform/TypeTraits.cpp \
               ../ACsoft/producer/ACQtProducer-ACC.C \
               ../ACsoft/producer/ACQtProducer-BackTracing.C \
               ../ACsoft/producer/ACQtProducer.C \
               ../ACsoft/producer/ACQtProducer-DAQ.C \
               ../ACsoft/producer/ACQtProducer-ECAL.C \
               ../ACsoft/producer/ACQtProducer-EventHeader.C \
               ../ACsoft/producer/ACQtProducer-include.C \
               ../ACsoft/producer/ACQtProducer-MC.C \
               ../ACsoft/producer/ACQtProducer-Particles.C \
               ../ACsoft/producer/ACQtProducer-RICH.C \
               ../ACsoft/producer/ACQtProducer-RunHeader.C \
               ../ACsoft/producer/ACQtProducer-TOF.C \
               ../ACsoft/producer/ACQtProducer-Tracker.C \
               ../ACsoft/producer/ACQtProducer-TRD.C \
               ../ACsoft/producer/ACQtProducer-Trigger.C \
               ../ACsoft/producer/ACQtWriter.C \
               ../ACsoft/trdcalibration/gain_functions.C \
               ../ACsoft/trdcalibration/TrdAdcSpectra.C \
               ../ACsoft/trdcalibration/TrdAlignment.C \
               ../ACsoft/trdcalibration/TrdGainCalibration.C \
               ../ACsoft/utilities/Binning.C \
               ../ACsoft/utilities/CoordinateSystems.C \
               ../ACsoft/utilities/ConfigHandler.C \
               ../ACsoft/utilities/dumpstreamers.C \
               ../ACsoft/utilities/DeadStrawLookup.C \
               ../ACsoft/utilities/MCConfigHandler.C \
               ../ACsoft/utilities/McGeneratedEventsCalculation.C \
               ../ACsoft/utilities/McParticleId.C \
               ../ACsoft/utilities/McSpectrumScaler.C \
               ../ACsoft/utilities/ObjectManager.C \
               ../ACsoft/utilities/RejectionUtils.C \
               ../ACsoft/utilities/ResolutionModels.C \
               ../ACsoft/utilities/RunTagDecoder.C \
               ../ACsoft/utilities/RTIData.C \
               ../ACsoft/utilities/RTIReader.C \
               ../ACsoft/utilities/SimpleGraphLookup.C \
               ../ACsoft/utilities/Statistics.C \
               ../ACsoft/utilities/StringTools.C \
               ../ACsoft/utilities/TemplateFitter.C \
               ../ACsoft/utilities/TemplateFitter2D.C \
               ../ACsoft/utilities/TimeHistogramManager.C \
               ../ACsoft/utilities/Palettes.C \
               ../ACsoft/utilities/TrackfindingHistogram.C \
               ../ACsoft/utilities/TrdPdfLookup.C \
               ../ACsoft/utilities/TrdPdfWrapper.C \
               ../ACsoft/utilities/Utilities.C

ACSOFT_DIR_PLATFORM=../ACsoft/platform
ACSOFT_DIR_ACROOT=../ACsoft/acroot
ACSOFT_DIR_CUTS=../ACsoft/cuts
ACSOFT_DIR_UTILITIES=../ACsoft/utilities
ACSOFT_DIR_ANALYSIS=../ACsoft/analysis
ACSOFT_DIR_PRODUCER=../ACsoft/producer
ACSOFT_DIR_INTERFACE=../ACsoft/interface
ACSOFT_DIR_IO=../ACsoft/io
ACSOFT_DIR_DETECTOR=../ACsoft/detector
ACSOFT_DIR_TRDCALIBRATION=../ACsoft/trdcalibration

ACSOFT_OBJECTS_PLATFORM:=$(ACSOFT_FILES:$(ACSOFT_DIR_PLATFORM)/%.cpp=$(BIN)%.AC.o) 
ACSOFT_OBJECTS_ACROOT:=$(ACSOFT_OBJECTS_PLATFORM:$(ACSOFT_DIR_ACROOT)/%.C=$(BIN)%.AC.o) 
ACSOFT_OBJECTS_CUTS:=$(ACSOFT_OBJECTS_ACROOT:$(ACSOFT_DIR_CUTS)/%.C=$(BIN)%.AC.o) 
ACSOFT_OBJECTS_UTILITIES:=$(ACSOFT_OBJECTS_CUTS:$(ACSOFT_DIR_UTILITIES)/%.C=$(BIN)%.AC.o) 
ACSOFT_OBJECTS_ANALYSIS:=$(ACSOFT_OBJECTS_UTILITIES:$(ACSOFT_DIR_ANALYSIS)/%.C=$(BIN)%.AC.o) 
ACSOFT_OBJECTS_PRODUCER:=$(ACSOFT_OBJECTS_ANALYSIS:$(ACSOFT_DIR_PRODUCER)/%.C=$(BIN)%.AC.o) 
ACSOFT_OBJECTS_INTERFACE:=$(ACSOFT_OBJECTS_PRODUCER:$(ACSOFT_DIR_INTERFACE)/%.C=$(BIN)%.AC.o) 
ACSOFT_OBJECTS_IO:=$(ACSOFT_OBJECTS_INTERFACE:$(ACSOFT_DIR_IO)/%.C=$(BIN)%.AC.o) 
ACSOFT_OBJECTS_DETECTOR:=$(ACSOFT_OBJECTS_IO:$(ACSOFT_DIR_DETECTOR)/%.C=$(BIN)%.AC.o) 
ACSOFT_OBJECTS_TRDCALIBRATION:=$(ACSOFT_OBJECTS_DETECTOR:$(ACSOFT_DIR_TRDCALIBRATION)/%.C=$(BIN)%.AC.o) 
ACSOFT_OBJECTS:=$(ACSOFT_OBJECTS_TRDCALIBRATION)

ACSOFT_DIRS_CPP = $(ACSOFT_DIR_PLATFORM)
ACSOFT_DIRS_C = $(ACSOFT_DIR_ACROOT) $(ACSOFT_DIR_CUTS) $(ACSOFT_DIR_UTILITIES) $(ACSOFT_DIR_ANALYSIS) \
                $(ACSOFT_DIR_PRODUCER) $(ACSOFT_DIR_INTERFACE) $(ACSOFT_DIR_IO) $(ACSOFT_DIR_DETECTOR) \
                $(ACSOFT_DIR_TRDCALIBRATION)

vpath %.cpp $(ACSOFT_DIRS_CPP)
vpath %.C $(ACSOFT_DIRS_C)

$(BIN)%.AC.o : %.cpp
	@echo Compiling $< ...
	@if ! [ -d $(BIN) ] ; then mkdir -p $(BIN); fi
	$(CXX) -c $(ACSOFT_INCDIRS) $(ACSOFT_BUILDFLAGS) $(CFLAGS) $< -o $@

$(BIN)%.AC.o : %.C
	@echo Compiling $< ...
	@if ! [ -d $(BIN) ] ; then mkdir -p $(BIN); fi
	$(CXX) -c $(ACSOFT_INCDIRS) $(ACSOFT_BUILDFLAGS) $(CFLAGS) $< -o $@
endif

#TOF + ACC
COMMON_FILES += $(SCC)tofrec02.C   $(SCC)tofuser02.C \
		$(SCC)tofsim02.C   $(SCC)tofdbc02.C   $(SCC)tofcalib02.C \
		$(SCC)tofid.C      $(SCC)antidbc02.C  $(SCC)antirec02.C \
		$(SCC)anticalib02.C $(SCC)Tofsim02.C $(SCC)Tofdbc.C \
		$(SCC)Tofrec02_ihep.C $(SCC)tofhit.C $(SCC)Tofcharge_ihep.C \

#ECAL
COMMON_FILES += $(SCC)ecaldbc.C    $(SCC)ecid.C       $(SCC)ecalrec.C \
		$(SCC)ecalcalib.C  $(SCC)ecalBDT.C $(SCC)ecalBDTCHI2.C $(SCC)EcalChi2CY.C

#RICH
COMMON_FILES += $(SCC)richdbc.C           $(SCC)richrec.C        $(SCC)richid.C      $(SCC)richgeom.C  \
		$(SCC)richlip.C           $(SFF)richpatt_lip.F      $(SFF)richrec_lip.F \
		$(SFF)richpmtlib.F        $(SFF)gtckov.F         $(SFF)graylc.F      $(SFF)cerenkov.F \
		$(SCC)richradid.C         $(SFF)ggckov.F         $(SCC)richcal.C     $(SCC)richdaq.C \
		$(SFF)richbetalike_lip.F \
		$(SFF)richbetastand_lip.F $(SFF)richchg_lip.F \
		$(SFF)richutil_lip.F \
		$(SCC)RichCharge.C $(SCC)RichConfig.C $(SCC)RichBeta.C $(SCC)RichTools.C 

# High level reconstruction
COMMON_FILES += $(SCC)beta.C     $(SCC)charge.C   $(SCC)particle.C   \
		$(SCC)astro.C    $(SCC)io.C       

#Trigger
COMMON_FILES += $(SCC)trigger102.C $(SCC)trigger302.C  


#DAQ Interface
COMMON_FILES +=  $(SCC)daqs2block.C $(SCC)daqecblock.C 

#SlowControlDB
COMMON_FILES +=  $(SCC)SlowControlDB.C

ifdef WEBACCESS
#AMSDataDir Web access interface
COMMON_FILES +=  $(SCC)fopen.c 
endif

#HBOOK interface
#COMMON_FILES += $(SCC)ntuple.C

#SunPosition
 COMMON_FILES +=  $(SCC)Sunposition.C

#Coordinate Transforamtion
 COMMON_FILES +=  $(SCC)FrameTrans.C

#Geomagnetic Trace back
 COMMON_FILES +=  $(SCC)GeoMagField.C $(SCC)GeoMagCoord.C $(SCC)GeoMagTrace.C

#GeoMagneticCutoff 
 COMMON_FILES += $(SCC)GM_SubLibrary.C

#BackTracing
COMMON_FILES += $(SCC)BackTracingMIB.C
COMMON_FILES += $(SFF)BackTracing.F

# Unfolding
#COMMON_FILES += $(SCC)UnfoldingTools.C

# AntiPG
COMMON_FILES += $(SCC)AntiPG.C

#MVA
COMMON_FILES += $(SCC)MVAmanager.C $(SCC)ConfigParser.C \
		$(SCC)OutHistoManager.C  $(SCC)OutTreeManager.C

#COMMON_FILES += $(SCC)ConfigParser.C


#Root Interface
COMMON_FILES += $(SCC)root.C $(SCC)root_setup.C   

ifdef PGTRACK
ROOTDICTFILES +=  ../include/TkPlane.h ../include/TkSens.h \
		../include/TrCalDB.h ../include/TrGainDB.h ../include/TrChargeLossDB.h ../include/TrOccDB.h \
		../include/TrEDepDB.h ../include/TrMipDB.h ../include/TrLinearDB.h ../include/TrLikeDB.h \
		../include/tkpatt.h         ../include/TkCoo.h  ../include/TkSens.h \
		../include/TrRecon.h 	  ../include/TkDBc.h  ../include/MagField.h \
		../include/TrLadCal.h \
		../include/TkLadder.h     ../include/TkObject.h ../include/TrLadPar.h  ../include/TrAlignFit.h \
		../include/TrCluster.h    ../include/TrRecHit.h ../include/TrTrack.h \
		../include/TrTasCluster.h ../include/TrTasPar.h ../include/TrTasDB.h \
		../include/TrFit.h \
		../include/TrMCCluster.h \
                ../include/trrec.h ../include/tkdcards.h ../include/TrdMcDatacards.h \
		../include/point.h ../include/edep.h ../include/root_RV.h ../include/root.h \
		../include/TrSim.h ../include/TrSimSensor.h ../include/TrSimCluster.h \
		../include/TrPdf.h ../include/TrPdfDB.h ../include/TrCharge.h \
		../include/TrHistoMan.h ../include/HistoMan.h \
		../include/TrTrackSelection.h \
		../include/TrkLH.h \
		../include/root_setup.h ../include/TrExtAlignDB.h ../include/TrInnerDzDB.h  ../include/pfunction.h
else
ROOTDICTFILES +=  ../include/point.h ../include/edep.h ../include/root_RV.h ../include/root.h  ../include/amschain.h ../include/root_setup.h ../include/TrPdf.h ../include/trfit.h 
endif

ROOTDICTFILES += ../include/VCon.h ../include/TrElem.h ../include/trigger102_setup.h  

ROOTDICTFILES += ../include/TrdRawHit.h ../include/TrdHSegment.h ../include/TrdHTrack.h ../include/TrdHRecon.h ../include/TrdHCalib.h ../include/TrdHCharge.h ../include/TrdTFit.h ../include/SlowControlDB.h

ROOTDICTFILES += ../include/DynAlignment.h ../include/RichTools.h ../include/RichCharge.h ../include/RichConfig.h ../include/Tofrec02_ihep.h  ../include/Tofcharge_ihep.h ../include/EcalChi2CY.h ../include/GeoMagField.h ../include/GeoMagCoord.h ../include/GeoMagTrace.h ../include/GM_SubLibrary.h

ROOTDICTFILES += ../include/BackTracingMIB.h

ifdef PGTRACK
ROOTDICTFILES += ../include/TrdSCalib.h ../include/TrdKCalib.h ../include/TrdKHit.h ../include/TrdMTrack.h ../include/TrdKPDF.h ../include/TrdKCluster.h ../include/TrdKToyMC.h
ROOTDICTFILES += ../include/TofGeometry.h  ../include/TofRecon.h  ../include/TofTrack.h ../include/bcorr.h
endif

ROOTDICTFILES += ../include/UnfoldingTools.h

# AntiPG
ROOTDICTFILES += ../include/AntiPG.h

# MVA
ROOTDICTFILES +=../include/MVAmanager.h ../include/ConfigParser.h  ../include/OutTreeManager.h  ../include/OutHistoManager.h

ROOTDICTFILES += ../include/linkdef.h


#------------------------------------------------------------------------------
# File list for standard gbatch compilation
#------------------------------------------------------------------------------

FILES= 	$(COMMON_FILES) \
	$(SCC)gbatch.C  $(SCC)geant.C  	$(SCC)geant3.C      \
	$(SCC)job.C     $(SCC)event.C   $(SCC)daqevt.C $(SFF)rndm.F   \
	$(SCC)timeid.C  $(SCC)ntuple.C  $(SFF)richnodisp_lip.F

ifndef PGTRACK
FILES +=$(SFF)TrackFit_utils.F
endif

OBJECTS:= $(BIN)mtx.o $(BIN)rootdict.o

MYOBJS1:=$(FILES:$(SFF)%.F=$(BIN)%.o) 
MYOBJS2:=$(MYOBJS1:$(SCC)%.C=$(BIN)%.o) 
MYOBJS:=$(MYOBJS2:$(SCC)%.c=$(BIN)%.o) 
OBJECTS+=$(MYOBJS:$(SFF)%.f=$(BIN)%.o) 

#########################################################################
## please put those objs which need special command to generate here ####
#########################################################################
ifeq ($(MARCH),linuxia64ecc) 
SPECIAL_OBJS=$(BIN)trigger302.o
OBJECTS:=$(filter-out $(SPECIAL_OBJS),$(OBJECTS)) 
SPOPT=-O0
endif


#------------------------------------------------------------------------------
# File list for readdaq  gbatch compilation

FILESR= $(COMMON_FILES)     $(SCC)readdaq.C $(SCC)daqevt.C  $(SFF)richnodisp_lip.F

ifndef PGTRACK
FILESR +=$(SFF)TrackFit_utils.F
endif

MYOBJS1:=$(FILESR:$(SFF)%.F=$(BIN)%.o) 
MYOBJS2:=$(MYOBJS1:$(SCC)%.c=$(BIN)%.o) 
MYOBJS:=$(MYOBJS2:$(SCC)%.C=$(BIN)%.o) 
OBJECTR:=$(MYOBJS:$(SFF)%.f=$(BIN)%.o) 

OBJECTR+= 	$(BIN)geant.o  $(BIN)geant3.o $(BIN)job.o \
		$(BIN)event.o   $(BIN)timeid.o $(BIN)ntuple.o \
		$(BIN)rootdict.o

#------------------------------------------------------------------------------
# File list for interactive gbatch compilation

ifdef  G4AMS
FILESI= $(COMMON_FILES)  \
	$(SCC)g4visman.C $(SFF)gi.F $(SCC)gi2.C  \
	$(SCC)job.C $(SCC)event.C $(SCC)daqevt.C \
        $(SCC)timeid.C  $(SCC)ntuple.C  $(SFF)richdisp_lip.F
else
FILESI= $(COMMON_FILES)  \
	$(SCC)gxint.C $(SFF)gi.F $(SCC)gi2.C  \
	$(SCC)job.C $(SCC)event.C $(SCC)daqevt.C \
        $(SCC)timeid.C  $(SCC)ntuple.C  $(SFF)richdisp_lip.F
endif
ifndef PGTRACK
FILESI +=$(SFF)TrackFit_utils.F
endif

MYOBJS1:=$(FILESI:$(SFF)%.F=$(BIN)%.o) 
MYOBJS2:=$(MYOBJS1:$(SCC)%.C=$(BIN)%.o) 
MYOBJS:=$(MYOBJS2:$(SCC)%.c=$(BIN)%.o) 
OBJECTSI:=$(MYOBJS:$(SFF)%.f=$(BIN)%.o) 

ifdef  G4AMS
OBJECTSI+=  $(BIN)mtx.o  $(BIN)rootdict.o   $(BIN)geant4i.o $(BIN)geanti.o  $(BIN)gbatchi.o
else
OBJECTSI+=  $(BIN)mtx.o  $(BIN)rootdict.o   $(BIN)geanti.o 	$(BIN)geant3i.o  
endif
#------------------------------------------------------------------------------
# File list for client(production) gbatch compilation

FILESC= $(COMMON_FILES)     $(SCC)producer.C $(SCC)client.C \
	$(SCC)ins_dummy.C $(SFF)rndm.F $(SFF)richnodisp_lip.F

MYOBJS1:=$(FILESC:$(SFF)%.F=$(BIN)%.o) 
MYOBJS2:=$(MYOBJS1:$(SCC)%.C=$(BIN)%.o) 
MYOBJS:=$(MYOBJS2:$(SCC)%.c=$(BIN)%.o) 
OBJECTSC:=$(MYOBJS:$(SFF)%.f=$(BIN)%.o) 

OBJECTSC+= 	$(BIN)gbatch_c.o  $(BIN)geant_c.o  $(BIN)geant3_c.o  $(BIN)job_c.o \
		$(BIN)event_c.o   $(BIN)timeid_c.o $(BIN)ntuple_c.o  $(BIN)daqevt_c.o  $(BIN)rootdict.o
ifdef  G4AMS
OBJECTSC +=$(BIN)geant4_c.o
OBJECTSC:=$(filter-out $(BIN)geant4.o,$(OBJECTSC)) 
OBJECTSI:=$(filter-out $(BIN)geant4.o,$(OBJECTSI))
endif
ifndef PGTRACK
OBJECTSC += $(BIN)TrackFit_utils_c.o
endif

ifdef AMS_ACQT_INTERFACE
OBJECTS+=$(ACSOFT_OBJECTS)
OBJECTSI+=$(ACSOFT_OBJECTS)
OBJECTSC+=$(ACSOFT_OBJECTS)
endif

#---Qi Yan
ifdef G4AMS
$(DPMBIN)%.o : $(DPMFF)%.f
	@echo Compiling  $< ...
	@if ! [ -d $(DPMBIN) ] ; then mkdir -p $(DPMBIN); fi
	$(FF) -c $(FFLAGS) -I$(CERNDIR)/include $< -o $@


$(DPMBIN)%.o : $(DPMSCC)%.cc
	@echo Compiling  $< ...
	@if ! [ -d $(DPMBIN) ] ; then mkdir -p $(DPMBIN); fi
	$(CXX) -c $(CFLAGS) -I$(CERNDIR)/include -I$(DPMDIR)/include -I../include  $< -o $@

DPMSCCLIST= $(wildcard $(DPMSCC)/*.cc)
DPMSCCLIST1= $(notdir $(DPMSCCLIST))
DPMSCCOBJ=$(patsubst %.cc, $(DPMBIN)%.o, $(DPMSCCLIST1))

DPMFFLIST= $(wildcard $(DPMFF)/*.f)
DPMFFLIST1= $(notdir $(DPMFFLIST))
DPMFFOBJ=$(patsubst %.f, $(DPMBIN)%.o, $(DPMFFLIST1))

OBJECTS+=  $(DPMSCCOBJ) $(DPMFFOBJ)
OBJECTSI+= $(DPMSCCOBJ) $(DPMFFOBJ)
OBJECTSC+= $(DPMSCCOBJ) $(DPMFFOBJ)
endif
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
		$(BIN)client.o     $(BIN)amsdbc.o   $(BIN)timeid_server.o \
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

STANDALONE:=$(EXE)gbatch$(G4)$(PG).exe 

gbatch: amswd $(STANDALONE)

$(STANDALONE): $(OBJECTS) $(SPECIAL_OBJS) $(BIN)ctype.o $(BIN)predict.o
	@if ! [ -d $(EXE) ] ; then mkdir -p $(EXE); fi; echo "Creating the $(EXE) directory ...."
	$(LD) $(FLAGSSTATIC)  -o $@ $(OBJECTS)  $(SPECIAL_OBJS) $(TIMEL) $(LDFLAGSSTATIC)

$(SPECIAL_OBJS):$(SPECIAL_OBJS: %.o=%.C) 
	 $(CXX) -c  $(CFLAGS) $(SPOPT)  $(SCC)trigger302.C -o $(BIN)/trigger302.o

readdaq:amswd  $(EXE)readdaq$(PG)

$(EXE)readdaq$(PG):  $(OBJECTR) $(BIN)ctype.o 
	$(LD) $(FLAGSSTATIC)  -o  $@  $(OBJECTR) $(TIMEL) $(LDFLAGSSTATIC)

#------------------------------------------------------------------------------
#  Gbatch standalone interactive session

ifndef G4AMS
INTERACTIVE:=$(EXE)gxint$(G4)$(PG).exe 
gxint :	amswd $(INTERACTIVE)


$(INTERACTIVE):	$(OBJECTSI) $(BIN)ctype.o $(BIN)predict.o
	@if ! [ -d $(EXE) ] ; then mkdir -p $(EXE); fi; echo "Creating the $(EXE) directory ...."
	$(LD) -v -o $@  $(OBJECTSI) $(LDFLAGSI)


$(BIN)geanti.o:      $(SCC)geant.C
	$(CXX) -c  $(CFLAGSI) $(SCC)geant.C -o $(BIN)/geanti.o

$(BIN)geant3i.o:      $(SCC)geant3.C
	$(CXX) -c  $(CFLAGSI) $(SCC)geant3.C -o $(BIN)/geant3i.o
else
INTERACTIVE:=$(EXE)gxint$(G4)$(PG).exe 
gxint :	amswd $(INTERACTIVE)


$(INTERACTIVE):	$(OBJECTSI) $(BIN)ctype.o $(BIN)predict.o
	@if ! [ -d $(EXE) ] ; then mkdir -p $(EXE); fi; echo "Creating the $(EXE) directory ...."
	$(LD) -v -o $@  $(OBJECTSI) $(LDFLAGSI)


$(BIN)geant4i.o:      $(SCC)geant4.C
	$(CXX) -c  $(CFLAGSI) -DG4VIS_USE $(SCC)geant4.C -o $(BIN)/geant4i.o

$(BIN)geanti.o:      $(SCC)geant.C
	$(CXX) -c  $(CFLAGSI) $(SCC)geant.C -o $(BIN)/geanti.o
$(BIN)gbatchi.o:      $(SCC)gbatch.C
	$(CXX) -c  $(CFLAGSI) $(SCC)gbatch.C -o $(BIN)/gbatchi.o
endif
#---------------------------------------------------------------------------------
# Client TARGET (producer)

CLIENT=$(EXE)gbatch-orbit$(G4)$(PG).exe
CLIENT6=$(EXE)gbatch-orbit$(G4)$(PG).exe.lxplus6

producer: amswd	$(CLIENT)

#                   Orbit-C      Orbit-C++   gbatch
$(CLIENT):          $(OBJECTCO) $(OBJECTCL) $(OBJECTSC) $(BIN)ctype.o $(BIN)predict.o
	@if ! [ -d $(EXE) ] ; then mkdir -p $(EXE); fi; echo "Creating the $(EXE) directory ...."
	$(LD) $(FLAGSSTATIC) -v -o $@  $(OBJECTCO)  $(OBJECTCL) $(OBJECTSC) $(LDFLAGSSTATICIDL) $(TIMEL) $(LDFLAGSSTATIC)


producer6: amswd	$(CLIENT6)

#                   Orbit-C      Orbit-C++   gbatch
$(CLIENT6):          $(OBJECTCO) $(OBJECTCL) $(OBJECTSC) $(BIN)ctype_l6.o $(BIN)predict.o
	@if ! [ -d $(EXE) ] ; then mkdir -p $(EXE); fi; echo "Creating the $(EXE) directory ...."
	$(LD) $(FLAGSSTATIC) -v -o $@  $(OBJECTCO)  $(OBJECTCL) $(OBJECTSC) $(LDFLAGSSTATICIDL) $(TIMEL) $(LDFLAGSSTATIC) $(BIN)ctype_l6.o


#---------------------------------------------------------------------------------
#  Server TARGET (server)

SERVER=$(EXE)amsprodserver$(G4)$(PG).exe

test: $(OBJECTCO)
	echo $^

server: amswd	$(SERVER)
#               Orbit-C   Orbit-C++-Ser
$(SERVER):    $(OBJECTCO) $(OBJECTSE)   $(OBJECTSEAUX) $(BIN)server.o $(BIN)ctype.o
	@if ! [ -d $(EXE) ] ; then mkdir -p $(EXE); fi; echo "Creating the $(EXE) directory ...."
	$(LD) -v -o $(SERVER) $(BIN)server.o  $(OBJECTCO) $(OBJECTSE) $(OBJECTSEAUX)   $(LDFLAGSSTATICIDL)  


#------------------------------------------------------------------------------
#   Libraries to read the ROOT output

lib: shared static 

ifdef PGTRACK
SHLIBO:=$(AMS_BASEO) $(TRACK_BASEO) $(BIN)amschain_s.o  \
        $(BIN)rnormx_s.o $(BIN)TrFit_s.o $(BIN)TrRecon_s.o  $(BIN)TrSim_s.o $(BIN)TrSimCluster_s.o $(BIN)TrSimSensor_s.o $(BIN)TkDBc_s.o $(BIN)TrTrack_s.o \
	$(BIN)root_rs.o $(BIN)root_setup_s.o  $(BIN)SlowControlDB_s.o $(BIN)TrCalDB_s.o $(BIN)TrTasDB_s.o $(BIN)TrGainDB_s.o $(BIN)TrChargeLossDB_s.o $(BIN)TrOccDB_s.o \
	$(BIN)TrEDepDB_s.o $(BIN)TrMipDB_s.o $(BIN)TrLinearDB_s.o $(BIN)TrLikeDB_s.o \
        $(BIN)rootdict_s.o   $(BIN)MagField_s.o \
        $(BIN)VCon_s.o $(BIN)TrdHSegment_s.o $(BIN)TrdHTrack.o $(BIN)TrdHCalib.o $(BIN)TrdMcDatacards.o $(BIN)TrdSCalib_s.o  $(BIN)TrdKCalib_s.o $(BIN)TrdKToyMC_s.o  $(BIN)TrdMTrack_s.o  $(BIN)TrdKCluster_s.o $(BIN)TrdKHit_s.o $(BIN)TrdKPDF_s.o $(BIN)TrdHCharge.o $(BIN)TrdTFit.o $(BIN)TrdHRecon_s.o $(BIN)VCon_root_s.o $(BIN)TrElem.o  $(BIN)ecalBDT.o $(BIN)ecalBDTCHI2.o $(BIN)commonsi.o $(BIN)EcalChi2CY.o \
	$(BIN)timeid_s.o $(BIN)TrExtAlignDB_s.o  $(BIN)DynAlignment_s.o $(BIN)RichCharge_s.o $(BIN)RichConfig_s.o $(BIN)RichBeta_s.o $(BIN)RichTools_s.o $(BIN)pfunction_s.o $(BIN)OrbGen_s.o $(BIN)cgmmap.o $(BIN)Sunposition.o $(BIN)FrameTrans.o $(BIN)GeoMagField.o $(BIN)GeoMagCoord.o $(BIN)GeoMagTrace.o   $(BIN)GM_SubLibrary.o \
	$(BIN)rkms_s.o $(BIN)gufld.o $(BIN)util.o $(BIN)predict.o
SHLIBO+=$(BIN)TofGeometry_s.o  $(BIN)TofRecon_s.o  $(BIN)TofTrack_s.o $(BIN)Tofdbc_s.o $(BIN)Tofrec02_ihep_s.o $(BIN)Tofcharge_ihep_s.o $(BIN)bcorr_s.o

SHLIBO+=$(BIN)BackTracingMIB.o
SHLIBO+=$(BIN)BackTracing.o
SHLIBO+=$(BIN)UnfoldingTools.o
SHLIBO+=$(BIN)AntiPG_s.o


SHLIBO+=$(BIN)MVAmanager.o
SHLIBO+=$(BIN)ConfigParser.o
SHLIBO+=$(BIN)OutTreeManager.o
SHLIBO+=$(BIN)OutHistoManager.o


SHLIBO+=$(ACSOFT_OBJECTS)
SHLIBBO:=$(SHLIBO)
else
SHLIBBO := $(BIN)point.o $(BIN)edep.o $(BIN)amschain_s.o $(BIN)root_rs.o $(BIN)rootdict_s.o $(BIN)id.o $(BIN)VCon_s.o $(BIN)TrdHSegment_s.o $(BIN)TrdHTrack.o $(BIN)TrdHCalib.o $(BIN)TrdHCharge.o $(BIN)TrdTFit.o $(BIN)TrdHRecon_s.o $(BIN)VCon_root_s.o $(BIN)TrElem.o $(BIN)root_setup_s.o $(BIN)tracking.o $(BIN)SlowControlDB.o $(BIN)ecalBDT.o $(BIN)ecalBDTCHI2.o $(BIN)EcalChi2CY.o $(BIN)timeid_s.o $(BIN)amsdbc.o  $(BIN)astring.o $(BIN)commonsi.o $(BIN)TrPdf.o $(BIN)TrFitF.o $(BIN)tkfit.o $(BIN)rkms.o $(BIN)/trfit_s.o $(BIN)tkfit.o $(BIN)/mfield_s.o $(BIN)/fint_my.o $(BIN)/bcorr_s.o  $(BIN)/trafit.o $(BIN)/txyfit.o  $(BIN)/tszfit.o $(BIN)/tptcir.o $(BIN)/tcircf.o $(BIN)/DynAlignment_s.o $(BIN)RichCharge_s.o $(BIN)RichConfig_s.o $(BIN)RichBeta_s.o $(BIN)RichTools_s.o $(BIN)Sunposition.o $(BIN)FrameTrans.o $(BIN)Tofdbc_s.o $(BIN)Tofrec02_ihep_s.o  $(BIN)Tofcharge_ihep_s.o $(BIN)GeoMagField.o $(BIN)GeoMagCoord.o $(BIN)GeoMagTrace.o  $(BIN)GM_SubLibrary.o $(BIN)predict.o  $(BIN)BackTracingMIB.o $(BIN)BackTracing.o $(BIN)UnfoldingTools.o $(BIN)AntiPG_s.o

SHLIBO := $(BIN)point.o $(BIN)edep.o $(BIN)amschain_s.o $(BIN)root_rs.o $(BIN)rootdict_s.o $(BIN)id.o $(BIN)VCon_s.o $(BIN)TrdHSegment_s.o $(BIN)TrdHTrack.o $(BIN)TrdHCalib.o $(BIN)TrdHCharge.o $(BIN)TrdTFit.o $(BIN)TrdHRecon_s.o $(BIN)VCon_root_s.o $(BIN)TrElem.o $(BIN)root_setup_s.o $(BIN)tracking.o $(BIN)SlowControlDB.o $(BIN)ecalBDT.o $(BIN)ecalBDTCHI2.o $(BIN)EcalChi2CY.o $(BIN)timeid_s.o $(BIN)amsdbc.o  $(BIN)astring.o $(BIN)commonsi.o $(BIN)TrPdf.o  $(BIN)/trfit.o $(BIN)DynAlignment_s.o $(BIN)RichCharge_s.o $(BIN)RichConfig_s.o $(BIN)RichBeta_s.o $(BIN)RichTools_s.o $(BIN)Sunposition.o $(BIN)FrameTrans.o $(BIN)Tofdbc_s.o $(BIN)Tofrec02_ihep_s.o $(BIN)Tofcharge_ihep_s.o $(BIN)GeoMagField.o $(BIN)GeoMagCoord.o $(BIN)GeoMagTrace.o $(BIN)GM_SubLibrary.o $(BIN)predict.o  $(BIN)BackTracingMIB.o $(BIN)BackTracing.o
SHLIBO+=$(BIN)UnfoldingTools.o
SHLIBO+=$(BIN)AntiPG_s.o


SHLIBO+=$(BIN)MVAmanager.o
SHLIBO+=$(BIN)ConfigParser.o
SHLIBO+=$(BIN)OutTreeManager.o
SHLIBO+=$(BIN)OutHistoManager.o


SHLIBO+=$(ACSOFT_OBJECTS)
SHLIBBO+=$(ACSOFT_OBJECTS)
endif

shared : amswd $(SHLD)
$(SHLD):  $(SHLIBO)   ../include/root_RV.h ../include/root.h  
	@if ! [ -d $(LIB) ] ; then mkdir -p $(LIB); fi; echo "Creating the $(LIB) directory ...."
	$(LD) -O   $(SHLIBO) $(SOFLAGS) $(SHLDFLAGS)  -o  $(SHLD)

ifdef AMS_ACQT_INTERFACE
static : amswd $(NTLD)
$(NTLD): $(SHLIBBO)
	@if ! [ -d $(LIB) ] ; then mkdir -p $(LIB); fi; echo "Creating the $(LIB) directory ...."
	$(AR) rv   $(NTLD) $(SHLIBBO)
	@if ! [ -z $(QT_STATIC_LIBRARYP) ] ; then mkdir -p qt_tmp; cd qt_tmp; $(AR) x $(QT_STATIC_LIBRARYP); cd ..; $(AR) q $(NTLD) `ls qt_tmp/*.o | xargs`; rm -Rf qt_tmp; fi
else
static : amswd $(NTLD)
$(NTLD): $(SHLIBBO)
	@if ! [ -d $(LIB) ] ; then mkdir -p $(LIB); fi; echo "Creating the $(LIB) directory ...."
	$(AR) rv   $(NTLD) $(SHLIBBO)
endif

#-------------------------------------------------------------------------------
# these files do not use the default rules

ifndef VGM_INSTALL
$(BIN)geant.o: 	 $(SCC)geant.C
	$(CXX) -c $(CFLAGS) -D__BATCH__  $< -o $@
else
$(BIN)geant.o: 	 $(SCC)geant.C
	$(CXX) -c $(CFLAGS) -D_USEVGM_ -I$(VGM_INSTALL)/include -D__BATCH__  $< -o $@
endif

$(BIN)geant3.o:	 $(SCC)geant3.C
	$(CXX) -c $(CFLAGS) -D__BATCH__  $< -o $@

$(BIN)ctype.o: $(SCC)ctype.c
ifeq  "$(shell uname)"  "Linux" 
	$(CC) -c $< -o$@
else 
	cp $(SCC)ctype.c $(BIN)ctype.o
endif

$(BIN)ctype_l6.o: $(SCC)ctype.c
	$(CC) -c -D__LXPLUS6__ $< -o$@
$(BIN)predict.o:  $(SCC)predict.c
	$(CC) -c $(CFLAGS) $< -o$@

../include/root_methods$(PG).h:      ../perl/root.perl
	perl ../perl/root.perl

ifdef PGTRACK
$(BIN)rootdict.C:  $(ROOTDICTFILES)
	@echo "Generating dictionary"
	$(ROOTSYS)/bin/rootcint   -f $(BIN)rootdict.C -c  -p $(ROOTDICT_CPPFLAGS) -I$(ROOTSYS)/include -I../include $^ 
	@echo "OK"

$(BIN)rootdict_s.C:   ../include/VCon_root.h ../include/root_RVSP.h ../include/amschain.h $(ROOTDICTFILES)
	@echo "Generating dictionary"
	$(ROOTSYS)/bin/rootcint    -f $(BIN)rootdict_s.C -c -p  $(ROOTDICT_CPPFLAGS) -I$(ROOTSYS)/include -I../include $^ 
	@echo "OK"
else
$(BIN)rootdict.C: ../include/point.h ../include/edep.h ../include/root_RV.h ../include/root.h ../include/root_setup.h ../include/trigger102_setup.h ../include/trfit.h ../include/DynAlignment.h ../include/RichTools.h ../include/RichCharge.h ../include/RichConfig.h  ../include/Tofrec02_ihep.h ../include/Tofcharge_ihep.h ../include/EcalChi2CY.h ../include/GeoMagField.h ../include/GeoMagCoord.h ../include/GeoMagTrace.h ../include/bcorr.h ../include/linkdef.h
	@echo "Generating dictionary"
	$(ROOTSYS)/bin/rootcint      -f $(BIN)rootdict.C -c -I$(ROOTSYS)/include -I../include $^ 
	@echo "OK"

$(BIN)rootdict_s.C: ../include/point.h ../include/edep.h ../include/VCon_root.h ../include/root_RVS.h ../include/root.h ../include/amschain.h  ../include/tracking.h ../include/trfit.h ../include/DynAlignment.h ../include/RichTools.h ../include/RichCharge.h ../include/RichConfig.h ../include/Tofrec02_ihep.h ../include/Tofcharge_ihep.h ../include/EcalChi2CY.h ../include/GeoMagField.h ../include/GeoMagCoord.h ../include/GeoMagTrace.h ../include/bcorr.h ../include/UnfoldingTools.h ../include/AntiPG.h ../include/linkdef.h
	@echo "Generating dictionary"
	$(ROOTSYS)/bin/rootcint   -f $(BIN)rootdict_s.C -c -I$(ROOTSYS)/include -I../include  $^ 
	@echo "OK"

endif

$(BIN)commonsi_time.C:      $(SCC)commonsi.C
	sed "s_buildtime=[^buildtime=]*;_buildtime=$(shell date +%s);_" $(SCC)commonsi.C > $(BIN)commonsi_time.C

$(BIN)rootdict.o:         $(BIN)rootdict.C
	$(CXX) -c $(ACSOFT_INCDIRS) $(CFLAGS) $(BIN)rootdict.C -o $(BIN)rootdict.o 

$(BIN)commonsi.o:	$(BIN)commonsi_time.C
	$(CXX) -c $(CFLAGS) $(BIN)commonsi_time.C -o $(BIN)commonsi.o 
$(BIN)daqevt.o:         $(SCC)daqevt.C
	$(CXX) -c $(CFLAGSDAQ) -D__AMS02DAQ__ $(SCC)daqevt.C -o $(BIN)daqevt.o 

$(BIN)daqevt_c.o:         $(SCC)daqevt.C
	$(CXX_ORBIT) -c $(CFLAGSDAQ) -D__AMS02DAQ__ $(IDLFLAGS) $(SCC)daqevt.C -o $(BIN)daqevt_c.o 


$(BIN)rootdict_s.o:         $(BIN)rootdict_s.C
	$(CXX) -c $(ACSOFT_INCDIRS) $(CFLAGS)  $(BIN)rootdict_s.C -o $(BIN)rootdict_s.o 


$(BIN)root_rs.o:    	 $(SCC)root.C ../include/root_methods$(PG).h
	$(CXX) -c $(CFLAGS)  -D__ROOTSHAREDLIBRARY__ $(SCC)root.C -o $(BIN)root_rs.o



$(BIN)ntuple.o:    	 $(SCC)ntuple.C

ifdef AMSMEMMONITOR
	$(CXX) -c -D__MEMMONITOR__ $(CFLAGS) $(SCC)ntuple.C -o $(BIN)ntuple.o
else
	$(CXX) -c $(CFLAGS) $(SCC)ntuple.C -o $(BIN)ntuple.o
endif

$(BIN)ntuple_c.o:   $(SCC)ntuple.C
	$(CXX_ORBIT) -c $(CFLAGS) $(IDLFLAGS) $(SCC)ntuple.C -o $(BIN)ntuple_c.o                                                                                  
$(BIN)TrackFit_utils_c.o:   $(SFF)TrackFit_utils.F
	$(FF) -c $(FFLAGSn) $(IDLFLAGS) $(SFF)TrackFit_utils.F -o $(BIN)TrackFit_utils_c.o                                                                                
$(BIN)TrackFit_utils.o:	$(SFF)TrackFit_utils.F
	$(FF) -c $(FFLAGSn)  $(SFF)TrackFit_utils.F -o $(BIN)TrackFit_utils.o                                                                      



$(BIN)Geant3_utils.o:   $(SFF)Geant3_utils.F
	$(FF) -c $(FFLAGS) $(FFLAGSSAVE)  -I$(CERNDIR)/include $(SFF)Geant3_utils.F -o $(BIN)Geant3_utils.o                                                                                
$(BIN)trd.o:   $(SFF)trd.F
	$(FF) -c $(FFLAGS) $(FFLAGSSAVE) -I$(CERNDIR)/include $(SFF)trd.F -o $(BIN)trd.o                                                                                
$(BIN)rkms.o:   $(SFF)rkms.F
	$(FF) -c $(FFLAGS) $(FFLAGSOPT) -I$(CERNDIR)/include $(SFF)rkms.F -o $(BIN)rkms.o                                                                                
$(BIN)BackTracing.o:   $(SFF)BackTracing.F
	$(FF) -c $(FFLAGS) $(FFLAGSSAVE) -I$(CERNDIR)/include $(SFF)BackTracing.F -o $(BIN)BackTracing.o

$(BIN)UnfoldingTools.o:   $(SCC)UnfoldingTools.C
	$(CXX) -c $(CFLAGS) $(SCC)UnfoldingTools.C -o $(BIN)UnfoldingTools.o

$(BIN)AntiPG_s.o: $(SCC)AntiPG.C
	$(CXX) -c $(CFLAGS) -D__ROOTSHAREDLIBRARY__ $(SCC)AntiPG.C -o $(BIN)AntiPG_s.o

$(BIN)AntiPG.o: $(SCC)AntiPG.C
	$(CXX) -c $(CFLAGS) $(SCC)AntiPG.C -o $(BIN)AntiPG.o

ifeq  "$(shell uname)"  "SunOS" 

$(BIN)mtx.o:                   $(SFF)mtx77.F
	f77 -c $(FFLAGS)      $(SFF)mtx77.F -o $(BIN)mtx.o
endif
ifeq  "$(shell uname)"  "HP-UX" 

$(BIN)mtx.o:                   $(SFF)mtx77.F
	f77 -c $(FFLAGS)      $(SFF)mtx77.F -o $(BIN)mtx.o
endif
ifeq  "$(shell uname)"  "Darwin" 

$(BIN)mtx.o:                   $(SFF)mtx77.F
	$(FF) -c $(FFLAGS)      $(SFF)mtx77.F -o $(BIN)mtx.o
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
	$(FFOLD) -c $(FFLAGS) -I$(CERNDIR)/include $(SFF)grfile.F -o $(BIN)grfile.o

$(SFF)gstord.F:      $(CERNSRCDIR)/src/geant321/gtrak/gstord.F
	sed  "s/DO 30/       if(jpolar.lt.jstak .and.jplast+NWPOLA.gt.jstak)then\n          write(*,*)' jpolar overwrite??? ',jplast,jstak\n           return\n            endif\n       DO 30/"   $(CERNSRCDIR)/src/geant321/gtrak/gstord.F > $(SFF)gstord.F

$(BIN)gstord.o:      $(SFF)gstord.F
	$(FFOLD) -c $(FFLAGS) -I$(CERNDIR)/include $(SFF)gstord.F -o $(BIN)gstord.o

$(SFF)gskpho.F:      $(CERNSRCDIR)/src/geant321/gtrak/gskpho.F
	sed  "s/IQ(JSTAK+1)+N2-N1+1-IQ(JPOLAR-1)\/3/IQ(JSTAK+2)-IQ(JPOLAR-1)\/3/g;s/N2-N1/NGPHOT\*10/g"  $(CERNSRCDIR)/src/geant321/gtrak/gskpho.F > $(SFF)gskpho.F

#$(BIN)gskpho.o:      $(SFF)gskpho.F
#	$(FFOLD) -c $(FFLAGS) -I$(CERNDIR)/include $(SFF)gskpho.F -o $(BIN)gskpho.o

$(BIN)ggckov.o:      $(SFF)ggckov.F
	$(FFOLD) -c $(FFLAGS) -I$(CERNDIR)/include $(SFF)ggckov.F -o $(BIN)ggckov.o

$(BIN)graylc.o:      $(SFF)graylc.F
	$(FFOLD) -c $(FFLAGS) -I$(CERNDIR)/include $(SFF)graylc.F -o $(BIN)graylc.o

$(BIN)gtckov.o:      $(SFF)gtckov.F
	$(FFOLD) -c $(FFLAGS) -I$(CERNDIR)/include $(SFF)gtckov.F -o $(BIN)gtckov.o

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
	$(CXX_ORBIT) -c $(CFLAGSB) -D__BATCH__ $(IDLFLAGS) $(SCC)geant.C -o $(BIN)geant_c.o


$(BIN)job_c.o: 	 $(SCC)job.C
	$(CXX_ORBIT) -c $(CFLAGS) $(IDLFLAGS) $(SCC)job.C -o $(BIN)job_c.o

$(BIN)geant3_c.o: 	 $(SCC)geant3.C
	$(CXX_ORBIT) -c $(CFLAGS) -D__BATCH__ $(IDLFLAGS) $(SCC)geant3.C -o $(BIN)geant3_c.o

$(BIN)geant4_c.o:        $(SCC)geant4.C
	$(CXX_ORBIT) -c $(CFLAGS) -D__BATCH__ $(IDLFLAGS) -UG4VIS_USE $(SCC)geant4.C -o $(BIN)geant4_c.o

$(BIN)geant4.o: 	 $(SCC)geant4.C
	$(CXX) -c $(CFLAGS) -D__BATCH__ -UG4VIS_USE $(SCC)geant4.C -o $(BIN)geant4.o


$(BIN)event_c.o: 	 $(SCC)event.C
	$(CXX_ORBIT) -c $(CFLAGS) $(IDLFLAGS) $(SCC)event.C -o $(BIN)event_c.o

$(BIN)timeid_c.o:        $(SCC)timeid.C
	$(CXX_ORBIT) -c $(CFLAGSTOFSIM) $(IDLFLAGS) $(SCC)timeid.C -o $(BIN)timeid_c.o

$(BIN)timeid.o:        $(SCC)timeid.C
	$(CXX) -c $(CFLAGSTOFSIM) $(SCC)timeid.C -o $(BIN)timeid.o

$(BIN)timeid_server.o:        $(SCC)timeid.C
	$(CXX_ORBIT) -c $(CFLAGS) -D__CORBASERVER__ $(SCC)timeid.C -o $(BIN)timeid_server.o

$(BIN)producer.o: 	 $(SCC)producer.C
	$(CXX_ORBIT) -c -w $(CFLAGSB) $(IDLFLAGS) $(SCC)producer.C -o $(BIN)producer.o

$(BIN)client.o: 	 $(SCC)client.C
	$(CXX_ORBIT) -fpermissive -c -w $(CFLAGS) $(IDLFLAGS) $(SCC)client.C -o $(BIN)client.o

$(BIN)server.o: 	 $(SCC)server.C
	$(CXX_ORBIT)  -c -w $(CFLAGS) $(IDLFLAGS) $(SCC)server.C -o $(BIN)server.o

$(BIN)dbserver.o: 	 $(SCC)dbserver.C
	$(CXX_ORBIT)  -c -w $(CFLAGS) $(IDLFLAGS) $(SCC)dbserver.C -o $(BIN)dbserver.o

$(BIN)VCon_s.o: ../include/VCon_root.h

##################### ADDTIONAL (PHONY) TARGETS ##############################################
#checks if the directory AMSWD exist 
amswd:
	@if ! [ -d $(AMSWD) ]; then echo FATAL: the directory pointer by AMSWD = $(AMSWD) is missing !!!; exit 1;fi

depend:         $(BIN)Make-depend

touch_depend:
	touch $(BIN)Make-depend

$(BIN)Make-depend:    $(INCS)
		@if ! [ -d $(BIN) ] ; then mkdir -p $(BIN); fi
ifneq "$(shell du -a  $(BIN)Make-depend | grep -c  ake)" "1"
		 touch $(BIN)Make-depend
endif
		makedepend $(DEFINES) -f $(BIN)Make-depend  -Y../include  $(FILES) -p$(BIN)
		cat  $(BIN)Make-depend | sed -e 'sx/../CC/x/x1' > $(BIN)Make-depend.new
		rm -f $(BIN)Make-depend
	mv $(BIN)Make-depend.new $(BIN)Make-depend

special-links:
	@if ! [ -d $(AMSWD)/$(MARCH) ] ; then mkdir $(AMSWD)/$(MARCH); fi
	@ln -s $(AMSWD)/exe/$(MARCH) $(AMSWD)/$(MARCH)/exe
	@ln -s $(AMSWD)/lib/$(MARCH) $(AMSWD)/$(MARCH)/lib
	@ln -s $(AMSWD)/bin/$(MARCH) $(AMSWD)/$(MARCH)/bin

clean : 
	rm -f $(BIN)*.o 
	rm -f $(BIN)Make-depend 
	rm -f $(BIN)*.C
	rm -f $(BIN)*.c
	rm -f $(BIN)*.cc
	rm -f $(BIN)*.h
	rm -f $(STANDALONE) $(INTERACTIVE) $(CLIENT) $(SERVER)
	rm -f $(NTLD) $(SHLD)
	rm -f $(DPMBIN)*.o
cleanALL : 
	rm -rf $(AMSWD)/bin 
	rm -rf $(AMSWD)/lib
	rm -rf $(AMSWD)/exe 
	rm -rf $(AMSWD)/$(MARCH)
	rm -f ../include/root_methods*.h

cleanlibs:
	rm -f $(NTLD) $(SHLD)
	rm -f $(BIN)/root*.o
	cd ../ruby; gmake clean
	cd ../python; gmake clean
	cd ../install
all:
	$(MAKE) -j 
	$(MAKE) -j lib 
	$(MAKE) -j gxint
	$(MAKE) -i producer
	$(MAKE) -j -C../online
	$(MAKE) -j -C../online osdepend
	$(MAKE) -j -C../display
	$(MAKE) -j -C../display osdepend
allg4:
	$(MAKE) -j 
	$(MAKE) -j lib 
	$(MAKE) -j gxint
	$(MAKE) -i producer
	$(MAKE) -j -C../online
	$(MAKE) -j -C../online osdepend
	$(MAKE) -j -C../display
	$(MAKE) -j -C../display osdepend
ifndef SLC6system
endif
fast64:
	$(MAKE) -j -f Makefastntrd64

allp:
	$(MAKE) -j 
	$(MAKE) -j lib 
	$(MAKE) -j gxint
	$(MAKE)  -i producer
	$(MAKE) -j -C../online
	$(MAKE) -j -C../online osdepend
	$(MAKE) -j -C../display
	$(MAKE) -j -C../display osdepend
	$(MAKE) -j server
allold:
	$(MAKE) -j 
	$(MAKE) -j lib 
	$(MAKE)  -i producer
	$(MAKE) -j -C../online
	$(MAKE) -j -C../online osdepend
	$(MAKE) -j -C../display
	$(MAKE) -j -C../display osdepend

ifneq "$(shell cat /etc/redhat-release | grep -c 'Scientific Linux CERN SLC release 4')" "1"
	@echo "Please use SLC4 distribution to produce Linux 2.4 executables"
	#@rm -f $(STANDALONE) $(INTERACTIVE) $(CLIENT) $(SERVER)
endif


##### DEPENDENCIES #####
ifneq ($(MAKECMDGOALS),touch_depend)
-include $(BIN)Make-depend
endif

# DO NOT DELETE
