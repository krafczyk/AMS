# $Id: Makefile,v 1.13 2011/09/06 09:23:02 shaino Exp $
#
# Makefile to build executables for Tracker alignment with TB/CR
#
# Minuit2 component is required in libRoot.a
#
# bin/tbalign: Generate a TTree for TB alignment from AMSRoot files
# bin/pmalign: Generate a TTree for CR alignment from AMSRoot files
#
# bin/tbafit : Tracker alignment with tbalign.root files
# bin/pmafit : Tracker alignment with pmalign.root files
#
# 2010/12/1 SH

WDIR = $(AMSSRC)/tkalign
ARCH = $(shell $(ROOTSYS)/bin/root-config --arch)
HOST = $(shell hostname --domain)
DEFS = -D_PGTRACK_ -D__ROOTSHAREDLIBRARY__ -D_WDIR_=\"$(WDIR)\"
FLGS = -I$(ROOTSYS)/include -I../include -Iinclude
OPTS = -O3


# 32 bit and 64 bit specification

M32  = $(shell $(ROOTSYS)/bin/root-config --cflags | awk -F -m '{print $$2}' | awk '{print $$1}')
ifeq ($(M32),32)
  FLGS := $(FLGS) -m32
endif


# for icc compiler

ICC  = $(shell $(ROOTSYS)/bin/root-config --cxx | grep icc)
ifeq ($(ICC),icc)
  PARA = -D__X8664__ -D__AMSPARALLEL__ -openmp
  STAT = -static -static-intel
  IDIR = /opt/intel
  IVER = Compiler/11.1/073
  IOPT = -axsse4.2,ssse3
  CXX  = $(IDIR)/$(IVER)/bin/intel64/icc  $(IOPT) -vec-report0 $(PARA)
  LD   = $(IDIR)/$(IVER)/bin/intel64/icpc $(IOPT) $(PARA) $(STAT)
endif

# for CNAF static binary with linuxx8664gcc
ifeq ($(HOST),cr.cnaf.infn.it)
  LIBX = -lXrdClient -lXrd -lXrdNet -lXrdOuc -lXrdSys -Llib -lMinuit2 -lshift
  LIBR = -L$(ROOTSYS)/lib -lRoot -lfreetype -pthread -lpcre -ldl $(LIBX)
  LIBP = $(AMSWD)/lib/$(ARCH)/libntuple_slc4_PG.a 
  LD   = g++ -static

# for CERN static binary with linuxx8664icc
else
ifeq ($(HOST),cern.ch)
  LIBX = -lXrdClient -lXrd -lXrdNet -lXrdOuc -lXrdSys
  LIBR = -L$(ROOTSYS)/lib -lRoot -lfreetype -pthread -lpcre -lshift $(LIBX)
  LIBP = $(AMSWD)/lib/$(ARCH)/libntuple_slc4_PG.a

# for dynamic binary
else
  LIBR = $(shell $(ROOTSYS)/bin/root-config --libs) -lNetx -lMinuit -lMinuit2
  LIBP = $(AMSWD)/lib/$(ARCH)/ntuple_slc4_PG.so

endif
endif


MODS = trcls refit mfit tkmerge tdv

BINS = $(addprefix bin/, $(MODS))

all: $(BINS)

obj/%.o: CC/m_%.C
	@if ! [ -d obj ] ; then mkdir -p obj; fi
	$(CXX) -c -o $@ $(DEFS) $(FLGS) $(OPTS) $<

obj/tdv.o: CC/tdv.C
	@if ! [ -d obj ] ; then mkdir -p obj; fi
	$(CXX) -c -o $@ $(DEFS) $(FLGS) $(OPTS) $<

bin/%: obj/%.o $(LIBP)
	@if ! [ -d bin ] ; then mkdir -p bin; fi
	$(LD) -o $@ $< $(LIBP) $(LIBR)
	#readelf -d $@

clean:
	rm -f bin/* obj/*

bin/tbalign: CC/tbalign.C CC/tbpos.C
bin/tbafit : CC/tbafit.C  CC/tkalign.C
bin/tbaf   : CC/tbafit.C  CC/tkalign.C
bin/tbafm  : CC/tbafit.C  CC/tkalign.C
bin/tbafcp : CC/tbafit.C  CC/tkalign.C

bin/pmalign: CC/pmalign.C
bin/pmafit : CC/pmafit.C CC/tkalign.C
bin/pmaf   : CC/pmafit.C CC/tkalign.C
bin/pmafcp : CC/pmafit.C CC/tkalign.C

bin/pmrinv : CC/pmrinv.C

bin/tkmerge: CC/tkmerge.C

