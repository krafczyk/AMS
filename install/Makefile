#  $Id: Makefile,v 1.4 2003/12/17 13:39:59 mdelgado Exp $

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


#-- CALL OS SPECIFIC FILE-------------------------------------------

# Operating system dependence.
# include a file called Makefile.$(MY_OS)
#  this file has os dependent defs

MY_OS := $(shell uname)
ifeq ($(origin local_makefile), undefined)
        local_makefile = Makefile.$(MY_OS)
endif
include $(shell echo ${local_makefile})
#------------------------------------------------------------------------------

# Setup rules to compile source from $SFF or $SCC and put objects in $BIN
# default rules for object files 
# object files that have special flags are still defined below 
#
#    rules to compile o from F,f,C,c using the AMS directories
#                  $<  =  the prerequsite (the source code)
#                  $@  =  the target (the object file)

$(BIN)%.o : $(SFF)%.F
	$(FF) -c $(FFLAGS) -I$(CERNDIR)/include $< -o $@

$(BIN)%.o : $(SFF)%.f
	$(FF) -c $(FFLAGS) -I$(CERNDIR)/include $< -o $@

$(BIN)%.o : $(SCC)%.C
	$(CC) -c $(CFLAGS) $< -o $@

$(BIN)%.o : $(SCC)%.c
	$(CC) -c $(CFLAGS) $< -o $@

#------------------------------------------------------------------------------


IDLFLAGS=-D__CORBA__ -I$(BIN)

