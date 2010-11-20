# $Id: Makefile,v 1.3 2010/11/20 15:16:39 shaino Exp $
#
# Makefile to build executables for Tracker alignment with TB/CR
#
# libMinuit2.so is required (ROOT v5.22.00j or higher is recommended)
#
# bin/tbalign: Generate a TTree for TB alignment from AMSRoot files
# bin/pmalign: Generate a TTree for CR alignment from AMSRoot files
#
# bin/tbafit : Tracker alignment with tbalign.root files
# bin/pmafit : Tracker alignment with pmalign.root files
#
# 2010/10/14 SH

WDIR = $(AMSSRC)/tkalign
ARCH = linux
DEFS = -D_PGTRACK_ -D__ROOTSHAREDLIBRARY__ -D_WDIR_=\"$(WDIR)\"
FLGS = -m32 -I$(ROOTSYS)/include -I../include
OPTS = -O3

LIBR = $(shell $(ROOTSYS)/bin/root-config --libs) -lNetx -lMinuit -lMinuit2
LIBS = $(AMSWD)/lib/$(ARCH)/libntuple_slc4_PG.a $(LIBR)

MODS = tbalign tbafit tbaf tbafm tbafcp \
       pmalign pmafit pmaf pmafcp pmrinv tkmerge

BINS = $(addprefix bin/, $(MODS))

all: $(BINS)

bin/%: CC/m_%.C
	@if ! [ -d bin ] ; then mkdir -p bin; fi
	$(CXX) -o $@ $(DEFS) $(FLGS) $(OPTS) $< $(LIBS)

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
