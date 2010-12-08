# $Id: Makefile,v 1.7 2010/12/08 16:06:14 shaino Exp $
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
DEFS = -D_PGTRACK_ -D__ROOTSHAREDLIBRARY__ -D_WDIR_=\"$(WDIR)\"
FLGS = -I$(ROOTSYS)/include -I../include
OPTS = -O3

M32  = $(shell $(ROOTSYS)/bin/root-config --cflags | awk -F -m '{print $$2}' | awk '{print $$1}')
ifeq ($(M32),32)
  FLGS := $(FLGS) -m32
endif


ICC  = $(shell $(ROOTSYS)/bin/root-config --cxx | grep icc)
ifeq ($(ICC),icc)
  PARA = -D__X8664__ -D__AMSPARALLEL__ -openmp
  IDIR = /opt/intel
  IVER = Compiler/11.1/073
  IOPT = -axsse4.2,ssse3
  CXX  = $(IDIR)/$(IVER)/bin/intel64/icc  $(IOPT) -vec-report0 $(PARA)
  LD   = $(IDIR)/$(IVER)/bin/intel64/icpc $(IOPT) $(PARA)
endif

# Dinamic library
#LIBR = $(shell $(ROOTSYS)/bin/root-config --libs) -lNetx -lMinuit -lMinuit2

# Static library
LIBR = -L$(ROOTSYS)/lib -lRoot -lXrdClient -lcrypt -lfreetype -ldl -lpcre
LIBS = $(AMSWD)/lib/$(ARCH)/libntuple_slc4_PG.a $(LIBR)

MODS = tbalign tbafit tbaf tbafm tbafcp \
       pmalign pmafit pmaf pmafcp pmrinv tkmerge trqpar

BINS = $(addprefix bin/, $(MODS))

all: $(BINS)

%.o: CC/m_%.C
	$(CXX) -c -o $@ $(DEFS) $(FLGS) $(OPTS) $<

bin/%: %.o
	@if ! [ -d bin ] ; then mkdir -p bin; fi
	$(LD) -o $@ $< $(LIBS)
	rm -f $<

clean:
	rm -f bin/*

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

bin/trqpar : CC/trqpar.C