INCS= $(wildcard ../include/*.h)    


FILES= \
 $(SCC)id.C         $(SCC)node.C     $(SCC)snode.C        $(SCC)gbatch.C \
 $(SCC)gvolume.C    $(SCC)amsdbc.C   $(SCC)cern.C         $(SCC)gsen.C\
 $(SCC)point.C      $(SCC)upool.C    $(SCC)gmat.C         $(SCC)amsgobj.C\
 $(SCC)amsgeom.C    $(SCC)geant.C    $(SCC)geant3.C       $(SCC)mceventg.C\
 ../F/tkmccl.F      ../F/fint_my.F    ../F/geocoor.F      $(SCC)rnormx.C\
 $(SCC)trid.C       $(SCC)trdid.C    $(SCC)link.C         $(SCC)trrec.C\
 $(SCC)vtx.C        $(SCC)trdsim.C   $(SCC)trdrec.C       $(SCC)uzstat.C\
 ../F/amstkshc++.F ../F/rkms.F  ../F/mtx.F        ../F/tptcir.F      ../F/trafit.F\
 ../F/tcircf.F      ../F/tszfit.F     ../F/txyfit.F       $(SCC)job.C\
 $(SCC)event.C      $(SCC)cont.C      $(SCC)mccluster.C   $(SCC)trrawcluster.C\
 ../F/rndm.F        ../F/timel.F        ../F/sgif.F        $(SCC)tofrec02.C\
 $(SCC)tofonline02.C $(SCC)tofuser02.C  $(SCC)antirec02.C  $(SCC)tofsim02.C\
 $(SCC)beta.C        $(SCC)charge.C     $(SCC)particle.C   $(SCC)commons.C\
 $(SCC)apool.C       $(SCC)tofdbc02.C   $(SCC)tofcalib02.C $(SCC)tofid.C\
 $(SCC)timeid.C      $(SCC)io.C         $(SCC)trcalib.C    $(SCC)tralig.C\
 ../F/mtx2.F          ../F/e04ccf.f     $(SCC)trigger102.C $(SCC)trigger302.C\
 $(SCC)antidbc02.C   $(SCC)daqevt.C     $(SCC)astring.C    $(SCC)daqs2block.C\
 ../F/mfield.F       ../F/egzav.F        ../F/tkfit.F      $(SCC)tkdbc.C\
 $(SCC)trddbc.C      $(SCC)ntuple.C     $(SCC)root.C       $(BIN)rootdict.C\
 $(SCC)user.C        $(SCC)status.C    $(SCC)bcorr.C       $(SCC)ecaldbc.C\
 $(SCC)ecid.C        $(SCC)ecalrec.C   $(SCC)ecalcalib.C   $(SCC)richdbc.C\
 $(SCC)richrec.C     $(SCC)richid.C    $(SCC)richgeom.C    ../F/gtckov.F\
 ../F/graylc.F      ../F/cerenkov.F    $(SCC)producer.C   $(SCC)client.C\
 $(SCC)server.C    $(SCC)dbserver.C    $(SCC)astro.C      ../F/richlikebeta_lip.F\
 ../F/richpatt_lip.F ../F/richrec_lip.F

OBJECTS=\
 $(BIN)gbatch.o     $(BIN)commons.o    $(BIN)gvolume.o   $(BIN)id.o\
 $(BIN)node.o       $(BIN)snode.o      $(BIN)cern.o      $(BIN)amsdbc.o\
 $(BIN)gsen.o       $(BIN)mtx.o        $(BIN)point.o     $(BIN)amstkshc++.o $(BIN)rkms.o\
 $(BIN)sgif.o       $(BIN)upool.o      $(BIN)gmat.o      $(BIN)amsgobj.o\
 $(BIN)amsgeom.o    $(BIN)geant.o      $(BIN)geant3.o    $(BIN)mceventg.o\
 $(BIN)tkmccl.o     $(BIN)geocoor.o    $(BIN)rnormx.o    $(BIN)trid.o\
 $(BIN)trdid.o      $(BIN)link.o       $(BIN)trrec.o     $(BIN)vtx.o\
 $(BIN)trdsim.o     $(BIN)trdrec.o     $(BIN)uzstat.o    $(BIN)tptcir.o\
 $(BIN)trafit.o     $(BIN)tcircf.o     $(BIN)tszfit.o    $(BIN)txyfit.o\
 $(BIN)job.o        $(BIN)event.o      $(BIN)cont.o      $(BIN)trrawcluster.o\
 $(BIN)mccluster.o  $(BIN)rndm.o       $(BIN)tofrec02.o  $(BIN)tofonline02.o\
 $(BIN)tofuser02.o  $(BIN)antirec02.o  $(BIN)tofsim02.o  $(BIN)beta.o\
 $(BIN)charge.o     $(BIN)particle.o   $(BIN)apool.o     $(BIN)tofdbc02.o\
 $(BIN)tofcalib02.o $(BIN)tofid.o      $(BIN)timeid.o    $(BIN)io.o\
 $(BIN)trcalib.o    $(BIN)tralig.o     $(BIN)mtx2.o      $(BIN)e04ccf.o\
 $(BIN)trigger102.o $(BIN)trigger302.o $(BIN)antidbc02.o $(BIN)daqevt.o\
 $(BIN)astring.o    $(BIN)daqs2block.o $(BIN)mfield.o    $(BIN)egzav.o\
 $(BIN)tkfit.o      $(BIN)tkdbc.o      $(BIN)trddbc.o    $(BIN)ntuple.o\
 $(BIN)root.o       $(BIN)rootdict.o   $(BIN)user.o      $(BIN)status.o\
 $(BIN)bcorr.o      $(BIN)ecaldbc.o    $(BIN)ecid.o      $(BIN)ecalrec.o\
 $(BIN)ecalcalib.o  $(BIN)richdbc.o    $(BIN)richrec.o   $(BIN)richid.o\
 $(BIN)richgeom.o   $(BIN)gtckov.o     $(BIN)graylc.o    $(BIN)cerenkov.o\
 $(BIN)grfile.o     $(BIN)sbcomp.o     $(BIN)gskpho.o    $(BIN)flinit.o\
 $(BIN)fint_my.o    $(BIN)astro.o      $(BIN)richlikebeta_lip.o $(BIN)richpatt_lip.o\
 $(BIN)richrec_lip.o

OBJECTSI= $(BIN)gxint.o $(BIN)commons.o $(BIN)gvolume.o  $(BIN)id.o $(BIN)node.o $(BIN)snode.o $(BIN)cern.o $(BIN)amsdbc.o $(BIN)gsen.o  $(BIN)mtx.o $(BIN)point.o $(BIN)amstkshc++.o  $(BIN)rkms.o $(BIN)sgif.o $(BIN)upool.o $(BIN)gmat.o $(BIN)amsgobj.o $(BIN)amsgeom.o  $(BIN)geanti.o $(BIN)geant3i.o $(BIN)mceventg.o $(BIN)tkmccl.o $(BIN)rnormx.o  $(BIN)trid.o $(BIN)trdid.o $(BIN)link.o $(BIN)trrec.o $(BIN)vtx.o $(BIN)trdrec.o $(BIN)trdsim.o $(BIN)uzstat.o $(BIN)tptcir.o $(BIN)trafit.o $(BIN)tcircf.o $(BIN)tszfit.o $(BIN)txyfit.o  $(BIN)job.o $(BIN)event.o $(BIN)cont.o $(BIN)trrawcluster.o $(BIN)mccluster.o   $(BIN)tofrec02.o  $(BIN)tofonline02.o $(BIN)antirec02.o $(BIN)beta.o $(BIN)charge.o $(BIN)particle.o  $(BIN)gi.o $(BIN)gi2.o $(BIN)e04ccf.o $(BIN)tofdbc02.o $(BIN)apool.o $(BIN)tofsim02.o $(BIN)tofcalib02.o $(BIN)timeid.o $(BIN)io.o $(BIN)trcalib.o $(BIN)mtx2.o $(BIN)trigger102.o  $(BIN)trigger302.o $(BIN)antidbc02.o $(BIN)daqevt.o $(BIN)astring.o $(BIN)daqs2block.o $(BIN)mfield.o $(BIN)egzav.o $(BIN)tkdbc.o $(BIN)trddbc.o  $(BIN)ntuple.o $(BIN)user.o  $(BIN)tofid.o $(BIN)status.o $(BIN)bcorr.o $(BIN)tralig.o  $(BIN)tofuser02.o $(BIN)geocoor.o $(BIN)ecaldbc.o $(BIN)ecid.o $(BIN)ecalrec.o  $(BIN)tkfit.o $(BIN)richdbc.o $(BIN)richrec.o $(BIN)richid.o $(BIN)richgeom.o $(BIN)gtckov.o $(BIN)graylc.o $(BIN)cerenkov.o $(BIN)grfile.o $(BIN)ecalcalib.o $(BIN)fint_my.o $(BIN)root.o $(BIN)rootdict.o $(BIN)astro.o $(BIN)richlikebeta_lip.o $(BIN)richpatt_lip.o $(BIN)richrec_lip.o


OBJECTSC= \
 $(BIN)gbatch_c.o   $(BIN)commons.o     $(BIN)gvolume.o   $(BIN)id.o\
 $(BIN)node.o       $(BIN)snode.o       $(BIN)cern.o      $(BIN)amsdbc.o\
 $(BIN)gsen.o       $(BIN)mtx.o         $(BIN)point.o     $(BIN)amstkshc++.o $(BIN)rkms.o\
 $(BIN)sgif.o       $(BIN)upool.o       $(BIN)gmat.o      $(BIN)amsgobj.o\
 $(BIN)amsgeom.o    $(BIN)geant_c.o     $(BIN)geant3_c.o  $(BIN)mceventg.o\
 $(BIN)tkmccl.o     $(BIN)geocoor.o     $(BIN)rnormx.o    $(BIN)trid.o\
 $(BIN)trdid.o      $(BIN)link.o        $(BIN)trrec.o     $(BIN)vtx.o\
 $(BIN)trdsim.o     $(BIN)trdrec.o      $(BIN)uzstat.o    $(BIN)tptcir.o\
 $(BIN)trafit.o     $(BIN)tcircf.o      $(BIN)tszfit.o    $(BIN)txyfit.o\
 $(BIN)job_c.o      $(BIN)event_c.o     $(BIN)cont.o      $(BIN)trrawcluster.o\
 $(BIN)mccluster.o  $(BIN)rndm.o        $(BIN)tofrec02.o  $(BIN)tofonline02.o\
 $(BIN)tofuser02.o  $(BIN)antirec02.o   $(BIN)tofsim02.o  $(BIN)beta.o\
 $(BIN)charge.o     $(BIN)particle.o    $(BIN)apool.o     $(BIN)tofdbc02.o\
 $(BIN)tofcalib02.o $(BIN)tofid.o       $(BIN)timeid_c.o  $(BIN)io.o\
 $(BIN)trcalib.o    $(BIN)tralig.o      $(BIN)mtx2.o      $(BIN)e04ccf.o\
 $(BIN)trigger102.o $(BIN)trigger302.o  $(BIN)antidbc02.o $(BIN)daqevt.o\
 $(BIN)astring.o    $(BIN)daqs2block.o  $(BIN)mfield.o    $(BIN)egzav.o\
 $(BIN)tkfit.o      $(BIN)tkdbc.o       $(BIN)trddbc.o    $(BIN)ntuple.o\
 $(BIN)root.o       $(BIN)rootdict.o    $(BIN)user.o      $(BIN)status.o\
 $(BIN)bcorr.o      $(BIN)ecaldbc.o     $(BIN)ecid.o      $(BIN)ecalrec.o\
 $(BIN)ecalcalib.o  $(BIN)richdbc.o     $(BIN)richrec.o   $(BIN)richid.o\
 $(BIN)richgeom.o   $(BIN)gtckov.o      $(BIN)graylc.o    $(BIN)cerenkov.o\
 $(BIN)grfile.o     $(BIN)flinit.o      $(BIN)producer.o  $(BIN)client.o\
 $(BIN)fint_my.o    $(BIN)ins_dummy.o   $(BIN)astro.o     $(BIN)sbcomp.o\
 $(BIN)gskpho.o     $(BIN)richlikebeta_lip.o $(BIN)richpatt_lip.o $(BIN)richrec_lip.o



gbatch: $(EXE)gbatch.exe

$(EXE)gbatch.exe:	$(OBJECTS)
	$(LD) $(FLAGSSTATIC) -v -o  $(EXE)gbatch.exe $(OBJECTS)\
         $(TIMEL) $(LDFLAGSSTATIC)

shared :	$(BIN)root_rs.o ../include/root_RV.h ../include/root.h $(BIN)rootdict_s.o
	$(LD) -O  $(SOFLAGS) $(BIN)/root_rs.o $(BIN)/rootdict_s.o $(SHLDFLAGS) -o  $(SHLD)



gxint :	$(EXE)gxint.exe

$(EXE)gxint.exe:	$(OBJECTSI)
	$(LD) -v -o $(EXE)/gxint.exe  $(OBJECTSI) $(LDFLAGSI)
	chmod +x  $(EXE)/gxint.exe

$(BIN)gxint.o:	../CC/gxint.C
	$(CC) -c $(CFLAGS) ../CC/gxint.C -o $(BIN)/gxint.o

$(BIN)geanti.o:      ../CC/geant.C
	$(CC) -c $(CFLAGS) ../CC/geant.C -o $(BIN)/geanti.o

$(BIN)geant3i.o:      ../CC/geant3.C
	$(CC) -c $(CFLAGS) ../CC/geant3.C -o $(BIN)/geant3i.o

$(BIN)gi2.o:      ../CC/gi2.C
	$(CC) -c $(CFLAGS) ../CC/gi2.C -o $(BIN)/gi2.o

$(BIN)gi.o:      ../F/gi.F
	$(FF) -c $(FFLAGS) ../F/gi.F -o $(BIN)/gi.o


#-------------------------------------------------------------------------------
# these files do not use the default rules

$(BIN)rootdict.C:    ../include/linkdef.h  ../include/root_RV.h ../include/root.h
	@echo "Generating dictionary"
	$(ROOTSYS)/bin/rootcint    -f $(BIN)rootdict.C -c ../include/root_RV.h ../include/linkdef.h
	@echo "OK"

$(BIN)rootdict_s.C:    ../include/linkdef.h  ../include/root_RVS.h ../include/root.h ../include/root_methods.h
	@echo "Generating dictionary"
	$(ROOTSYS)/bin/rootcint  -f $(BIN)rootdict_s.C -c ../include/root_RVS.h ../include/linkdef.h
	@echo "OK"

$(BIN)rootdict.o:         $(BIN)rootdict.C
	$(CC) -c $(CFLAGS) $(BIN)rootdict.C -o $(BIN)rootdict.o 
 
$(BIN)rootdict_s.o:         $(BIN)rootdict_s.C
	$(CC) -c $(CFLAGS) $(BIN)rootdict_s.C -o $(BIN)rootdict_s.o 
 
$(BIN)ntuple.o:    	 $(SCC)ntuple.C

ifdef AMSMEMMONITOR
	$(CC) -c -D__MEMMONITOR__ $(CFLAGS) $(SCC)ntuple.C -o $(BIN)ntuple.o
else
	$(CC) -c $(CFLAGS) $(SCC)ntuple.C -o $(BIN)ntuple.o
endif

$(BIN)root_rs.o:    	 $(SCC)root.C ../include/root_methods.h
	$(CC) -c $(CFLAGS) -D__ROOTSHAREDLIBRARY__ $(SCC)root.C -o $(BIN)root_rs.o


ifeq  "$(shell uname)"  "SunOS" 

$(BIN)mtx.o:                   ../F/mtx77.F
	f77 -c $(FFLAGS)      ../F/mtx77.F -o $(BIN)mtx.o
endif
ifeq  "$(shell uname)"  "HP-UX" 

$(BIN)mtx.o:                   ../F/mtx77.F
	f77 -c $(FFLAGS)      ../F/mtx77.F -o $(BIN)mtx.o
endif
ifeq  "$(shell uname)"  "Linux" 

$(BIN)mtx.o:                   ../F/mtx77.F
	$(FF) -c $(FFLAGS)      ../F/mtx77.F -o $(BIN)mtx.o
endif
ifeq  "$(shell uname)"  "AIX" 

$(BIN)mtx.o:                   ../F/mtx.F
	xlf90 -c $(FFLAGS)      ../F/mtx.F -o $(BIN)mtx.o
endif
ifeq  "$(shell uname)"  "AIXold" 

$(BIN)mtx.o:                   ../F/mtx.F
	xlf90 -c $(FFLAGS)      ../F/mtx.F -o $(BIN)mtx.o
endif

ifeq  "$(shell uname)"  "OSF1" 

$(BIN)mtx.o:                   ../F/mtx.F
	f90 -c -free $(FFLAGS)      ../F/mtx.F -o $(BIN)mtx.o
endif


../F/flinit.F:      $(CERNSRCDIR)/src/geant321/fiface/flinit.F
	sed  "s?OPEN(UNIT=LUNERR,FILE='flukaerr.dat'?OPEN(UNIT=LUNERR,FILE='/dev/null'?g"  $(CERNSRCDIR)/src/geant321/fiface/flinit.F > ../F/flinit.F

../include/root_methods.h:      ../perl/root.perl
	perl ../perl/root.perl

../F/grfile.F:      $(CERNSRCDIR)/src/geant321/giopa/grfile.F
	sed  "s/NQUOT=1000/NQUOT=32000/g"  /$(CERNSRCDIR)/src/geant321/giopa/grfile.F > ../F/grfile.F

$(BIN)grfile.o:      ../F/grfile.F
	$(FF) -c $(FFLAGS) -I$(CERNDIR)/include ../F/grfile.F -o $(BIN)grfile.o

../F/gskpho.F:      $(CERNSRCDIR)/src/geant321/gtrak/gskpho.F
	sed  "s/N2-N1/NGPHOT\*10/g;s/MISSNG.GT.0/MISSNG.GT.NGPHOT\*10/g;s/MSSPOL.GT.0/MSSPOL.GT.NGPHOT\*10/g"  $(CERNSRCDIR)/src/geant321/gtrak/gskpho.F > ../F/gskpho.F

$(BIN)gskpho.o:      ../F/gskpho.F
	$(FF) -c $(FFLAGS) -I$(CERNDIR)/include ../F/gskpho.F -o $(BIN)gskpho.o

../F/sbcomp.F:      $(CERNSRCDIR)/src/geant321/peanut/sbcomp.F
	sed  "s/STOP 'BIMPCT'/write(*,*)'****SBCOMP BIMPCT STOP ****',R0TRAJ,BIMPCT,R1TRAJ/g"  $(CERNSRCDIR)/src/geant321/peanut/sbcomp.F > ../F/sbcomp.F

$(BIN)sbcomp.o:      ../F/sbcomp.F
	$(FF) -c $(FFLAGS) -I$(CERNDIR)/include -DCERNLIB_DOUBLE ../F/sbcomp.F -o $(BIN)sbcomp.o


$(BIN)flinit.o:      ../F/flinit.F
	$(FF) -c $(FFLAGS) -DCERNLIB_UNIX -I$(CERNDIR)/include ../F/flinit.F -o $(BIN)flinit.o

#---------------------------------------------------------------------------------

SERVER=$(EXE)amsprodserver.exe

server:	$(SERVER)

$(SERVER):          $(OBJECTCO) $(OBJECTSE) $(OBJECTSEAUX) $(BIN)server.o
	$(LD) -v -o $(SERVER) $(BIN)server.o  $(OBJECTSE) $(OBJECTSEAUX) $(OBJECTCO)  $(LDFLAGSSTATICIDL) 



CLIENT=$(EXE)gbatch-orbit.exe

producer:	$(CLIENT)


$(CLIENT):          $(OBJECTCO) $(OBJECTCL) $(OBJECTSC)
	$(LD) $(FLAGSSTATIC) -v -o $(CLIENT) $(OBJECTSC)  $(OBJECTCL) $(OBJECTCO) $(LDFLAGSSTATICIDL) $(TIMEL) $(LDFLAGSSTATIC) -pthread /usr/lib/ctype-info.o

$(OBJECTSE):         $(SRCSCPP)
	$(CPP)  -c $(CFLAGS) $(SRCSCPP) 
	mv *.o $(BIN)

$(OBJECTCO):         $(SRCS)
	$(CCC)  -c $(CFLAGS) $(SRCS) 
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
	$(CPP) -c $(CFLAGS) -D__ROOTA__ $(SCC)gbatch.C $(IDLFLAGS)  -o $(BIN)gbatch_c.o

$(BIN)geant_c.o: 	 $(SCC)geant.C
	$(CPP) -c $(CFLAGSB) $(IDLFLAGS) $(SCC)geant.C -o $(BIN)geant_c.o

$(BIN)job_c.o: 	 $(SCC)job.C
	$(CPP) -c $(CFLAGS) $(IDLFLAGS) $(SCC)job.C -o $(BIN)job_c.o

$(BIN)geant3_c.o: 	 $(SCC)geant3.C
	$(CPP) -c $(CFLAGS) $(IDLFLAGS) $(SCC)geant3.C -o $(BIN)geant3_c.o

$(BIN)event_c.o: 	 $(SCC)event.C
	$(CPP) -c $(CFLAGS) $(IDLFLAGS) $(SCC)event.C -o $(BIN)event_c.o

$(BIN)timeid_c.o:        $(SCC)timeid.C
	$(CPP) -c $(CFLAGS) $(IDLFLAGS) $(SCC)timeid.C -o $(BIN)timeid_c.o
           
$(BIN)producer.o: 	 $(SCC)producer.C
	$(CPP) -c -w $(CFLAGSB) $(IDLFLAGS) $(SCC)producer.C -o $(BIN)producer.o

$(BIN)client.o: 	 $(SCC)client.C
	$(CPP) -fpermissive -c -w $(CFLAGS) $(IDLFLAGS) $(SCC)client.C -o $(BIN)client.o

$(BIN)server.o: 	 $(SCC)server.C
	$(CPP)  -c -w $(CFLAGS) $(IDLFLAGS) $(SCC)server.C -o $(BIN)server.o

$(BIN)dbserver.o: 	 $(SCC)dbserver.C
	$(CPP)  -c -w $(CFLAGS) $(IDLFLAGS) $(SCC)dbserver.C -o $(BIN)dbserver.o


depend:         $(BIN)Make-depend
touch_depend:
		touch $(BIN)Make-depend
$(BIN)Make-depend:    $(INCS)
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
	rm -f $(BIN)*.h
	rm -f $(EXE)gbatch_r.exe
	rm -f $(EXE)gbatch.exe

##### DEPENDENCIES #####
ifneq ($(MAKECMDGOALS),touch_depend)
 include $(BIN)Make-depend
endif


